# Interactive audio effects processor notebook
# SPDX-License-Identifier: GPL-2.0

import marimo

__generated_with = "0.19.2"
app = marimo.App(width="medium")

with app.setup(hide_code=True):
    import marimo as mo
    import numpy as np
    import matplotlib.pyplot as plt
    from scipy import signal
    import soundfile as sf
    import subprocess
    import tempfile
    import os
    from pathlib import Path
    import importlib
    from typing import Optional, Tuple, Any

    # We file watch C source files for changes, and compile dynamically
    import sys
    from cffi import FFI


@app.cell(hide_code=True)
def title():
    mo.md("""
    # Audio Effects Processor

    Interactive guitar pedal effects processor with live C code reloading.
    Upload an MP3 file, select an effect, adjust parameters, and visualize the results.
    """)
    return


@app.cell(hide_code=True)
def ui_controls(
    audio_uploader,
    defaults: list[float],
    effect_selector,
    labels: list[str],
):
    # Parameter sliders
    pot1 = mo.ui.slider(0.0, 1.0, 0.01, value=defaults[0], label=labels[0])
    pot2 = mo.ui.slider(0.0, 1.0, 0.01, value=defaults[1], label=labels[1])
    pot3 = mo.ui.slider(0.0, 1.0, 0.01, value=defaults[2], label=labels[2])
    pot4 = mo.ui.slider(0.0, 1.0, 0.01, value=defaults[3], label=labels[3])

    # Process button
    process_btn = mo.ui.run_button(label="\U0001f3b8 Process Audio", kind="success")

    mo.hstack(
        [
            mo.vstack(
                [
                    mo.md("### Effect Parameters"),
                    audio_uploader,
                    effect_selector,
                    process_btn,
                ]
            ),
            mo.vstack([pot1, pot2, pot3, pot4]),
        ]
    )
    return pot1, pot2, pot3, pot4, process_btn


@app.cell(hide_code=True)
def process_and_download(
    audio_uploader,
    default_mp3: Optional[Path],
    effect_selector,
    effects_lib,
    pot1,
    pot2,
    pot3,
    pot4,
    process_btn,
):
    audio_file: Optional[bytes] = None
    if audio_uploader.value:
        uploaded_file = (
            audio_uploader.value[0]
            if isinstance(audio_uploader.value, list)
            else audio_uploader.value
        )
        audio_file = uploaded_file.contents
    elif default_mp3 is not None:
        audio_file = default_mp3.read_bytes()

    mo.stop(
        not (process_btn.value and audio_file is not None),
        mo.md("Click 'Process Audio' to start. Default file: BassForLinus.mp3"),
    )

    processed_audio: Optional[Tuple[np.ndarray, np.ndarray, int]] = process_audio(
        audio_file,
        effect_selector.value,
        pot1.value,
        pot2.value,
        pot3.value,
        pot4.value,
        effects_lib,
    )
    _, output_audio, sr = processed_audio

    # Write to temporary file
    with tempfile.NamedTemporaryFile(suffix=".wav", delete=False) as tmp:
        sf.write(tmp.name, output_audio, sr)
        with open(tmp.name, "rb") as _f:
            audio_bytes = _f.read()

    download_link = mo.download(
        data=audio_bytes,
        filename="processed_audio.wav",
        label="Download WAV",
        mimetype="audio/wav",
    )
    return download_link, processed_audio


@app.cell(hide_code=True)
def _():
    mo.md("""
    ## Audio Playback
    """)
    return


@app.cell(hide_code=True)
def audio_playback(
    download_link,
    processed_audio: Optional[Tuple[np.ndarray, np.ndarray, int]],
):
    # Audio playback widgets
    mo.stop(
        processed_audio is None or processed_audio[0] is None,
        mo.md("No processed audio available yet"),
    )

    input_audio, output_audio_play, sr_play = processed_audio

    input_player = mo.audio(src=input_audio, rate=sr_play)
    output_player = mo.audio(src=output_audio_play, rate=sr_play)

    mo.vstack(
        [
            mo.hstack(
                [
                    mo.vstack([mo.md("**Input**"), input_player]),
                    mo.vstack([mo.md("**Output**"), output_player]),
                ]
            ),
            download_link,
        ]
    )
    return


@app.cell(hide_code=True)
def _():
    mo.md("""
    ---
    ## Spectrogram Comparison
    """)
    return


@app.cell(hide_code=True)
def spectrogram_plot(
    processed_audio: Optional[Tuple[np.ndarray, np.ndarray, int]],
):
    # Spectrogram visualization
    mo.stop(
        processed_audio is None or processed_audio[0] is None,
        mo.md("No spectrogram data available"),
    )

    input_spec, output_spec, sr_spec = processed_audio

    fig_spec, (ax_in, ax_out) = plt.subplots(1, 2, figsize=(14, 5))

    # Input spectrogram
    f_in, t_in, Sxx_in = signal.spectrogram(input_spec, sr_spec, nperseg=1024)
    im_in = ax_in.pcolormesh(
        t_in,
        f_in,
        10 * np.log10(Sxx_in + 1e-10),
        shading="gouraud",
        cmap="viridis",
        vmax=0,
        vmin=-80,
    )
    ax_in.set_ylabel("Frequency (Hz)")
    ax_in.set_xlabel("Time (s)")
    ax_in.set_title("Input Spectrogram", fontweight="bold")
    ax_in.set_ylim(0, 8000)
    plt.colorbar(im_in, ax=ax_in, label="Power (dB)")

    # Output spectrogram
    f_out, t_out, Sxx_out = signal.spectrogram(output_spec, sr_spec, nperseg=1024)
    im_out = ax_out.pcolormesh(
        t_out,
        f_out,
        10 * np.log10(Sxx_out + 1e-10),
        shading="gouraud",
        cmap="viridis",
        vmax=0,
        vmin=-80,
    )
    ax_out.set_ylabel("Frequency (Hz)")
    ax_out.set_xlabel("Time (s)")
    ax_out.set_title("Output Spectrogram", fontweight="bold")
    ax_out.set_ylim(0, 8000)
    plt.colorbar(im_out, ax=ax_out, label="Power (dB)")

    plt.tight_layout()
    plt.gca()
    return


@app.cell(hide_code=True)
def _():
    mo.md("""
    ---
    ## Statistics
    """)
    return


@app.cell(hide_code=True)
def statistics(processed_audio: Optional[Tuple[np.ndarray, np.ndarray, int]]):
    # Audio statistics
    mo.stop(
        processed_audio is None or processed_audio[0] is None,
        mo.md("No statistics available"),
    )

    input_stats, output_stats, sr_stats = processed_audio

    def compute_stats(audio: np.ndarray) -> dict[str, float]:
        return {
            "samples": len(audio),
            "duration": len(audio) / sr_stats,
            "peak": np.max(np.abs(audio)),
            "rms": np.sqrt(np.mean(audio**2)),
            "crest_factor": np.max(np.abs(audio))
            / (np.sqrt(np.mean(audio**2)) + 1e-10),
        }

    in_stats = compute_stats(input_stats)
    out_stats = compute_stats(output_stats)

    # Display statistics using mo.stat
    mo.hstack(
        [
            mo.vstack(
                [
                    mo.md("### Input"),
                    mo.hstack(
                        [
                            mo.stat(
                                label="Duration",
                                value=f"{in_stats['duration']:.2f}s",
                            ),
                            mo.stat(
                                label="Samples",
                                value=f"{in_stats['samples']:,}",
                            ),
                        ]
                    ),
                    mo.hstack(
                        [
                            mo.stat(
                                label="Peak",
                                value=f"{in_stats['peak']:.4f}",
                            ),
                            mo.stat(
                                label="RMS",
                                value=f"{in_stats['rms']:.4f}",
                            ),
                            mo.stat(
                                label="Crest Factor",
                                value=f"{in_stats['crest_factor']:.2f}",
                            ),
                        ]
                    ),
                ]
            ),
            mo.vstack(
                [
                    mo.md("### Output"),
                    mo.hstack(
                        [
                            mo.stat(
                                label="Duration",
                                value=f"{out_stats['duration']:.2f}s",
                            ),
                            mo.stat(
                                label="Samples",
                                value=f"{out_stats['samples']:,}",
                            ),
                        ]
                    ),
                    mo.hstack(
                        [
                            mo.stat(
                                label="Peak",
                                value=f"{out_stats['peak']:.4f}",
                                caption=f"{(out_stats['peak'] / in_stats['peak'] - 1) * 100:+.1f}%",
                            ),
                            mo.stat(
                                label="RMS",
                                value=f"{out_stats['rms']:.4f}",
                                caption=f"{(out_stats['rms'] / in_stats['rms'] - 1) * 100:+.1f}%",
                            ),
                            mo.stat(
                                label="Crest Factor",
                                value=f"{out_stats['crest_factor']:.2f}",
                                caption=f"{(out_stats['crest_factor'] / in_stats['crest_factor'] - 1) * 100:+.1f}%",
                            ),
                        ]
                    ),
                ]
            ),
        ]
    )
    return


@app.cell(hide_code=True)
def _():
    mo.md("""
    ---
    ## About

    This notebook provides an interactive interface to the AudioNoise guitar pedal
    effects processor. The effects are implemented in C for performance and include
    various classic guitar effects.

    ### Features:
    - Live C code reloading - Edit .c or .h files and the library auto-rebuilds
    - Audio processing - Effect processing via CFFI
    - Visualization - Spectrograms, and statistics
    - Interactive controls - Adjust effect parameters

    ### Effect Descriptions:
    - **Phaser**: All-pass filter based phase shifting with LFO modulation
    - **Flanger**: Short delay with feedback creating jet-plane effect
    - **Echo**: Classic delay-based echo with feedback control
    - **FM**: Frequency modulation for ring modulator effects
    - **Discont**: Pitch shifter using discontinuous delays

    ### Advanced Waveform Viewer

    For advanced waveform visualization with zoom/pan controls, use the interactive
    viewer section above or run `audionoise-visualize` from the command line.
    """)
    return


@app.cell(hide_code=True)
def watch_c_files():
    # Watch C source files for changes - per file
    convert_c_watcher = mo.watch.file(path="convert.c")

    # Watch all header files
    header_files: list[Path] = list(Path.cwd().glob("*.h"))
    header_watchers = [mo.watch.file(path=str(h)) for h in header_files]
    return convert_c_watcher, header_watchers


@app.function(hide_code=True)
def build_effects_library() -> Tuple[Optional[Tuple[Any, Any]], Optional[str]]:
    """Build the effects shared library using CFFI"""
    ffi = FFI()

    # Read C interface from separate file
    interface_path = Path.cwd() / "ffi.h"
    with open(interface_path, "r") as _f:
        c_interface = _f.read()

    ffi.cdef(c_interface)

    # Build the shared library first with make
    try:
        result = subprocess.run(
            ["make", "libeffects.so"], check=True, capture_output=True, text=True
        )
    except subprocess.CalledProcessError as e:
        return None, f"Build error: {e.stderr}"

    # Load the library
    lib_path = Path.cwd() / "libeffects.so"
    if not lib_path.exists():
        return None, "Error: libeffects.so not found"

    lib = ffi.dlopen(str(lib_path))
    return (ffi, lib), None


@app.cell(hide_code=True)
def build_cffi_library(convert_c_watcher, header_watchers):
    # CFFI builder cell - rebuilds when C files change
    # Create dependency on file watchers
    _c_trigger = convert_c_watcher
    _h_trigger = tuple(header_watchers)

    # Remove old module from cache to force reload
    if "_effects" in sys.modules:
        del sys.modules["_effects"]

    effects_result, error_msg = build_effects_library()

    mo.stop(
        effects_result is None, mo.md(f"Failed to build effects library: {error_msg}")
    )

    ffi, effects_lib = effects_result
    mo.md("Effects library built successfully")
    return (effects_lib,)


@app.cell(hide_code=True)
def _():
    mo.md(r"""
    ## UI widgets + helpers
    """)
    return


@app.cell
def file_uploader():
    # File uploader for audio with default file
    default_mp3: Optional[Path] = Path.cwd() / "BassForLinus.mp3"

    if not default_mp3.exists():
        default_mp3 = None

    audio_uploader = mo.ui.file(
        filetypes=[".mp3", ".wav", ".flac", ".ogg"],
        multiple=False,
        label="Upload Audio File",
    )
    return audio_uploader, default_mp3


@app.cell
def effect_selector():
    # Effect selection - NOTE: options dict is value: label format
    effect_selector = mo.ui.dropdown(
        options={
            "Phaser - Phase shifting with LFO": "phaser",
            "Flanger - Classic jet plane effect": "flanger",
            "Echo - Delay with feedback": "echo",
            "FM - Frequency modulation": "fm",
            "Discont - Pitch shifter": "discont",
        },
        value="Phaser - Phase shifting with LFO",
        label="Select Effect",
    )
    return (effect_selector,)


@app.cell
def effect_parameters(effect_selector):
    # Effect parameter labels based on selected effect
    param_labels: dict[str, list[str]] = {
        "phaser": [
            "LFO Time (25ms-2s)",
            "Feedback (0-0.75)",
            "Center Freq (50Hz-1kHz)",
            "Q Factor (0.25-2)",
        ],
        "flanger": [
            "LFO Freq (0-10Hz)",
            "Delay (0-4ms)",
            "Depth (0-100%)",
            "Feedback (0-100%)",
        ],
        "echo": ["Delay (0-1s)", "Unused", "LFO (0-4ms)", "Feedback (0-100%)"],
        "fm": ["Volume", "Base Freq", "Freq Range", "LFO (1-11Hz)"],
        "discont": ["Pitch Shift", "Unused", "Unused", "Unused"],
    }

    # Default values for each effect
    param_defaults: dict[str, list[float]] = {
        "phaser": [0.3, 0.3, 0.5, 0.5],
        "flanger": [0.6, 0.6, 0.6, 0.6],
        "echo": [0.3, 0.3, 0.3, 0.3],
        "fm": [0.25, 0.25, 0.5, 0.5],
        "discont": [0.8, 0.1, 0.2, 0.2],
    }

    current_effect: str = effect_selector.value
    labels: list[str] = param_labels.get(
        current_effect, ["Param 1", "Param 2", "Param 3", "Param 4"]
    )
    defaults: list[float] = param_defaults.get(current_effect, [0.5, 0.5, 0.5, 0.5])
    return defaults, labels


@app.function
def process_audio(
    audio_file: bytes,
    effect_name: str,
    p1: float,
    p2: float,
    p3: float,
    p4: float,
    lib: Any,
) -> Optional[Tuple[np.ndarray, np.ndarray, int]]:
    """Process audio using CFFI library

    Args:
        audio_file: Audio file contents as bytes
        effect_name: Name of the effect to apply
        p1, p2, p3, p4: Effect parameters (0.0 to 1.0)
        lib: CFFI library object

    Returns:
        Tuple of (input_audio, output_audio, sample_rate) or None if inputs are invalid
    """
    if audio_file is None or lib is None:
        return None

    # Read audio file
    import io

    audio_data, sample_rate = sf.read(io.BytesIO(audio_file))

    # Convert to mono if stereo
    if len(audio_data.shape) > 1:
        audio_data = audio_data.mean(axis=1)

    # Resample to 48kHz if needed
    if sample_rate != 48000:
        from scipy import signal as scipy_signal

        num_samples = int(len(audio_data) * 48000 / sample_rate)
        audio_data = scipy_signal.resample(audio_data, num_samples)
        sample_rate = 48000

    # Normalize to float32
    audio_data = audio_data.astype(np.float32)

    # Initialize effect
    init_func = getattr(lib, f"{effect_name}_init")
    step_func = getattr(lib, f"{effect_name}_step")
    init_func(p1, p2, p3, p4)

    # Process audio sample by sample
    output_data = np.zeros_like(audio_data)
    for i in range(len(audio_data)):
        output_data[i] = step_func(audio_data[i])

    return audio_data, output_data, sample_rate


if __name__ == "__main__":
    app.run()
