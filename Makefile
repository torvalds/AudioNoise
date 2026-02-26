CC = gcc
CFLAGS = -Wall -O2
LDLIBS = -lm

PYTHON = python3
PLAY = ffplay -v fatal -nodisp -autoexit -f s32le -ar 48000 -ch_layout mono

effects = flanger echo fm phaser discont am distortion tube growlingbass
flanger_defaults = 0.6 0.6 0.6 0.6
echo_defaults = 0.3 0.3 0.3 0.3
fm_defaults = 0.25 0.25 0.5 0.5
am_defaults = 0.25 0.25 0.5 0.5
phaser_defaults = 0.3 0.3 0.5 0.5
discont_defaults = 0.8 0.1 0.2 0.2
distortion_defaults = 0.5 0.6 0.8 0.0
tube_defaults = 0.5 0.2 0.0 1.0
growlingbass_defaults = 0.4 0.35 0.0 0.4

HEADERS = am.h biquad.h discont.h distortion.h echo.h effect.h flanger.h growlingbass.h  fm.h  gensin.h lfo.h  phaser.h  util.h process.h tube.h

default:
	@echo "Pick one of" $(effects)

play: output.raw
	$(PLAY) $<

visualize: input.raw output.raw magnitude.raw outmagnitude.raw
	$(PYTHON) visualize.py input.raw output.raw magnitude.raw outmagnitude.raw

%.raw: %.mp3
	ffmpeg -y -v fatal -i $< -f s32le -ar 48000 -ac 1 $@

$(effects): input.raw convert
	./convert $@ $($@_defaults) input.raw output.raw
	ffmpeg -y -v fatal -f s32le -ar 48000 -ac 1 -i output.raw -f mp3 $@.mp3
	$(PLAY) output.raw

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
	@if [ ! -d ~/Wav/Seymour\ Duncan ]; then echo "Directory ~/Wav/Seymour Duncan not found"; exit 0; fi
	for i in ~/Wav/Seymour\ Duncan/*; do \
		[ -f "$$i" ] || continue; \
		ffmpeg -y -v fatal -i "$$i" -f s32le -ar 48000 -ac 1 pipe:1 | ./convert phaser $(phaser_defaults) | $(PLAY) -i pipe:0 ; \
	done

gensin.h: gensin
	./gensin > gensin.h

gensin: gensin.c

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
