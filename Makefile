CC = gcc
CFLAGS = -Wall -O2 -Iaudio -Ibuild -I.
LDLIBS = -lm

PYTHON = python3
PLAY = ffplay -v fatal -nodisp -autoexit -f s32le -ar 48000 -ch_layout mono -i pipe:0

effects = boost eq flanger phaser echo pitch compressor
boost_defaults =
eq_defaults =
phaser_defaults = 0.3 0.3 0.5 0.5
flanger_defaults = 0.6 0.6 0.6 0.6
echo_defaults = 0.3 0.3 0.3 0.3
pitch_defaults = 0.8 0.8
compressor_defaults =

GEN_HEADERS = build/log2.h build/pow2.h build/quarter_sine.h build/eq-w0.h

HEADERS = $(GEN_HEADERS) audio/lfo.h audio/util.h \
          audio/biquad.h audio/effect.h audio/process.h \
          audio/boost.h audio/eq.h audio/phaser.h audio/flanger.h \
          audio/echo.h audio/pitch.h

default:
	@echo "Pick one of" $(effects)

play: output.raw
	$(PLAY) < output.raw

visualize: input.raw output.raw
	$(PYTHON) visualize.py input.raw output.raw

%.raw: %.mp3
	ffmpeg -y -v fatal -i $< -f s32le -ar 48000 -ac 1 $@

$(effects): input.raw convert
	./convert $@ $($@_defaults) input.raw output.raw
	ffmpeg -y -v fatal -f s32le -ar 48000 -ac 1 -i output.raw -f mp3 $@.mp3
	$(PLAY) < output.raw

convert.o: CFLAGS += -ffast-math -fsingle-precision-constant -Wfloat-conversion # -Wdouble-promotion
convert.o: $(HEADERS)

convert: convert.o

output.raw: input.raw convert
	./convert echo $(echo_defaults) input.raw output.raw

magnitude.raw: input.raw convert
	./convert magnitude 0.1 0.0001 0 0 input.raw magnitude.raw

outmagnitude.raw: output.raw convert
	./convert magnitude 0.1 0.0001 0 0 output.raw outmagnitude.raw

input.raw: BassForLinus.mp3
	ffmpeg -y -v fatal -i $< -f s32le -ar 48000 -ac 1 $@

SeymourDuncan: convert
	for i in ~/Wav/Seymour\ Duncan/*; do ffmpeg -y -v fatal -i "$$i" -f s32le -ar 48000 -ac 1 pipe:1 | ./convert phaser $(phaser_defaults) | $(PLAY) ; done

build/log2.h: scripts/log2.py
	mkdir -p build
	$(PYTHON) scripts/log2.py build/log2.h 8

build/pow2.h: scripts/pow2.py
	mkdir -p build
	$(PYTHON) scripts/pow2.py build/pow2.h 8

build/quarter_sine.h: scripts/quarter_sine.py
	mkdir -p build
	$(PYTHON) scripts/quarter_sine.py build/quarter_sine.h 8

build/eq-w0.h: scripts/eq-w0.py
	mkdir -p build
	$(PYTHON) scripts/eq-w0.py build/eq-w0.h 8

test: test-sincos test-lfo

tests/lfo: tests/lfo.o
tests/lfo.o: $(HEADERS)
test-lfo: tests/lfo
	tests/lfo

tests/sincos: tests/sincos.o
tests/sincos.o: $(HEADERS)
test-sincos: tests/sincos
	tests/sincos

.PHONY: default play $(effects) SeymourDuncan visualize test-lfo test-sincos
