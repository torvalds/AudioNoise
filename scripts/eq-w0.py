#!/usr/bin/env python3
import math
import sys
from pathlib import Path

def generate_eq_w0(output_path, samples_per_sec):
	Path(output_path).parent.mkdir(parents=True, exist_ok=True)

	with open(output_path, 'w') as f:
		freq = 31.25
		f.write(f"// Precomputed EQ table for {samples_per_sec/1000:.3f}kHz sample rate\n")
		f.write(f"static const struct sincos EQ_W0[] = {{\n")
		for i in range(10):
			phase = freq / samples_per_sec
			angle = phase * math.pi * 2
			sin = math.sin(angle)
			cos = math.cos(angle)

			f.write(f"\t{{ {sin:.8e}, {cos:.8e} }},\t// {freq}Hz\n")

			freq = freq * 2
		f.write(f"}};\n")

if __name__ == "__main__":
	generate_eq_w0(sys.argv[1], float(sys.argv[2]))
