#!/usr/bin/env python3
"""
AudioNoise TUI - Real-time control interface for effects and pots.

Implements the --control= interface from commit ea71138 for dynamic
pot adjustment while audio is playing.

Features:
- Dynamically reads effects and defaults from Makefile
- Real-time pot control via pXYY commands
- Visual knobs with rotating pointers
- Uses aplay -B 100 for ~100ms latency

Usage:
    cd AudioNoise
    python3 tui.py

License: GPL-2.0
"""

import curses
import subprocess
import os
import sys
import signal
import shutil
import re
from pathlib import Path

SAMPLE_RATE = 48000

# Known pot names for effects (fallback: Pot1-4)
POT_NAMES = {
    'flanger':     ['Depth', 'Rate', 'Fback', 'Mix'],
    'echo':        ['Delay', 'Fback', 'Mix', 'Tone'],
    'fm':          ['Depth', 'Rate', 'Carr', 'Mix'],
    'am':          ['Depth', 'Rate', 'Shape', 'Mix'],
    'phaser':      ['Depth', 'Rate', 'Stage', 'Fback'],
    'discont':     ['Pitch', 'Rate', 'Blend', 'Mix'],
    'distortion':  ['Drive', 'Tone', 'Level', 'Mix'],
    'tube':        ['Drive', 'Tone', 'Mix', 'Bias'],
    'growlingbass': ['Sub', 'Odd', 'Even', 'Tone'],
}


def parse_makefile():
    """Parse Makefile to get effects and defaults dynamically."""
    effects = {}
    effect_order = []
    
    makefile = Path('Makefile')
    if not makefile.exists():
        # Fallback if no Makefile
        return {
            'flanger':    [60, 60, 60, 60],
            'echo':       [30, 30, 30, 30],
            'phaser':     [30, 30, 50, 50],
        }, ['flanger', 'echo', 'phaser']
    
    content = makefile.read_text()
    
    # Find effects list: "effects = flanger echo fm ..."
    match = re.search(r'^effects\s*=\s*(.+)$', content, re.MULTILINE)
    if match:
        effect_order = match.group(1).split()
    
    # Find defaults: "flanger_defaults = 0.6 0.6 0.6 0.6"
    for effect in effect_order:
        pattern = rf'^{re.escape(effect)}_defaults\s*=\s*(.+)$'
        match = re.search(pattern, content, re.MULTILINE)
        if match:
            floats = [float(x) for x in match.group(1).split()[:4]]
            defaults = [int(f * 100) for f in floats]
            while len(defaults) < 4:
                defaults.append(50)
            effects[effect] = defaults
        else:
            effects[effect] = [50, 50, 50, 50]
    
    return effects, effect_order


def get_knob(value):
    """Return knob visual for value 0-99."""
    pointers = ['↙', '←', '↖', '↑', '↗', '→', '↘']
    ptr_idx = int(value * 6 / 99) if value > 0 else 0
    pointer = pointers[min(ptr_idx, 6)]
    return [
        "╭───╮",
        f"│ {pointer} │",
        "╰───╯",
    ]


class AudioNoiseTUI:
    def __init__(self, stdscr):
        self.stdscr = stdscr
        
        # Load effects from Makefile
        defaults, self.effect_order = parse_makefile()
        self.effects = {name: list(vals) for name, vals in defaults.items()}
        self.pot_values = {name: list(vals) for name, vals in defaults.items()}
        
        self.effect_idx = 0
        self.pot_idx = 0
        self.status = ''
        self.playing = False
        self.proc = None
        self.player = None
        self.control_write = None
        
        # Detect audio player
        if shutil.which('aplay'):
            self.player_cmd = 'aplay'
        elif shutil.which('ffplay'):
            self.player_cmd = 'ffplay'
        else:
            self.player_cmd = None
        
        curses.curs_set(0)
        curses.start_color()
        curses.use_default_colors()
        curses.init_pair(1, curses.COLOR_CYAN, -1)
        curses.init_pair(2, curses.COLOR_GREEN, -1)
        curses.init_pair(3, curses.COLOR_YELLOW, -1)
        curses.init_pair(4, curses.COLOR_RED, -1)
        curses.init_pair(5, curses.COLOR_MAGENTA, -1)
        
        self._check_environment()
    
    def _check_environment(self):
        if not Path('./convert').exists():
            self.status = "Run 'make convert' first"
        elif not self.player_cmd:
            self.status = "No audio player (need aplay or ffplay)"
        elif not Path('input.raw').exists():
            mp3 = list(Path('.').glob('*.mp3'))
            if mp3:
                self.status = f"Converting {mp3[0].name}..."
                try:
                    subprocess.run([
                        'ffmpeg', '-y', '-v', 'fatal', '-i', str(mp3[0]),
                        '-f', 's32le', '-ar', str(SAMPLE_RATE), '-ac', '1', 'input.raw'
                    ], check=True)
                    self.status = "Ready"
                except:
                    self.status = "ffmpeg conversion failed"
            else:
                self.status = "No input.raw or .mp3"
        else:
            self.status = "Ready"
    
    @property
    def effect(self):
        return self.effect_order[self.effect_idx]
    
    @property
    def pots(self):
        return self.pot_values[self.effect]
    
    @property
    def pot_names(self):
        return POT_NAMES.get(self.effect, ['Pot 1', 'Pot 2', 'Pot 3', 'Pot 4'])
    
    def send_pot(self, pot_idx, value):
        """Send pot command: pXYY\\n"""
        if self.control_write and self.playing:
            cmd = f"p{pot_idx}{value:02d}\n"
            try:
                os.write(self.control_write, cmd.encode())
            except:
                pass
    
    def start_playback(self):
        if self.playing:
            self.stop_playback()
        
        if not Path('input.raw').exists() or not self.player_cmd:
            return
        
        ctrl_read, ctrl_write = os.pipe()
        self.control_write = ctrl_write
        
        pots_float = [f"{v/100:.2f}" for v in self.pots]
        
        try:
            input_fd = os.open('input.raw', os.O_RDONLY)
            
            self.proc = subprocess.Popen(
                ['./convert', f'--control={ctrl_read}', self.effect] + pots_float,
                stdin=input_fd,
                stdout=subprocess.PIPE,
                stderr=subprocess.DEVNULL,
                pass_fds=(ctrl_read,)
            )
            os.close(input_fd)
            os.close(ctrl_read)
            
            if self.player_cmd == 'aplay':
                self.player = subprocess.Popen(
                    ['aplay', '-c1', '-r', str(SAMPLE_RATE), '-f', 's32', '-B', '100', '-q'],
                    stdin=self.proc.stdout,
                    stdout=subprocess.DEVNULL,
                    stderr=subprocess.DEVNULL
                )
            else:
                self.player = subprocess.Popen(
                    ['ffplay', '-v', 'fatal', '-nodisp', '-autoexit',
                     '-f', 's32le', '-ar', str(SAMPLE_RATE), '-ch_layout', 'mono', '-i', 'pipe:0'],
                    stdin=self.proc.stdout,
                    stdout=subprocess.DEVNULL,
                    stderr=subprocess.DEVNULL
                )
            
            self.playing = True
            latency = "~100ms" if self.player_cmd == 'aplay' else "high latency"
            self.status = f"Playing {self.effect} ({latency})"
            
        except Exception as e:
            self.status = f"Error: {e}"
            if self.control_write:
                os.close(self.control_write)
                self.control_write = None
    
    def stop_playback(self):
        if self.control_write:
            try:
                os.close(self.control_write)
            except:
                pass
            self.control_write = None
        
        for p in [self.player, self.proc]:
            if p:
                try:
                    p.terminate()
                    p.wait(timeout=0.5)
                except:
                    pass
        
        self.player = None
        self.proc = None
        self.playing = False
        self.status = "Stopped"
    
    def check_playback(self):
        if self.playing and self.proc and self.proc.poll() is not None:
            self.stop_playback()
            self.status = "Finished"
    
    def draw(self):
        self.stdscr.erase()
        h, w = self.stdscr.getmaxyx()
        
        if h < 20 or w < 60:
            self.stdscr.addstr(0, 0, "Terminal too small (need 60x20)")
            self.stdscr.noutrefresh()
            curses.doupdate()
            return
        
        # Title
        title = "═══════════ AUDIONOISE ═══════════"
        self.stdscr.addstr(0, (w - len(title)) // 2, title, curses.color_pair(1) | curses.A_BOLD)
        
        # Effect selector with scrolling
        self.stdscr.addstr(2, 3, "EFFECT", curses.A_BOLD | curses.color_pair(1))
        
        max_visible = min(9, len(self.effect_order))
        start = max(0, min(self.effect_idx - max_visible // 2, 
                          len(self.effect_order) - max_visible))
        
        for i in range(max_visible):
            idx = start + i
            if idx >= len(self.effect_order):
                break
            y = 3 + i
            name = self.effect_order[idx]
            if idx == self.effect_idx:
                display = f"▶ {name.upper()}"[:14]
                self.stdscr.addstr(y, 3, display, curses.color_pair(2) | curses.A_BOLD)
            else:
                display = f"  {name}"[:14]
                self.stdscr.addstr(y, 3, display, curses.A_DIM)
        
        # Knobs
        knob_x = 20
        knob_spacing = 10
        
        self.stdscr.addstr(2, knob_x, "──── POTS ────", curses.A_BOLD | curses.color_pair(1))
        
        for i in range(4):
            x = knob_x + (i * knob_spacing)
            selected = (i == self.pot_idx)
            val = self.pots[i]
            
            knob = get_knob(val)
            color = curses.color_pair(3) if selected else curses.A_NORMAL
            
            for j, line in enumerate(knob):
                self.stdscr.addstr(4 + j, x, line, color | (curses.A_BOLD if selected else 0))
            
            self.stdscr.addstr(7, x + 1, f"{val:02d}", 
                             curses.color_pair(2) if selected else curses.A_DIM)
            
            label = self.pot_names[i][:5]
            self.stdscr.addstr(8, x, f"{label:^5}", 
                             curses.color_pair(5) if selected else curses.A_DIM)
        
        # Playing indicator
        if self.playing:
            self.stdscr.addstr(2, w - 16, "♪♫ PLAYING ♫♪", 
                             curses.color_pair(4) | curses.A_BOLD | curses.A_BLINK)
        
        # Info box
        box_y = 13
        self.stdscr.addstr(box_y, 3, "┌" + "─" * 54 + "┐", curses.color_pair(1))
        self.stdscr.addstr(box_y + 1, 3, "│", curses.color_pair(1))
        
        info = f" {self.effect.upper()}: "
        for i, name in enumerate(self.pot_names[:4]):
            info += f"{name[:5]}={self.pots[i]:02d} "
        self.stdscr.addstr(box_y + 1, 4, info[:54].ljust(54), curses.A_BOLD)
        self.stdscr.addstr(box_y + 1, 58, "│", curses.color_pair(1))
        self.stdscr.addstr(box_y + 2, 3, "└" + "─" * 54 + "┘", curses.color_pair(1))
        
        # Controls
        ctrl_y = 17
        self.stdscr.addstr(ctrl_y, 3, "CONTROLS", curses.color_pair(1) | curses.A_BOLD)
        self.stdscr.addstr(ctrl_y + 1, 3, "↑/↓", curses.color_pair(3))
        self.stdscr.addstr(ctrl_y + 1, 7, "effect", curses.A_DIM)
        self.stdscr.addstr(ctrl_y + 1, 16, "Tab", curses.color_pair(3))
        self.stdscr.addstr(ctrl_y + 1, 20, "pot", curses.A_DIM)
        self.stdscr.addstr(ctrl_y + 1, 26, "←/→", curses.color_pair(3))
        self.stdscr.addstr(ctrl_y + 1, 30, "value", curses.A_DIM)
        self.stdscr.addstr(ctrl_y + 1, 38, "SPACE", curses.color_pair(3))
        self.stdscr.addstr(ctrl_y + 1, 44, "play/stop", curses.A_DIM)
        self.stdscr.addstr(ctrl_y + 1, 54, "r", curses.color_pair(3))
        self.stdscr.addstr(ctrl_y + 1, 56, "reset", curses.A_DIM)
        
        # Status bar
        status_color = curses.color_pair(2) if self.playing else curses.A_NORMAL
        status_text = f" {self.status}"[:w-1].ljust(w-1)
        self.stdscr.addstr(h - 1, 0, status_text, status_color | curses.A_REVERSE)
        
        self.stdscr.noutrefresh()
        curses.doupdate()
    
    def reset_pots(self):
        self.pot_values[self.effect] = list(self.effects[self.effect])
        if self.playing:
            for i, v in enumerate(self.pots):
                self.send_pot(i, v)
        self.status = f"Reset {self.effect}"
    
    def change_effect(self, direction):
        new_idx = (self.effect_idx + direction) % len(self.effect_order)
        self.effect_idx = new_idx
        if self.playing:
            self.stop_playback()
            self.start_playback()
    
    def run(self):
        self.stdscr.timeout(100)
        
        while True:
            self.check_playback()
            self.draw()
            
            try:
                key = self.stdscr.getch()
            except:
                key = -1
            
            if key == -1:
                continue
            elif key == ord('q') or key == ord('Q'):
                break
            elif key == curses.KEY_UP:
                self.change_effect(-1)
            elif key == curses.KEY_DOWN:
                self.change_effect(1)
            elif key == ord('\t'):
                self.pot_idx = (self.pot_idx + 1) % 4
            elif key == curses.KEY_LEFT:
                old = self.pots[self.pot_idx]
                self.pots[self.pot_idx] = max(0, old - 5)
                if self.pots[self.pot_idx] != old:
                    self.send_pot(self.pot_idx, self.pots[self.pot_idx])
            elif key == curses.KEY_RIGHT:
                old = self.pots[self.pot_idx]
                self.pots[self.pot_idx] = min(99, old + 5)
                if self.pots[self.pot_idx] != old:
                    self.send_pot(self.pot_idx, self.pots[self.pot_idx])
            elif key == ord(' '):
                if self.playing:
                    self.stop_playback()
                else:
                    self.start_playback()
            elif key == ord('r') or key == ord('R'):
                self.reset_pots()
        
        self.stop_playback()


def main():
    signal.signal(signal.SIGINT, lambda s, f: sys.exit(0))
    try:
        curses.wrapper(lambda stdscr: AudioNoiseTUI(stdscr).run())
    except KeyboardInterrupt:
        pass


if __name__ == '__main__':
    main()
