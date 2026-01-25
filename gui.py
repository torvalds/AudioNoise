#!/usr/bin/env python3
"""
AudioNoise GTK GUI - Professional Guitar Pedal Interface

Requires: PyGObject (GTK 3)
    sudo apt install python3-gi python3-gi-cairo gir1.2-gtk-3.0

Usage:
    cd AudioNoise
    python3 gui.py

License: GPL-2.0
"""

import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk, Gdk, GLib
import cairo
import subprocess
import os
import shutil
import re
import math
from pathlib import Path

SAMPLE_RATE = 48000

COLORS = {
    'enclosure': (0.12, 0.12, 0.12),
    'knob_indicator': (0.95, 0.95, 0.95),
    'knob_selected': (0.83, 0.69, 0.22),
    'led_off': (0.25, 0.0, 0.0),
    'led_on': (1.0, 0.1, 0.1),
    'stomp_up': (0.28, 0.28, 0.28),
    'stomp_down': (0.20, 0.20, 0.20),
    'text': (0.75, 0.75, 0.75),
    'text_bright': (1.0, 1.0, 1.0),
    'accent': (0.83, 0.69, 0.22),
    'tick': (0.35, 0.35, 0.35),
}

POT_NAMES = {
    'flanger':      ['DEPTH', 'RATE', 'FBACK', 'MIX'],
    'echo':         ['DELAY', 'FBACK', 'MIX', 'TONE'],
    'fm':           ['DEPTH', 'RATE', 'CARR', 'MIX'],
    'am':           ['DEPTH', 'RATE', 'SHAPE', 'MIX'],
    'phaser':       ['DEPTH', 'RATE', 'STAGE', 'FBACK'],
    'discont':      ['PITCH', 'RATE', 'BLEND', 'MIX'],
    'distortion':   ['DRIVE', 'TONE', 'LEVEL', 'MIX'],
    'tube':         ['DRIVE', 'TONE', 'BASS', 'TREBLE'],
    'growlingbass': ['SUB', 'ODD', 'EVEN', 'TONE'],
}


def parse_makefile():
    effects = {}
    effect_order = []
    makefile = Path('Makefile')
    if not makefile.exists():
        return {'flanger': [60, 60, 60, 60]}, ['flanger']
    content = makefile.read_text()
    match = re.search(r'^effects\s*=\s*(.+)$', content, re.MULTILINE)
    if match:
        effect_order = match.group(1).split()
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


class Knob(Gtk.DrawingArea):
    """Rotary knob with unlimited smooth dragging."""

    def __init__(self, label="KNOB", value=50, on_change=None):
        super().__init__()
        self.value = value
        self.label = label
        self.on_change = on_change
        self.dragging = False
        self.selected = False
        self.hover = False
        self.accumulated_dx = 0  # Track total drag distance

        self.set_size_request(100, 130)
        self.set_can_focus(True)
        self.add_events(
            Gdk.EventMask.BUTTON_PRESS_MASK |
            Gdk.EventMask.BUTTON_RELEASE_MASK |
            Gdk.EventMask.POINTER_MOTION_MASK |
            Gdk.EventMask.SCROLL_MASK |
            Gdk.EventMask.ENTER_NOTIFY_MASK |
            Gdk.EventMask.LEAVE_NOTIFY_MASK
        )

        self.connect('draw', self.on_draw)
        self.connect('button-press-event', self.on_button_press)
        self.connect('button-release-event', self.on_button_release)
        self.connect('motion-notify-event', self.on_motion)
        self.connect('scroll-event', self.on_scroll)
        self.connect('enter-notify-event', self.on_enter)
        self.connect('leave-notify-event', self.on_leave)

    def on_draw(self, widget, cr):
        width = self.get_allocated_width()
        height = self.get_allocated_height()
        cx = width / 2
        cy = 50
        radius = 32

        cr.set_source_rgb(*COLORS['enclosure'])
        cr.rectangle(0, 0, width, height)
        cr.fill()

        if self.selected:
            gradient = cairo.RadialGradient(cx, cy, radius - 5, cx, cy, radius + 18)
            gradient.add_color_stop_rgba(0, *COLORS['knob_selected'], 0.4)
            gradient.add_color_stop_rgba(1, *COLORS['knob_selected'], 0.0)
            cr.set_source(gradient)
            cr.arc(cx, cy, radius + 18, 0, 2 * math.pi)
            cr.fill()

        cr.set_line_width(1.5)
        for i in range(11):
            angle = math.radians(225 - i * 27)
            x1 = cx + math.cos(angle) * (radius + 4)
            y1 = cy - math.sin(angle) * (radius + 4)
            x2 = cx + math.cos(angle) * (radius + 10)
            y2 = cy - math.sin(angle) * (radius + 10)
            tick_value = i * 10
            if abs(self.value - tick_value) < 5:
                cr.set_source_rgb(*COLORS['accent'])
            else:
                cr.set_source_rgb(*COLORS['tick'])
            cr.move_to(x1, y1)
            cr.line_to(x2, y2)
            cr.stroke()

        cr.set_source_rgba(0, 0, 0, 0.5)
        cr.arc(cx + 2, cy + 2, radius, 0, 2 * math.pi)
        cr.fill()

        gradient = cairo.RadialGradient(cx - 10, cy - 10, 0, cx, cy, radius)
        if self.selected:
            gradient.add_color_stop_rgb(0, 0.38, 0.33, 0.22)
            gradient.add_color_stop_rgb(0.7, 0.24, 0.21, 0.14)
            gradient.add_color_stop_rgb(1, 0.14, 0.12, 0.08)
        elif self.hover:
            gradient.add_color_stop_rgb(0, 0.36, 0.36, 0.36)
            gradient.add_color_stop_rgb(1, 0.16, 0.16, 0.16)
        else:
            gradient.add_color_stop_rgb(0, 0.30, 0.30, 0.30)
            gradient.add_color_stop_rgb(1, 0.10, 0.10, 0.10)
        cr.set_source(gradient)
        cr.arc(cx, cy, radius, 0, 2 * math.pi)
        cr.fill()

        cr.set_source_rgba(1, 1, 1, 0.08)
        cr.set_line_width(1)
        cr.arc(cx, cy, radius - 1, math.radians(200), math.radians(340))
        cr.stroke()

        inner_radius = radius - 9
        gradient = cairo.RadialGradient(cx - 4, cy - 4, 0, cx, cy, inner_radius)
        gradient.add_color_stop_rgb(0, 0.10, 0.10, 0.10)
        gradient.add_color_stop_rgb(1, 0.03, 0.03, 0.03)
        cr.set_source(gradient)
        cr.arc(cx, cy, inner_radius, 0, 2 * math.pi)
        cr.fill()

        angle = math.radians(225 - (self.value / 99) * 270)
        x1 = cx + math.cos(angle) * 5
        y1 = cy - math.sin(angle) * 5
        x2 = cx + math.cos(angle) * (radius - 3)
        y2 = cy - math.sin(angle) * (radius - 3)

        if self.selected:
            cr.set_source_rgba(*COLORS['accent'], 0.7)
            cr.set_line_width(5)
            cr.set_line_cap(cairo.LineCap.ROUND)
            cr.move_to(x1, y1)
            cr.line_to(x2, y2)
            cr.stroke()

        cr.set_source_rgb(*COLORS['knob_indicator'])
        cr.set_line_width(3)
        cr.set_line_cap(cairo.LineCap.ROUND)
        cr.move_to(x1, y1)
        cr.line_to(x2, y2)
        cr.stroke()
        cr.arc(x2, y2, 2, 0, 2 * math.pi)
        cr.fill()

        value_y = cy + radius + 18
        if self.selected:
            cr.set_source_rgb(*COLORS['accent'])
        else:
            cr.set_source_rgb(*COLORS['text_bright'])
        cr.select_font_face("Sans", cairo.FONT_SLANT_NORMAL, cairo.FONT_WEIGHT_BOLD)
        cr.set_font_size(14)
        text = str(self.value)
        extents = cr.text_extents(text)
        cr.move_to(cx - extents.width / 2, value_y)
        cr.show_text(text)

        label_y = cy + radius + 36
        if self.selected:
            cr.set_source_rgb(*COLORS['accent'])
        else:
            cr.set_source_rgb(*COLORS['text'])
        cr.select_font_face("Sans", cairo.FONT_SLANT_NORMAL, cairo.FONT_WEIGHT_BOLD)
        cr.set_font_size(10)
        extents = cr.text_extents(self.label)
        cr.move_to(cx - extents.width / 2, label_y)
        cr.show_text(self.label)

        return False

    def set_value(self, value, notify=True):
        new_value = max(0, min(99, int(round(value))))
        if new_value != self.value:
            self.value = new_value
            self.queue_draw()
            if notify and self.on_change:
                self.on_change(self.value)

    def set_label(self, label):
        self.label = label
        self.queue_draw()

    def set_selected(self, selected):
        self.selected = selected
        self.queue_draw()

    def on_button_press(self, widget, event):
        if event.button == 1:
            self.dragging = True
            self.drag_start_x = event.x_root
            self.drag_start_value = self.value
            
            parent = self.get_toplevel()
            if hasattr(parent, 'select_knob_widget'):
                parent.select_knob_widget(self)
        return True

    def on_button_release(self, widget, event):
        self.dragging = False
        return True

    def on_motion(self, widget, event):
        if self.dragging:
            # Calculate total displacement from start
            total_dx = event.x_root - self.drag_start_x
            
            # 300 pixels = full range (0 to 99)
            # This allows smooth continuous dragging
            new_value = self.drag_start_value + (total_dx / 3.0)
            self.set_value(new_value)
        return True

    def on_scroll(self, widget, event):
        if event.direction == Gdk.ScrollDirection.UP:
            self.set_value(self.value + 5)
        elif event.direction == Gdk.ScrollDirection.DOWN:
            self.set_value(self.value - 5)
        elif event.direction == Gdk.ScrollDirection.SMOOTH:
            _, dy = event.get_scroll_deltas()
            self.set_value(self.value - int(dy * 5))
        return True

    def on_enter(self, widget, event):
        self.hover = True
        self.queue_draw()
        return True

    def on_leave(self, widget, event):
        self.hover = False
        self.queue_draw()
        return True


class StompSwitch(Gtk.DrawingArea):
    def __init__(self, on_toggle=None):
        super().__init__()
        self.active = False
        self.on_toggle = on_toggle
        self.pressed = False
        self.hover = False

        self.set_size_request(120, 140)
        self.add_events(
            Gdk.EventMask.BUTTON_PRESS_MASK |
            Gdk.EventMask.BUTTON_RELEASE_MASK |
            Gdk.EventMask.ENTER_NOTIFY_MASK |
            Gdk.EventMask.LEAVE_NOTIFY_MASK
        )

        self.connect('draw', self.on_draw)
        self.connect('button-press-event', self.on_button_press)
        self.connect('button-release-event', self.on_button_release)
        self.connect('enter-notify-event', lambda w, e: self.set_hover(True))
        self.connect('leave-notify-event', lambda w, e: self.set_hover(False))

    def set_hover(self, hover):
        self.hover = hover
        self.queue_draw()
        return True

    def on_draw(self, widget, cr):
        width = self.get_allocated_width()
        height = self.get_allocated_height()
        cx = width / 2
        cy = height / 2 + 10

        cr.set_source_rgb(*COLORS['enclosure'])
        cr.rectangle(0, 0, width, height)
        cr.fill()

        led_y = 18
        if self.active:
            gradient = cairo.RadialGradient(cx, led_y, 0, cx, led_y, 22)
            gradient.add_color_stop_rgba(0, 1.0, 0.3, 0.3, 0.8)
            gradient.add_color_stop_rgba(0.5, 1.0, 0.1, 0.1, 0.4)
            gradient.add_color_stop_rgba(1, 1.0, 0.0, 0.0, 0.0)
            cr.set_source(gradient)
            cr.arc(cx, led_y, 22, 0, 2 * math.pi)
            cr.fill()

        gradient = cairo.RadialGradient(cx - 2, led_y - 2, 0, cx, led_y, 7)
        if self.active:
            gradient.add_color_stop_rgb(0, 1.0, 0.6, 0.6)
            gradient.add_color_stop_rgb(1, 1.0, 0.1, 0.1)
        else:
            gradient.add_color_stop_rgb(0, 0.35, 0.08, 0.08)
            gradient.add_color_stop_rgb(1, 0.20, 0.0, 0.0)
        cr.set_source(gradient)
        cr.arc(cx, led_y, 6, 0, 2 * math.pi)
        cr.fill()

        cr.set_source_rgb(0.06, 0.06, 0.06)
        cr.set_line_width(2)
        cr.arc(cx, led_y, 7, 0, 2 * math.pi)
        cr.stroke()

        cr.set_source_rgba(0, 0, 0, 0.4)
        cr.arc(cx + 3, cy + 3, 38, 0, 2 * math.pi)
        cr.fill()

        gradient = cairo.RadialGradient(cx - 12, cy - 12, 0, cx, cy, 42)
        gradient.add_color_stop_rgb(0, 0.20, 0.20, 0.20)
        gradient.add_color_stop_rgb(1, 0.08, 0.08, 0.08)
        cr.set_source(gradient)
        cr.arc(cx, cy, 40, 0, 2 * math.pi)
        cr.fill()

        offset = 2 if self.pressed else 0
        button_color = COLORS['stomp_down'] if self.pressed else COLORS['stomp_up']

        gradient = cairo.RadialGradient(cx - 8, cy - 8 + offset, 0, cx, cy + offset, 32)
        boost = 0.04 if self.hover else 0
        gradient.add_color_stop_rgb(0,
            min(1, button_color[0] + 0.10 + boost),
            min(1, button_color[1] + 0.10 + boost),
            min(1, button_color[2] + 0.10 + boost))
        gradient.add_color_stop_rgb(1,
            max(0, button_color[0] - 0.02),
            max(0, button_color[1] - 0.02),
            max(0, button_color[2] - 0.02))
        cr.set_source(gradient)
        cr.arc(cx, cy + offset, 32, 0, 2 * math.pi)
        cr.fill()

        cr.set_source_rgba(0.45, 0.45, 0.45, 0.3)
        cr.set_line_width(1)
        cr.arc(cx, cy + offset, 32, 0, 2 * math.pi)
        cr.stroke()

        cr.set_line_width(0.7)
        for i in range(1, 4):
            r = 8 + i * 6
            cr.set_source_rgba(0.35, 0.35, 0.35, 0.2)
            cr.arc(cx, cy + offset, r, 0, 2 * math.pi)
            cr.stroke()

        for angle_deg in range(0, 360, 45):
            angle = math.radians(angle_deg)
            x = cx + math.cos(angle) * 20
            y = cy + math.sin(angle) * 20 + offset
            cr.set_source_rgb(0.12, 0.12, 0.12)
            cr.arc(x, y, 2, 0, 2 * math.pi)
            cr.fill()

        label_y = cy + 50
        if self.active:
            cr.set_source_rgb(0.3, 1.0, 0.3)
            text = "▶ PLAYING"
        else:
            cr.set_source_rgb(*COLORS['text'])
            text = "PLAY"
        cr.select_font_face("Sans", cairo.FONT_SLANT_NORMAL, cairo.FONT_WEIGHT_BOLD)
        cr.set_font_size(10)
        extents = cr.text_extents(text)
        cr.move_to(cx - extents.width / 2, label_y)
        cr.show_text(text)

        return False

    def set_active(self, active):
        self.active = active
        self.queue_draw()

    def on_button_press(self, widget, event):
        if event.button == 1:
            self.pressed = True
            self.queue_draw()
        return True

    def on_button_release(self, widget, event):
        if event.button == 1:
            self.pressed = False
            self.active = not self.active
            self.queue_draw()
            if self.on_toggle:
                self.on_toggle(self.active)
        return True


class AudioNoisePedal(Gtk.Window):
    def __init__(self):
        Gtk.Window.__init__(self, title="GUI")
        
        self.set_default_size(480, 520)
        self.set_resizable(True)
        self.set_position(Gtk.WindowPosition.CENTER)

        # Dark background
        self.override_background_color(
            Gtk.StateFlags.NORMAL,
            Gdk.RGBA(*COLORS['enclosure'], 1.0)
        )

        self.effects, self.effect_order = parse_makefile()
        self.pot_values = {name: list(vals) for name, vals in self.effects.items()}
        self.current_effect = self.effect_order[0] if self.effect_order else 'flanger'

        self.playing = False
        self.proc = None
        self.player = None
        self.control_write = None

        self.selected_knob = 0
        self.knobs = []

        if shutil.which('aplay'):
            self.player_cmd = 'aplay'
        elif shutil.which('ffplay'):
            self.player_cmd = 'ffplay'
        else:
            self.player_cmd = None

        self.create_widgets()
        self.check_environment()
        self.update_knob_selection()

        self.connect('key-press-event', self.on_key_press)
        self.connect('destroy', self.on_destroy)

    def create_widgets(self):
        main_box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=0)
        main_box.set_margin_start(15)
        main_box.set_margin_end(15)
        main_box.set_margin_top(12)
        main_box.set_margin_bottom(12)
        self.add(main_box)

        title_label = Gtk.Label()
        title_label.set_markup('<span font="28" weight="bold" foreground="#d4af37">AUDIONOISE</span>')
        main_box.pack_start(title_label, False, False, 5)

        subtitle_label = Gtk.Label()
        subtitle_label.set_markup('<span font="9" foreground="#666666">DIGITAL EFFECTS PROCESSOR</span>')
        main_box.pack_start(subtitle_label, False, False, 2)

        sep = Gtk.Separator(orientation=Gtk.Orientation.HORIZONTAL)
        main_box.pack_start(sep, False, False, 12)

        effect_box = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=10)
        effect_box.set_halign(Gtk.Align.CENTER)

        effect_label = Gtk.Label()
        effect_label.set_markup('<span font="10" weight="bold" foreground="#aaaaaa">EFFECT</span>')
        effect_box.pack_start(effect_label, False, False, 0)

        self.effect_combo = Gtk.ComboBoxText()
        for effect in self.effect_order:
            self.effect_combo.append_text(effect.upper())
        self.effect_combo.set_active(0)
        self.effect_combo.connect('changed', self.on_effect_change)
        effect_box.pack_start(self.effect_combo, False, False, 0)

        main_box.pack_start(effect_box, False, False, 8)

        knobs_box = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=8)
        knobs_box.set_halign(Gtk.Align.CENTER)

        pot_names = POT_NAMES.get(self.current_effect, ['POT 1', 'POT 2', 'POT 3', 'POT 4'])

        for i in range(4):
            knob = Knob(
                label=pot_names[i],
                value=self.pot_values[self.current_effect][i],
                on_change=lambda v, idx=i: self.on_knob_change(idx, v)
            )
            knobs_box.pack_start(knob, False, False, 5)
            self.knobs.append(knob)

        main_box.pack_start(knobs_box, False, False, 8)

        stomp_box = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL)
        stomp_box.set_halign(Gtk.Align.CENTER)

        self.stomp = StompSwitch(on_toggle=self.on_stomp)
        stomp_box.pack_start(self.stomp, False, False, 0)

        main_box.pack_start(stomp_box, False, False, 8)

        status_box = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=8)
        status_box.set_halign(Gtk.Align.CENTER)

        self.status_indicator = Gtk.DrawingArea()
        self.status_indicator.set_size_request(12, 12)
        self.status_indicator.connect('draw', self.draw_status_indicator)
        status_box.pack_start(self.status_indicator, False, False, 0)

        self.status_label = Gtk.Label()
        self.status_label.set_markup('<span font="9" foreground="#888888">Ready</span>')
        status_box.pack_start(self.status_label, False, False, 0)

        main_box.pack_start(status_box, False, False, 10)

        shortcuts1 = Gtk.Label()
        shortcuts1.set_markup(
            '<span font="8" foreground="#777777">'
            'Tab: knob  ←→: ±5  ↑↓: ±1  Space: play  R: reset  1-9: effect'
            '</span>'
        )
        main_box.pack_start(shortcuts1, False, False, 4)

        footer = Gtk.Label()
        footer.set_markup('<span font="7" foreground="#555555">ea71138 • Real-time Control • GPL-2.0</span>')
        main_box.pack_start(footer, False, False, 6)

    def draw_status_indicator(self, widget, cr):
        w = widget.get_allocated_width()
        h = widget.get_allocated_height()
        cx, cy = w / 2, h / 2
        if self.playing:
            cr.set_source_rgb(0.2, 0.9, 0.2)
            cr.arc(cx, cy, 5, 0, 2 * math.pi)
            cr.fill()
        else:
            cr.set_source_rgb(0.4, 0.4, 0.4)
            cr.arc(cx, cy, 4, 0, 2 * math.pi)
            cr.fill()
        return False

    def select_knob_widget(self, knob_widget):
        for i, k in enumerate(self.knobs):
            if k == knob_widget:
                self.selected_knob = i
                k.set_selected(True)
            else:
                k.set_selected(False)

    def update_knob_selection(self):
        for i, knob in enumerate(self.knobs):
            knob.set_selected(i == self.selected_knob)

    def set_status(self, text, color="#888888"):
        self.status_label.set_markup(f'<span font="9" foreground="{color}">{text}</span>')
        self.status_indicator.queue_draw()

    def check_environment(self):
        if not Path('./convert').exists():
            self.set_status("Run 'make convert' first", "#ff6666")
        elif not self.player_cmd:
            self.set_status("No audio player (need aplay or ffplay)", "#ff6666")
        elif not Path('input.raw').exists():
            mp3_files = list(Path('.').glob('*.mp3'))
            if mp3_files:
                self.set_status(f"Converting {mp3_files[0].name}...", "#ffaa00")
                while Gtk.events_pending():
                    Gtk.main_iteration()
                try:
                    subprocess.run([
                        'ffmpeg', '-y', '-v', 'fatal', '-i', str(mp3_files[0]),
                        '-f', 's32le', '-ar', str(SAMPLE_RATE), '-ac', '1', 'input.raw'
                    ], check=True)
                    self.set_status("Ready", "#88cc88")
                except:
                    self.set_status("ffmpeg conversion failed", "#ff6666")
            else:
                self.set_status("No input.raw or .mp3 file found", "#ff6666")
        else:
            self.set_status("Ready", "#88cc88")

    def on_key_press(self, widget, event):
        key = event.keyval

        if key == Gdk.KEY_Tab:
            if event.state & Gdk.ModifierType.SHIFT_MASK:
                self.selected_knob = (self.selected_knob - 1) % 4
            else:
                self.selected_knob = (self.selected_knob + 1) % 4
            self.update_knob_selection()
            return True
        elif key == Gdk.KEY_Left:
            self.knobs[self.selected_knob].set_value(self.knobs[self.selected_knob].value - 5)
            return True
        elif key == Gdk.KEY_Right:
            self.knobs[self.selected_knob].set_value(self.knobs[self.selected_knob].value + 5)
            return True
        elif key == Gdk.KEY_Up:
            self.knobs[self.selected_knob].set_value(self.knobs[self.selected_knob].value + 1)
            return True
        elif key == Gdk.KEY_Down:
            self.knobs[self.selected_knob].set_value(self.knobs[self.selected_knob].value - 1)
            return True
        elif key == Gdk.KEY_space:
            self.stomp.active = not self.stomp.active
            self.stomp.queue_draw()
            self.on_stomp(self.stomp.active)
            return True
        elif key in (Gdk.KEY_r, Gdk.KEY_R):
            self.reset_knobs()
            return True
        elif Gdk.KEY_1 <= key <= Gdk.KEY_9:
            idx = key - Gdk.KEY_1
            if idx < len(self.effect_order):
                self.effect_combo.set_active(idx)
            return True
        elif key in (Gdk.KEY_q, Gdk.KEY_Q, Gdk.KEY_Escape):
            self.on_destroy(None)
            return True
        return False

    def reset_knobs(self):
        defaults = self.effects[self.current_effect]
        for i, knob in enumerate(self.knobs):
            knob.set_value(defaults[i])
            self.pot_values[self.current_effect][i] = defaults[i]
        self.set_status(f"Reset {self.current_effect.upper()}", "#88cc88")

    def on_effect_change(self, combo):
        text = combo.get_active_text()
        if text:
            self.current_effect = text.lower()
            pot_names = POT_NAMES.get(self.current_effect, ['POT 1', 'POT 2', 'POT 3', 'POT 4'])
            for i, knob in enumerate(self.knobs):
                knob.set_label(pot_names[i])
                knob.set_value(self.pot_values[self.current_effect][i], notify=False)
            if self.playing:
                self.stop_playback()
                self.start_playback()

    def on_knob_change(self, idx, value):
        self.pot_values[self.current_effect][idx] = value
        self.send_pot(idx, value)

    def on_stomp(self, active):
        if active:
            self.start_playback()
        else:
            self.stop_playback()

    def send_pot(self, pot_idx, value):
        """Send real-time pot command: pXYY\\n format as per Linus's ea71138"""
        if self.control_write and self.playing:
            cmd = f"p{pot_idx}{value:02d}\n"
            try:
                os.write(self.control_write, cmd.encode())
            except:
                pass

    def start_playback(self):
        if self.playing:
            self.stop_playback()

        if not Path('input.raw').exists():
            self.set_status("No input.raw file", "#ff6666")
            return

        if not self.player_cmd:
            self.set_status("No audio player available", "#ff6666")
            return

        # Create control pipe for real-time pot adjustment (Linus's --control= interface)
        ctrl_read, ctrl_write = os.pipe()
        self.control_write = ctrl_write

        pots = self.pot_values[self.current_effect]
        pots_float = [f"{v/100:.2f}" for v in pots]

        try:
            input_fd = os.open('input.raw', os.O_RDONLY)

            # Start convert with --control= pipe as per ea71138
            self.proc = subprocess.Popen(
                ['./convert', f'--control={ctrl_read}', self.current_effect] + pots_float,
                stdin=input_fd,
                stdout=subprocess.PIPE,
                stderr=subprocess.DEVNULL,
                pass_fds=(ctrl_read,)
            )
            os.close(input_fd)
            os.close(ctrl_read)

            # Start audio player
            if self.player_cmd == 'aplay':
                # Low latency with aplay -B 100 as Linus specified
                self.player = subprocess.Popen(
                    ['aplay', '-c1', '-r', str(SAMPLE_RATE), '-f', 's32', '-B', '100', '-q'],
                    stdin=self.proc.stdout,
                    stdout=subprocess.DEVNULL,
                    stderr=subprocess.DEVNULL
                )
                latency = "aplay ~100ms"
            else:
                # ffplay fallback with low latency flags
                self.player = subprocess.Popen(
                    ['ffplay', '-v', 'fatal', '-nodisp', '-autoexit',
                     '-f', 's32le', '-ar', str(SAMPLE_RATE), '-ch_layout', 'mono',
                     '-fflags', 'nobuffer', '-flags', 'low_delay',
                     '-i', 'pipe:0'],
                    stdin=self.proc.stdout,
                    stdout=subprocess.DEVNULL,
                    stderr=subprocess.DEVNULL
                )
                latency = "ffplay"

            self.playing = True
            self.set_status(f"▶ {self.current_effect.upper()} ({latency})", "#88cc88")

            GLib.timeout_add(100, self.check_playback)

        except Exception as e:
            self.set_status(f"Error: {e}", "#ff6666")
            if self.control_write:
                try:
                    os.close(self.control_write)
                except:
                    pass
                self.control_write = None

    def stop_playback(self):
        if self.control_write:
            try:
                os.close(self.control_write)
            except:
                pass
            self.control_write = None

        for proc in [self.player, self.proc]:
            if proc:
                try:
                    proc.terminate()
                    proc.wait(timeout=0.3)
                except:
                    try:
                        proc.kill()
                    except:
                        pass

        self.player = None
        self.proc = None
        self.playing = False
        self.stomp.set_active(False)
        self.set_status("Stopped", "#888888")

    def check_playback(self):
        if self.playing and self.proc and self.proc.poll() is not None:
            self.stop_playback()
            self.set_status("Finished", "#888888")
            return False
        return self.playing

    def on_destroy(self, widget):
        self.stop_playback()
        Gtk.main_quit()


def main():
    app = AudioNoisePedal()
    app.show_all()
    Gtk.main()


if __name__ == '__main__':
    main()
