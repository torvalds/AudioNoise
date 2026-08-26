#!/usr/bin/env python3
import math
import sys
from pathlib import Path

def generate_sine(output_path, step_shift):
	steps = 1 << step_shift
	Path(output_path).parent.mkdir(parents=True, exist_ok=True)

	with open(output_path, 'w') as f:
		f.write(f"#define QUARTER_SINE_STEP_SHIFT {step_shift}\n")
		f.write("const float quarter_sin[] = {")

		for i in range(steps + 1):
			prefix = "\n\t" if i % 4 == 0 else " "
			val = math.sin(i * math.pi / steps / 2)
			f.write(f"{prefix}{val:+.8f}f,")

		f.write(f" {1.0:+.8f}f\n}};\n")

if __name__ == "__main__":
	generate_sine(sys.argv[1], int(sys.argv[2]))
