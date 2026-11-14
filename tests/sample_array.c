
#include <math.h>
#include <stdio.h>

#define SAMPLES_PER_SEC 48000.0f
#include "../audio/util.h"

static int failures;

static void check_sample(const char *name, float actual,
                         float expected)
{
    if (!isfinite(actual) || fabsf(actual - expected) > 0.0001f) {
        printf("FAIL: %s: got %.4f, expected %.4f\n",
               name, actual, expected);
        failures++;
    } else {
        printf("PASS: %s\n", name);
    }
}

int main(void)
{
    float samples[8] = { 0 };
    unsigned index = 0;

    sample_array_write(1.0f, &index, samples);
    sample_array_write(2.0f, &index, samples);
    sample_array_write(3.0f, &index, samples);

    check_sample("write index", (float)index, 3.0f);

    check_sample("read current sample",
                 sample_array_read(0.0f, &index, samples),
                 3.0f);

    check_sample("read previous sample",
                 sample_array_read(1.0f, &index, samples),
                 2.0f);

    check_sample("fractional interpolation",
                 sample_array_read(0.5f, &index, samples),
                 2.5f);

    /*
     * Write enough samples to wrap around the buffer.
     */
    for (unsigned i = 4; i <= 10; i++)
        sample_array_write((float)i, &index, samples);

    check_sample("wrapped current sample",
                 sample_array_read(0.0f, &index, samples),
                 10.0f);

    check_sample("wrapped previous sample",
                 sample_array_read(1.0f, &index, samples),
                 9.0f);

    check_sample("oldest retained sample",
                 sample_array_read(7.0f, &index, samples),
                 3.0f);

    printf("\nSample array tests: %s (%d failures)\n",
           failures ? "FAILED" : "PASSED", failures);

    return failures ? 1 : 0;
}
