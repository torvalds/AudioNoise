## Another silly guitar-pedal-related repo

The digital [RP2354 and TAC5112-based guitar
pedal](https://github.com/torvalds/GuitarPedal) actually does work, even
if I'm not thrilled about some of my analog interface choices (ie the
potentiometers in particular, and I'm growing to hate the clicky footswitch even
if I do love how it also doubles as a boot selector switch for
programming). 

But while the hardware design is archived while I ponder the mysteries
of life and physical user interfaces, I'm still looking at the digital
effects on the side.  But right now purely in a "since it's all digital,
let's simulate it and not worry about the hardware so much". 

These are -- like the analog circuits that started my journey -- toy
effects that you shouldn't take seriously.  The main design goal has
been to learn about digital audio processing basics.  Exactly like the
guitar pedal was about learning about the hardware side. 

So no fancy FFT-based vocoders or anything like that, just IIR filters
and basic delay loops.  Everything is "single sample in, single sample
out with no latency".  The sample may be stored in a delay loop to be
looked up later (for eacho effects), but it's not doing any real
processing. 

I was happy with how the TAC5112 had sub-ms latencies for feeding
through the ADC->DAC chain, and this is meant to continue exactly that
kind of thing.  Plus it's not like I've done any of this before, so it's
all very basic and simple just by virtue of me being a newbie. 

Put another way: the IIR filters aren't the fancy AI "emulate a cab"
kind of a modern pedal or guitar amp.  No, while they do emulate analog
circuits like a phaser, they do so by emulating the effects of a RC
network with just a digital all-pass filter, not by doing anything
actually *clever*. 

Also note that the python visualizer tool has been basically written by
vibe-coding.  I know more about analog filters -- and that's not saying
much -- than I do about python.  It started out as my typical "google
and do the monkey-see-monkey-do" kind of programming, but then I cut out
the middle-man -- me -- and just used Google Antigravity to do the audio
sample visualizer.


## Building and Running

### Dependencies

- GCC
- FFmpeg and FFplay (for audio format conversion and playback)
- Python 3 with NumPy and Matplotlib (for visualization only)

On macOS with Homebrew:
```
brew install ffmpeg
pip3 install numpy matplotlib
```

On Debian/Ubuntu:
```
apt install gcc ffmpeg python3-numpy python3-matplotlib
```

Or install Python dependencies via:
```bash
pip install -r requirements.txt
```

Or using [uv](https://docs.astral.sh/uv/):
```bash
uv sync
```

### Build

Compile the audio processor:
```
make convert
```

### Running Effects

The workflow converts an MP3 to raw 48kHz mono 32-bit audio, processes it through an effect, and plays the result.

Available effects: `flanger`, `echo`, `fm`, `am`, `phaser`, `discont`, `distortion`, `tube`, `growlingbass`

To run an effect (uses `BassForLinus.mp3` as input by default):
```
make flanger
make echo
make distortion
# etc.
```

This will process the audio, save the output as `<effect>.mp3`, and play it back.

To use your own input file:
```
ffmpeg -y -i yourfile.mp3 -f s32le -ar 48000 -ac 1 input.raw
./convert flanger 0.6 0.6 0.6 0.6 input.raw output.raw
ffmpeg -y -f s32le -ar 48000 -ac 1 -i output.raw output.mp3
```

### Visualization

To visualize the input/output waveforms:
```
make visualize
```

### Tests

Run the unit tests:
```
make test
```
