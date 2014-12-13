// Simple looping PCM sample demonstration
#include <stdlib.h>
#include <poly.h>

// A longer sample will be of higher quality, but will be the same pitch.
// libPoly assumes all samples are intended to be A at 440Hz. The div paramter
// lets the user shift by octaves if a different base is desired.
#define TEST_LEN 32
int main(int argc, char **argv)
{
	// 16-bit, stereo, 44.1KHz, 1 generator, no file to dump to.
	poly_init(16,2,44100,1,NULL);
	printf("Usage: wavetest [freq=440] [div=0]\n");

	// If the user is specifying a frequency, init with it.
	if (argc >= 2)
	{
		poly_init_generator(0,poly_loopsample,1.0,atoi(argv[1]));
	}
	else
	{
		poly_init_generator(0,poly_loopsample,1.0,440);
	}

	// Create a sample structure
	poly_sample *samp = (poly_sample *)malloc(sizeof(poly_sample));

	// The actual sample data - a simple floating PCM sample.
	// This wave is a grungy square wave, like this: _-_-____--------
	// You can make it anything you want here, even garbage data.
	samp->data = (float *)malloc(sizeof(float) * TEST_LEN);

	for (int i = 0; i < TEST_LEN; i++)
	{
		samp->data[i] = (i > TEST_LEN / 2) ? (1) : (-1);
	}
	samp->data[2] = 1.0;
	samp->data[4] = 1.0;

	// Don't forget to set the sample length in the struct too.
	samp->len = TEST_LEN;

	// This is a division of the playback frequency by 2 ^ div. Used for easily
	// shifting your sample down octaves.
	samp->div = 1;
	if (argc == 3)
	{
		samp->div = atoi(argv[2]);
	}
	poly_set_sample(0,samp);
	poly_start();
	printf("Playing a sample at %8.3f Hz.\n",poly_get_freq(0));
	printf("Strike enter to exit.\n");
	scanf("uh");

	return 0;
}
