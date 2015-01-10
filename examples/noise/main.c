// A test program for the psuedo-random noise
#include <stdlib.h>
#include <poly.h>

// The tap bit controls the feedback for the PRNG for noise. A value of 6
// should provide a reasonable approximation of the NES's 2A03 noise.
#define TAP_BIT 6
int main(int argc, char **argv)
{
	// 16-bit, stereo, 44.1KHz, 1 generator, no file to dump to.
	poly_init(16,2,44100,1,NULL);
	printf("Usage: wavetest [freq=440] [tap=6] [size=15] [seed=1] [mult=2]\n");

	unsigned int size = 15;
	unsigned int tap = 6;
	unsigned int seed = 5;
	unsigned int mult = 2;
	float freq = 440.0;

	if (argc > 1)
	{
		freq = (float)atoi(argv[1]);
		printf("Using freq %f\n",freq);
	}
	poly_init_generator(0,poly_noise,1.0,freq);
	if (argc > 2)
	{
		tap = atoi(argv[2]);
		printf("Using tap bit %d\n",tap);
	}
	if (argc > 3)
	{
		size = atoi(argv[3]);
		printf("Using size %d\n",size);
	}
	if (argc > 4)
	{
		seed = atoi(argv[4]);
		printf("Using seed %d\n",seed);
	}
	if (argc > 5)
	{
		mult = atoi(argv[5]);
		printf("Using mult %d\n",mult);
	}

	poly_seed_noise(0,seed);
	poly_set_noise_tap(0,tap);
	poly_set_noise_size(0,size);
	poly_set_noise_mult(0,mult);
	poly_start();
	printf("Strike enter to exit.\n");
	scanf("uh");

	return 0;
}
