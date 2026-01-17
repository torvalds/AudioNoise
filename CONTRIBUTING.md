# Contributing to AudioNoise

Thank you for your interest in contributing to AudioNoise! This project is a learning-focused digital audio effects toolkit.

## Getting Started

### Prerequisites

- GCC compiler
- ffmpeg (for audio format conversion)
- aplay (Linux audio playback) or equivalent
- Python 3 with numpy and matplotlib (for visualizer)

### Building

```bash
make convert
```

### Testing an Effect

```bash
make flanger   # or echo, phaser, fm, discont
```

This will:
1. Convert `BassForLinus.mp3` to raw format
2. Process it through the selected effect
3. Play the result
4. Save an MP3 of the output

## How to Contribute

### Reporting Bugs

Open an issue with:
- Description of the bug
- Steps to reproduce
- Expected vs actual behavior
- Your environment (OS, compiler version)

### Suggesting Features

Open an issue describing:
- What problem the feature solves
- Proposed implementation approach
- Any DSP references or algorithms involved

### Submitting Code

1. **Fork the repository**
2. **Create a feature branch** from `main`:
   ```bash
   git checkout -b feature/my-new-effect
   ```
3. **Make your changes** following the code style below
4. **Test thoroughly** with the provided Makefile targets
5. **Commit with DCO sign-off** (see below):
   ```bash
   git commit -s -m "Add reverb effect using Schroeder algorithm"
   ```
6. **Push and open a Pull Request**

### Developer Certificate of Origin (DCO)

All contributions to this project **must** include a DCO sign-off. This certifies that you have the right to submit the work under the project's license (GPL-2.0).

Add a sign-off line to your commit message:
```
Signed-off-by: Your Name <your.email@example.com>
```

You can do this automatically with the `-s` flag:
```bash
git commit -s -m "Your commit message"
```

By signing off, you certify that:
- (a) The contribution was created by you and you have the right to submit it under GPL-2.0; or
- (b) The contribution is based on previous work covered under an appropriate open source license; or
- (c) The contribution was provided to you by someone who certified (a) or (b).
- (d) You understand this project and contribution are public.

See the full DCO text at: https://developercertificate.org/

## Code Style

This project uses a minimal, kernel-like C style:

- **Indentation**: Tabs
- **Line length**: Keep reasonable (no hard limit, but ~80 chars preferred)
- **Braces**: K&R style (opening brace on same line)
- **Comments**: Use `//` for inline, `/* */` for block
- **Naming**: lowercase_with_underscores for functions/variables

### Adding a New Effect

1. Create `myeffect.h` with:
   ```c
   static inline void myeffect_init(float pot1, float pot2, float pot3, float pot4)
   {
       // Initialize effect state
       fprintf(stderr, "myeffect: param1=%g\n", pot1);
   }

   static inline float myeffect_step(float in)
   {
       // Process one sample
       return in;  // Modified output
   }
   ```

2. Add to `convert.c`:
   ```c
   #include "myeffect.h"
   // ...
   EFF(myeffect),  // in effects[] array
   ```

3. Add to `Makefile`:
   ```makefile
   effects = ... myeffect
   myeffect_defaults = 0.5 0.5 0.5 0.5
   HEADERS = ... myeffect.h ...
   ```

### Design Principles

- **Zero latency**: Single sample in, single sample out
- **No FFT**: IIR filters and delay lines only
- **Hardware-friendly**: Code should be portable to embedded (RP2354)
- **Simple math**: Prefer fast approximations over library calls
- **Four pots**: All effects controlled by four float parameters (0.0-1.0)

## License

This project is licensed under GPL-2.0. By contributing, you agree that your contributions will be licensed under the same terms.

### GPL-2.0 Requirements

- Modified files must carry prominent notices stating changes and dates
- Derivative works must be licensed under GPL-2.0
- Keep copyright headers intact

## Questions?

Open an issue for any questions about contributing.
