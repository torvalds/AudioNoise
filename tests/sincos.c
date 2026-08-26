#include <math.h>
#include <stdio.h>

#define SAMPLES_PER_SEC 48000.0f

typedef unsigned int uint;
#include "../audio/util.h"

int main(int argc, char **argv)
{
	double maxesin = 0, maxecos = 0;

	for (double f = 0; f < 1.0001; f+=0.001) {
		struct sincos my = fastsincos(f);
		double s = sin(2*M_PI*f), c = cos(2*M_PI*f);
		double esin = fabs(my.sin - s);
		double ecos = fabs(my.cos - c);
		maxesin = fmax(esin,maxesin);
		maxecos = fmax(ecos,maxecos);
	}
	printf("Max sin() error %.8f (%.1f digits)\n", maxesin, -log10(maxesin));
	printf("Max cos() error %.8f (%.1f digits)\n", maxecos, -log10(maxecos));
}
