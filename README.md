## Another silly guitar-pedal-related repo

The digital [RP2354 and TAC5112-based guitar
pedal](https://github.com/torvalds/GuitarPedal) actually does work, even
if I'm not thrilled about some of my analog interface choices (ie the
pots in particular, and I'm growing to hate the clicky footswitch even
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
