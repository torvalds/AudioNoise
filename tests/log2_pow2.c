
#include <math.h>
#include <stdio.h>

#define SAMPLES_PER_SEC 48000.0f
#include "../audio/util.h"

#define TEST_POINTS 1000
#define LOG_TOLERANCE 0.0001
#define POW_REL_TOLERANCE 0.0001

static int failures;

static void check_log2(double input)
{
    double actual = log2f((float)input);
    double expected = log2(input);

    if (!isfinite(actual) ||
        fabs(actual - expected) > LOG_TOLERANCE) {
        printf("FAIL log2f(%f): got %.9f, expected %.9f\n",
               input, actual, expected);
        failures++;
    }
}

static void check_pow2(double input)
{
    double actual = pow2((float)input);
    double expected = exp2(input);
    double relative_error = fabs(actual - expected) / expected;

    if (!isfinite(actual) ||
        relative_error > POW_REL_TOLERANCE) {
        printf("FAIL pow2(%f): got %.9f, expected %.9f\n",
               input, actual, expected);
        failures++;
    }
}

int main(void)
{
    for (int i = 0; i <= TEST_POINTS; i++) {
        double value = 0.5 + 1.5 * i / TEST_POINTS;
        check_log2(value);

        double exponent = -8.0 + 16.0 * i / TEST_POINTS;
        check_pow2(exponent);
    }

    if (failures) {
        printf("%d logarithm/power tests failed\n", failures);
        return 1;
    }

    printf("All logarithm and power tests passed\n");
    return 0;
}
