#!/usr/bin/env python3
import math
import sys
from pathlib import Path

def generate_log2(output_path, step_shift):
	steps = 1 << step_shift
	Path(output_path).parent.mkdir(parents=True, exist_ok=True)

	with open(output_path, 'w') as f:
		f.write(f"#define LOG2_STEP_SHIFT {step_shift}\n")
		f.write("const float log2_table[] = {")

		for i in range(steps + 1):
			prefix = "\n\t" if i % 4 == 0 else " "
			val = math.log(1 + i / steps) / math.log(2)
			f.write(f"{prefix}{val:+.8e}f,")

		f.write(f" {1.0:+.8e}f\n}};\n")

if __name__ == "__main__":
	generate_log2(sys.argv[1], int(sys.argv[2]))
