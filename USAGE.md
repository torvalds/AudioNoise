# Usage

## uv as a python package manager
To install uv, you can use the standalone installer by running the command `curl -LsSf https://astral.sh/uv/install.sh | sh` on macOS or Linux, or `powershell -ExecutionPolicy ByPass -c "irm https://astral.sh/uv/install.ps1 | iex"` on Windows. Alternatively, you can install it using package managers like Homebrew or pip.

## Raw audio format
- int32 little-endian (`s32le`), mono
- default sample rate: 48000 Hz

## Makefile quickstart
- `make input.raw` converts `BassForLinus.mp3` to raw audio
- `make output.raw` runs the default effect chain (`convert` + `echo`)
- `make visualize` launches the Python visualizer on input/output and magnitudes
- `make play` plays `output.raw` via `aplay`
- `make flanger|echo|fm|phaser|discont` runs a specific effect and plays it

## Python visualizer
- `PYTHONPATH=python python3 -m audionoise.visualize input.raw output.raw magnitude.raw outmagnitude.raw`
- Rust helpers are optional; if installed, the visualizer uses Rust autoscale/decimation

## TUI waveform viewer
- `cargo run -p audionoise-tui -- input.raw output.raw --rate 48000`
- keys: `q` quit, `left/right` or `h/l` pan, `z/x` zoom, `+/-` fine zoom, `g/G` jump start/end

## Rust/Python bindings
- `uv sync -U` builds and installs the extension into the active venv
- `uv run python -c "import audionoise; print(audionoise.autoscale_symmetric(-0.2, 0.5))"`
