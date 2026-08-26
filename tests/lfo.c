#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <inttypes.h>

#define SAMPLES_PER_SEC (48000.0)
#define MAX_ITER 0xFFFFFFFFUL  // 2^32 - 1

// Assume these exist in your project (provide stubs if missing)
#include "../util.h"
#include "../lfo.h"

static struct lfo_state lfo;

// Calculate greatest common divisor (32-bit)
static u32 gcd32(u32 a, u32 b) {
    while (b) {
        u32 temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// Format large numbers with commas (e.g., 4294967296 -> 4,294,967,296)
static void format_comma(u64 value, char *buffer, size_t len) {
    char temp[32] = {0};
    int count = 0;
    do {
        temp[count++] = '0' + (value % 10);
        value /= 10;
    } while (value > 0 && count < 20);
    
    if (value > 0) strcat(temp, " "); // For values > 2^32
    // Add commas every 3 digits
    char *dest = buffer;
    for (int i = count - 1; i >= 0; i--) {
        *dest++ = temp[i];
        if (i > 0 && (i % 3) == 0) {
            *dest++ = ',';
        }
    }
    *dest = '\0';
}

int main(int argc, char **argv) {
    // Default configuration
    u32 step = 1;
    int verbose = 1;
    u64 max_iterations = 0; // 0 = full 2^32 range

    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--step") == 0 && i + 1 < argc) {
            step = (u32)atoi(argv[i + 1]);
            i++;
        } else if (strcmp(argv[i], "--max") == 0 && i + 1 < argc) {
            max_iterations = strtoull(argv[i + 1], NULL, 10);
            i++;
        } else if (strcmp(argv[i], "--quiet") == 0) {
            verbose = 0;
        } else {
            fprintf(stderr, "Usage: %s [--step <n>] [--max <n>] [--quiet]\n", argv[0]);
            return 1;
        }
    }

    // Validate step size
    if (step == 0) {
        fprintf(stderr, "ERROR: Step size cannot be 0 (causes infinite loop)\n");
        return 1;
    }

    // Reset and configure LFO
    memset(&lfo, 0, sizeof(lfo));
    lfo.step = step;

    // Calculate actual test range
    u64 period = MAX_ITER / gcd32(step, MAX_ITER + 1);
    if (max_iterations && max_iterations < period) {
        period = max_iterations;
        verbose = 1; // Force verbose mode for limited tests
    }

    // Track best error
    float maxerr = 0.0f;
    u32 maxidx = 0;
    time_t start_time = time(NULL);

    // Progress monitoring variables
    u64 progress_update = period ? period / 100 : 1;
    double start_sample_rate = 0.0;

    // Main testing loop
    for (u64 i = 0; i < period; i++) {
        u32 idx = lfo.idx;
        float val = lfo_step(&lfo, lfo_sinewave);
        
        float s = idx * (2.0f / (float)MAX_ITER) * M_PI;
        float exact = sinf(s);
        float err = fabsf(val - exact);
        
        if (err > maxerr) {
            maxerr = err;
            maxidx = idx;
        }

        // Progress updates every 1% of progress
        if (verbose && (i == 0 || i >= progress_update)) {
            time_t now = time(NULL);
            double elapsed = (double)(now - start_time);
            double sample_rate = (i > 0) ? (i / elapsed) : 0.0;
            
            if (i == 0) {
                char num_buffer[32];
                format_comma(period, num_buffer, sizeof(num_buffer));
                printf("Testing LFO sine wave (step=%u, range=%s)...\n", 
                       step, num_buffer);
            }
            
            printf("\r  %3.0f%% complete | Error: %.8f | Samples/sec: %.1f",
                   100.0 * i / period, maxerr, sample_rate);
            fflush(stdout);
            
            start_sample_rate = sample_rate;
            progress_update = i + period / 100;
        }
    }

    // Final results
    time_t end_time = time(NULL);
    double total_time = (double)(end_time - start_time);
    printf("\n\nFinal Results:\n");
    printf("  • Total time: %.2f seconds (%.2f samples/sec)\n", 
           total_time, period / total_time);
    printf("  • Max error: %.8f\n", maxerr);
    printf("  • Worst case index: %u (0x%08X)\n", maxidx, maxidx);
    
    // Convert index to readable range
    double phase = (maxidx * (360.0 / (float)MAX_ITER));
    printf("  • Phase at max error: %.2f°\n", phase);
    if (total_time > 1) {
        printf("  • Note: Full 2^32 test took %g seconds\n", total_time);
        printf("    For faster testing, try: %s --max 100000000\n", argv[0]);
    }

    return 0;
}
