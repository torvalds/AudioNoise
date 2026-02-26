import os
import subprocess
import threading
import tkinter as tk
from tkinter import ttk, messagebox, filedialog
from pathlib import Path
from typing import Optional
import numpy as np
import sounddevice as sd
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

SAMPLE_RATE = 48000
CHUNK_SIZE = 1024
EFFECTS = {
    'flanger':      {'defaults': [60, 60, 60, 60], 'pots': ['Depth', 'Rate', 'Fback', 'Mix']},
    'echo':         {'defaults': [30, 30, 30, 30], 'pots': ['Delay', 'Fback', 'Mix', 'Tone']},
    'fm':           {'defaults': [25, 25, 50, 50], 'pots': ['Depth', 'Rate', 'Carr', 'Mix']},
    'am':           {'defaults': [50, 50, 50, 50], 'pots': ['Depth', 'Rate', 'Shape', 'Mix']},
    'phaser':       {'defaults': [30, 30, 50, 50], 'pots': ['Depth', 'Rate', 'Stage', 'Fback']},
    'discont':      {'defaults': [80, 10, 20, 20], 'pots': ['Pitch', 'Rate', 'Blend', 'Mix']},
    'distortion':   {'defaults': [50, 60, 80, 0],  'pots': ['Drive', 'Tone', 'Level', 'Mix']},
    'tube':         {'defaults': [50, 20, 0, 100], 'pots': ['Volume', 'Boost', 'LF', 'HF']},
    'growlingbass': {'defaults': [40, 35, 0, 40],  'pots': ['Sub', 'Odd', 'Even', 'Tone']},
}
EFFECT_NAMES = list(EFFECTS.keys())


class AudioFileManager:
    """Manages audio file loading and conversion."""
    
    def __init__(self, sample_rate=SAMPLE_RATE):
        self.sample_rate = sample_rate
        self.audio_buffer: Optional[bytes] = None
        self.current_file = None
    
    def browse_and_load(self) -> Optional[bytes]:
        """Open file dialog and load audio file."""
        path = filedialog.askopenfilename(
            filetypes=[("Audio files", "*.raw *.mp3"), ("Raw PCM", "*.raw"), ("MP3", "*.mp3")]
        )
        if not path:
            return None
        
        return self.load_file(path)
    
    def load_file(self, path: str) -> Optional[bytes]:
        """Load audio file (raw or mp3) into memory."""
        try:
            path_obj = Path(path)
            
            if path_obj.suffix.lower() == '.raw':
                with open(path, 'rb') as f:
                    self.audio_buffer = f.read()
                self.current_file = path_obj.name
                messagebox.showinfo("Success", f"Loaded {path_obj.name} ({len(self.audio_buffer)} bytes)")
                return self.audio_buffer
            
            elif path_obj.suffix.lower() == '.mp3':

                proc = subprocess.Popen(
                    ['ffmpeg', '-v', 'fatal', '-i', path, '-f', 's32le', '-ar', str(self.sample_rate), '-ac', '1', '-'],
                    stdout=subprocess.PIPE,
                    stderr=subprocess.DEVNULL
                )
                self.audio_buffer, _ = proc.communicate()
                
                if proc.returncode != 0:
                    messagebox.showerror("Error", "Failed to convert MP3")
                    return None
                
                self.current_file = path_obj.name
                messagebox.showinfo("Success", f"Loaded {path_obj.name} ({len(self.audio_buffer)} bytes)")
                return self.audio_buffer
            
            else:
                messagebox.showerror("Error", "Unsupported file format. Use .raw or .mp3")
                return None
        
        except FileNotFoundError:
            messagebox.showerror("Error", "ffmpeg not found. Please install it.")
            return None
        except Exception as e:
            messagebox.showerror("Error", f"Failed to load file: {e}")
            return None


class AudioNoisePlayer:
    EFFECT_INDEX = {name: idx for idx, name in enumerate(EFFECT_NAMES)}
    
    def __init__(self, filename="input.raw"):
        self.filename = filename
        self.audio_buffer: Optional[bytes] = None
        self.proc = None
        self.playing = False
        self.paused = False
        self.control_write = None
        self.effect = EFFECT_NAMES[0]
        self.pot_values = {name: list(e['defaults']) for name, e in EFFECTS.items()}
        self.stream = None
        self.delay_ms = 0.0
        self.waveform = np.zeros(4096, dtype=np.float32)
        self.lock = threading.Lock()
        self.stream_thread = None
        self.samples_played = 0
        self.playback_time = 0.0

    def start(self):

        if self.paused:
            self.paused = False
            self.playing = True
            return
            
        if self.playing:
            return
        self.playing = True
        self.samples_played = 0

        ctrl_read, ctrl_write = os.pipe()
        self.control_write = ctrl_write

        pots_float = [v/100 for v in self.pot_values[self.effect]]
        cmd = ['./convert', self.effect] + [f"{v:.2f}" for v in pots_float] + [f'--control={ctrl_read}']

        if self.audio_buffer:
            stdin_pipe = subprocess.PIPE
            input_fd = None
        else:
            input_fd = os.open(self.filename, os.O_RDONLY)
            stdin_pipe = input_fd

        self.proc = subprocess.Popen(
            cmd,
            stdin=stdin_pipe,
            stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL,
            pass_fds=(ctrl_read,)
        )
        
        if input_fd is not None:
            os.close(input_fd)
        os.close(ctrl_read)

        if self.audio_buffer:
            def write_buffer():
                try:
                    if self.proc and self.proc.stdin and self.audio_buffer:
                        self.proc.stdin.write(self.audio_buffer)
                        self.proc.stdin.close()
                except:
                    pass
            threading.Thread(target=write_buffer, daemon=True).start()

        try:
            self.stream = sd.OutputStream(
                samplerate=SAMPLE_RATE,
                channels=1,
                dtype='float32',
                blocksize=CHUNK_SIZE,
                latency='low'
            )
            self.stream.start()
            self.device_latency_ms = self.stream.latency * 1000
        except Exception as e:
            self.playing = False
            if self.proc:
                self.proc.terminate()
            raise e

        self.stream_thread = threading.Thread(target=self.stream_audio, daemon=True)
        self.stream_thread.start()

    def pause(self):
        """Pause without stopping the pipeline"""
        if self.playing:
            self.playing = False
            self.paused = True

    def stop(self):
        """Completely stop and clean up"""
        self.playing = False
        self.paused = False
        
        if self.stream_thread and self.stream_thread.is_alive():
            self.stream_thread.join(timeout=0.2)
        
        if self.control_write:
            try: os.close(self.control_write)
            except: pass
            self.control_write = None
        
        if self.proc:
            try:
                self.proc.terminate()
                self.proc.wait(timeout=0.5)
            except: pass
            self.proc = None
        
        if self.stream:
            try:
                self.stream.stop()
                self.stream.close()
            except:
                pass
            self.stream = None
        
        self.delay_ms = 0.0

    def send_pot(self, idx, value):
        if self.control_write and self.playing:
            cmd = f"p{idx}{value:02d}\n"
            try: 
                os.write(self.control_write, cmd.encode())
            except: pass
    
    def send_effect(self, effect_name):
        """Change effect by restarting the convert process with the new effect.

        Note: The C code's control protocol only supports pot changes ('p' command),
        not runtime effect switching. So we stop and restart the pipeline."""
        if not self.playing:
            return
        was_playing = self.playing
        audio_buf = self.audio_buffer
        self.stop()
        self.effect = effect_name
        self.audio_buffer = audio_buf
        if was_playing:
            self.start()

    def stream_audio(self):
        while (self.playing or self.paused) and self.proc and self.proc.stdout:
            try:
                raw = self.proc.stdout.read(CHUNK_SIZE*4)
                if not raw:
                    break

                data = np.frombuffer(raw, dtype=np.int32).astype(np.float32)/2147483648.0

                if self.stream:
                    if self.playing and not self.paused:
                        self.stream.write(data)
                        with self.lock:
                            self.waveform = data.copy()
                            self.samples_played += len(data)
                            self.playback_time = self.samples_played / SAMPLE_RATE
                    elif self.paused:
                        silence = np.zeros(len(data), dtype=np.float32)
                        self.stream.write(silence)
                    
            except Exception:
                break

        if not self.paused:
            self.playing = False


class AudioNoiseGUI(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("TUI")
        self.geometry("900x600")
        
        self.protocol("WM_DELETE_WINDOW", self.on_closing)

        self.file_manager = AudioFileManager(SAMPLE_RATE)
        self.player = AudioNoisePlayer()

        loader_frame = ttk.Frame(self)
        loader_frame.pack(pady=5)
        ttk.Label(loader_frame, text="Load audio:").pack(side=tk.LEFT, padx=5)
        ttk.Button(loader_frame, text="Browse & Load", command=self.on_load_file).pack(side=tk.LEFT, padx=2)
        self.file_label = ttk.Label(loader_frame, text="No file loaded", foreground="gray")
        self.file_label.pack(side=tk.LEFT, padx=10)

        self.effect_var = tk.StringVar(value=EFFECT_NAMES[0])
        effect_menu = ttk.Combobox(self, textvariable=self.effect_var, values=EFFECT_NAMES)
        effect_menu.pack()
        effect_menu.bind("<<ComboboxSelected>>", self.change_effect)

        self.sliders = []
        slider_frame = ttk.Frame(self)
        slider_frame.pack()
        for i in range(4):
            frame = ttk.Frame(slider_frame)
            frame.pack(side=tk.LEFT, padx=10)
            label = ttk.Label(frame, text=EFFECTS[self.current_effect]['pots'][i])
            label.pack()
            scale = tk.Scale(frame, from_=0, to=99, orient=tk.VERTICAL,
                             command=lambda v, idx=i: self.update_pot(idx, v))
            scale.set(self.player.pot_values[self.current_effect][i])
            scale.pack()
            self.sliders.append(scale)

        button_frame = ttk.Frame(self)
        button_frame.pack(pady=5)
        self.play_button = ttk.Button(button_frame, text="Play", command=self.toggle_play, state='disabled')
        self.play_button.pack(side=tk.LEFT, padx=5)
        self.restart_button = ttk.Button(button_frame, text="Restart", command=self.restart_play, state='disabled')
        self.restart_button.pack(side=tk.LEFT, padx=5)
        
        self.fig = Figure(figsize=(8, 2))
        self.ax = self.fig.add_subplot()
        self.ax.set_ylim(-1, 1)
        self.line, = self.ax.plot([], [], lw=1, label="")
        self.delay_text = self.ax.text(0.01, 0.9, "", transform=self.ax.transAxes)
        self.canvas = FigureCanvasTkAgg(self.fig, master=self)
        self.canvas.get_tk_widget().pack(fill="both", expand=True)

        self.after(50, self.update_waveform)


    def on_load_file(self):
        """Handle file loading via file manager."""
        audio_data = self.file_manager.browse_and_load()
        if audio_data:
            self.player.audio_buffer = audio_data
            self.file_label.config(text=f"✓ {self.file_manager.current_file}", foreground="green")
            self.play_button.config(state='normal')
        else:
            self.file_label.config(text="No file loaded", foreground="gray")

    @property
    def current_effect(self):
        return self.effect_var.get()

    def change_effect(self, event=None):
        old_effect = self.player.effect
        self.player.effect = self.current_effect
        
        for i, s in enumerate(self.sliders):
            s.set(self.player.pot_values[self.current_effect][i])
        
        if self.player.playing and old_effect != self.current_effect:
            self.player.send_effect(self.current_effect)

    def update_pot(self, idx, value):
        self.player.pot_values[self.current_effect][idx] = int(value)
        if self.player.playing:
            self.player.send_pot(idx, int(value))

    def toggle_play(self):
        if not self.player.audio_buffer:
            messagebox.showwarning("Warning", "Please load an audio file first")
            return
        
        if self.player.playing:
            self.player.pause()
            self.play_button.config(text="Resume")
            self.restart_button.config(state='normal')
        elif self.player.paused:
            self.player.start()
            self.play_button.config(text="Stop")
            self.restart_button.config(state='disabled')
        else:
            try:
                self.player.start()
                self.play_button.config(text="Stop")
                self.restart_button.config(state='disabled')
            except Exception as e:
                messagebox.showerror("Audio Error", f"Failed to start playback:\n{e}")
    
    def restart_play(self):
        """Restart playback from the beginning"""
        if not self.player.audio_buffer:
            messagebox.showwarning("Warning", "Please load an audio file first")
            return
        
        self.player.stop()
        self.player.samples_played = 0
        self.player.playback_time = 0.0
        try:
            self.player.start()
            self.play_button.config(text="Stop")
            self.restart_button.config(state='disabled')
        except Exception as e:
            messagebox.showerror("Audio Error", f"Failed to start playback:\n{e}")

    def on_closing(self):
        """Clean up before closing the window"""
        if self.player.playing or self.player.paused:
            self.player.stop()
        self.destroy()

    def update_waveform(self):
        if not self.player.playing and not self.player.paused and self.play_button.cget("text") == "Stop":
            self.play_button.config(text="Play")
            self.restart_button.config(state='disabled')

        with self.player.lock:
            data = self.player.waveform.copy()
        
        if self.player.playing and self.player.stream:
            latency_val = self.player.stream.latency
            if isinstance(latency_val, (list, tuple)):
                device_latency_ms = float(latency_val[1]) * 1000
            else:
                device_latency_ms = float(latency_val) * 1000
            buffer_latency_ms = (CHUNK_SIZE / SAMPLE_RATE) * 1000
            total_latency_ms = device_latency_ms + buffer_latency_ms
        else:
            total_latency_ms = 0

        if len(data) and np.any(data != 0):
            self.line.set_data(np.arange(len(data))/SAMPLE_RATE, data)
            self.ax.set_xlim(0, len(data)/SAMPLE_RATE)
            effect_name = self.player.effect.upper() if self.player.playing else "STOPPED"
            self.delay_text.set_text(f"Effect: {effect_name} | Latency: {total_latency_ms:.1f} ms")
            self.canvas.draw_idle()

        self.after(50, self.update_waveform)


if __name__ == "__main__":
    app = AudioNoiseGUI()
    app.mainloop()
