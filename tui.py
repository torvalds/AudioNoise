#!/usr/bin/env python3
"""
AudioNoise TUI - Simple Terminal UI for turning virtual "pots" and picking effects.

A minimal curses-based TUI as requested by Linus Torvalds:
"Some UI (whether T or G) for actually turning the virtual 'pots' and 
picking the effect would probably be interesting. Right now I just do 
it all manually. But only something simple."

Dependencies: Python 3.x with curses (standard library - no external deps)
Requirements: ffmpeg, ffplay, and the AudioNoise 'convert' binary

Usage:
    cd AudioNoise
    python3 tui.py

License: GPL-2.0 (same as AudioNoise)
"""

import curses
import subprocess
import os
import sys
from pathlib import Path

SAMPLE_RATE = 48000
SAMPLE_FORMAT = 's32le'
CHANNELS = 'mono'

EFFECTS = {
    'flanger': {
        'defaults': [0.6, 0.6, 0.6, 0.6],
        'pots': ['Depth', 'Rate', 'Feedback', 'Mix'],
        'desc': 'Modulated delay - jet-plane swoosh'
    },
    'echo': {
        'defaults': [0.3, 0.3, 0.3, 0.3],
        'pots': ['Delay', 'Feedback', 'Mix', 'Tone'],
        'desc': 'Delay loop up to 1.25 seconds'
    },
    'fm': {
        'defaults': [0.25, 0.25, 0.5, 0.5],
        'pots': ['Mod Depth', 'Mod Rate', 'Carrier', 'Mix'],
        'desc': 'Frequency modulation synthesis'
    },
    'am': {
        'defaults': [0.5, 0.5, 0.5, 0.5],
        'pots': ['Depth', 'Rate', 'Shape', 'Mix'],
        'desc': 'Amplitude modulation'
    },
    'phaser': {
        'defaults': [0.3, 0.3, 0.5, 0.5],
        'pots': ['Depth', 'Rate', 'Stages', 'Feedback'],
        'desc': 'All-pass filter sweep'
    },
    'discont': {
        'defaults': [0.8, 0.1, 0.2, 0.2],
        'pots': ['Pitch', 'Rate', 'Blend', 'Mix'],
        'desc': 'Pitch shift via crossfade'
    },
}

EFFECT_NAMES = list(EFFECTS.keys())


class AudioNoiseTUI:
    def __init__(self, stdscr):
        self.stdscr = stdscr
        self.effect_idx = 0
        self.pot_idx = 0
        self.pot_values = {name: list(e['defaults']) for name, e in EFFECTS.items()}
        self.status = ''
        self.status_ok = True
        
        curses.curs_set(0)
        curses.start_color()
        curses.use_default_colors()
        curses.init_pair(1, curses.COLOR_CYAN, -1)
        curses.init_pair(2, curses.COLOR_GREEN, -1)
        curses.init_pair(3, curses.COLOR_YELLOW, -1)
        curses.init_pair(4, curses.COLOR_RED, -1)
        
        self._check_environment()
    
    def _check_environment(self):
        if not Path('convert').exists() and not Path('./convert').exists():
            self.status = "Warning: 'convert' not found. Run 'make convert' first."
            self.status_ok = False
        elif not Path('input.raw').exists():
            mp3_files = list(Path('.').glob('*.mp3'))
            if mp3_files:
                self.status = f"Will convert {mp3_files[0].name} to input.raw"
            else:
                self.status = "No input.raw or .mp3 found"
                self.status_ok = False
        else:
            self.status = "Ready - press 'p' to process, 'q' to quit"
    
    @property
    def effect(self):
        return EFFECT_NAMES[self.effect_idx]
    
    @property
    def effect_data(self):
        return EFFECTS[self.effect]
    
    @property
    def pots(self):
        return self.pot_values[self.effect]
    
    def draw(self):
        self.stdscr.clear()
        h, w = self.stdscr.getmaxyx()
        
        if h < 18 or w < 50:
            self.stdscr.addstr(0, 0, "Terminal too small (need 50x18)")
            self.stdscr.refresh()
            return
        
        title = "═══ AUDIONOISE TUI ═══"
        self.stdscr.addstr(0, (w - len(title)) // 2, title, 
                          curses.color_pair(1) | curses.A_BOLD)
        
        self.stdscr.addstr(2, 2, "EFFECT:", curses.A_BOLD)
        for i, name in enumerate(EFFECT_NAMES):
            y = 3 + i
            selected = i == self.effect_idx
            
            if selected:
                marker = ">"
                attr = curses.color_pair(2) | curses.A_BOLD
            else:
                marker = " "
                attr = curses.A_DIM
            
            self.stdscr.addstr(y, 2, f" {marker} {name.upper()}", attr)
        
        self.stdscr.addstr(3, 20, self.effect_data['desc'], curses.A_DIM)
        
        self.stdscr.addstr(10, 2, "POTS:", curses.A_BOLD)
        
        pot_names = self.effect_data['pots']
        for i in range(4):
            y = 11 + i
            selected = i == self.pot_idx
            value = self.pots[i]
            name = pot_names[i]
            
            if selected:
                attr = curses.color_pair(3) | curses.A_BOLD
            else:
                attr = curses.A_NORMAL
            
            self.stdscr.addstr(y, 2, f" {name:12}", attr)
            
            bar_width = min(20, w - 30)
            filled = int(value * bar_width)
            bar = "#" * filled + "-" * (bar_width - filled)
            
            bar_attr = curses.color_pair(2) if selected else curses.A_DIM
            self.stdscr.addstr(y, 16, f"[{bar}]", bar_attr)
            self.stdscr.addstr(y, 18 + bar_width, f" {value:.2f}", attr)
        
        self.stdscr.addstr(16, 2, "Keys:", curses.color_pair(3))
        self.stdscr.addstr(16, 8, "Up/Down=effect  Tab=pot  Left/Right=value  p=play  r=reset  q=quit", curses.A_DIM)
        
        status_attr = curses.color_pair(2) if self.status_ok else curses.color_pair(4)
        self.stdscr.addstr(h - 1, 0, self.status[:w-1], status_attr)
        
        self.stdscr.refresh()
    
    def process_and_play(self):
        effect = self.effect
        pots = self.pots
        
        self.status = f"Processing {effect}..."
        self.status_ok = True
        self.draw()
        
        if not Path('input.raw').exists():
            mp3_files = list(Path('.').glob('*.mp3'))
            if not mp3_files:
                self.status = "Error: No input file found"
                self.status_ok = False
                return
            
            try:
                subprocess.run([
                    'ffmpeg', '-y', '-v', 'fatal',
                    '-i', str(mp3_files[0]),
                    '-f', SAMPLE_FORMAT, '-ar', str(SAMPLE_RATE), '-ac', '1',
                    'input.raw'
                ], check=True)
            except (subprocess.CalledProcessError, FileNotFoundError) as e:
                self.status = f"Error converting input: {e}"
                self.status_ok = False
                return
        
        convert_path = './convert' if Path('./convert').exists() else 'convert'
        if not Path(convert_path).exists():
            self.status = "Error: 'convert' not found - run 'make convert'"
            self.status_ok = False
            return
        
        try:
            with open('input.raw', 'rb') as infile, open('output.raw', 'wb') as outfile:
                result = subprocess.run(
                    [convert_path, effect] + [f'{p:.2f}' for p in pots],
                    stdin=infile,
                    stdout=outfile,
                    stderr=subprocess.PIPE,
                    check=True
                )
        except subprocess.CalledProcessError as e:
            self.status = f"Processing failed: {e.stderr.decode() if e.stderr else e}"
            self.status_ok = False
            return
        except FileNotFoundError:
            self.status = "Error: convert binary not found"
            self.status_ok = False
            return
        
        try:
            subprocess.Popen([
                'ffplay', '-v', 'fatal', '-nodisp', '-autoexit',
                '-f', SAMPLE_FORMAT, '-ar', str(SAMPLE_RATE), 
                '-ch_layout', CHANNELS, '-i', 'output.raw'
            ], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            
            self.status = f"Playing: {effect} [{', '.join(f'{p:.2f}' for p in pots)}]"
            self.status_ok = True
        except FileNotFoundError:
            self.status = "Processed output.raw but ffplay not found for playback"
            self.status_ok = False
    
    def reset_pots(self):
        self.pot_values[self.effect] = list(self.effect_data['defaults'])
        self.status = f"Reset {self.effect} to defaults"
        self.status_ok = True
    
    def run(self):
        while True:
            self.draw()
            
            key = self.stdscr.getch()
            
            if key == ord('q') or key == ord('Q'):
                break
            elif key == curses.KEY_UP:
                self.effect_idx = (self.effect_idx - 1) % len(EFFECT_NAMES)
            elif key == curses.KEY_DOWN:
                self.effect_idx = (self.effect_idx + 1) % len(EFFECT_NAMES)
            elif key == ord('\t'):
                self.pot_idx = (self.pot_idx + 1) % 4
            elif key == curses.KEY_LEFT:
                self.pots[self.pot_idx] = max(0.0, self.pots[self.pot_idx] - 0.05)
            elif key == curses.KEY_RIGHT:
                self.pots[self.pot_idx] = min(1.0, self.pots[self.pot_idx] + 0.05)
            elif key == ord('p') or key == ord('P'):
                self.process_and_play()
            elif key == ord('r') or key == ord('R'):
                self.reset_pots()


def main():
    print("AudioNoise TUI - press 'q' to quit")
    try:
        curses.wrapper(lambda stdscr: AudioNoiseTUI(stdscr).run())
    except KeyboardInterrupt:
        pass
    print("Goodbye!")


if __name__ == '__main__':
    main()
