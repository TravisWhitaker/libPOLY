// libPOLY Copyright (C) Travis Whitaker 2014

#include <stdlib.h>
#include <stdint.h>

#include <pthread.h>

#include <errno.h>

#include <ao/ao.h>

#include <poly/state.h>
#include <poly/client.h>
#include <poly/debug.h>

// Initialize the libPOLY global state. Return 0 on success.
int poly_init(int bitdepth, int channels, int bitrate, int max_generators, const char *filename)
{
	// Make sure arguments are valid:
	if(!((bitdepth == 8) || (bitdepth == 16) || (bitdepth == 24)))
	{
		DEBUG_MSG("bitdepth not one of 8, 16, 26");
		errno = EINVAL;
		return 1;
	}
	if(!((channels == 1) || (channels == 2)))
	{
		DEBUG_MSG("channels not one of 1, 2");
		errno = EINVAL;
		return 1;
	}
	if(!((bitrate == 44100) || (bitrate == 48000) || (bitrate == 96000)))
	{
		DEBUG_MSG("bitrate not one of 44100, 48000, 96000");
		errno = EINVAL;
		return 1;
	}
	if(max_generators <= 0)
	{
		DEBUG_MSG("max_generators <= 0");
		errno = EINVAL;
		return 1;
	}

	// Initialize libao:
	ao_initialize();
	char poly_ao_init = 1; ((void)poly_ao_init);
	ao_device *poly_card;
	ao_sample_format *poly_format;

	pthread_t *poly_thread; ((void)poly_thread);
	char poly_playback = 0; ((void)poly_playback);
	int poly_max_generators = max_generators;
	poly_gen *poly_generators;
	uint64_t poly_time = 0; ((void)poly_time);

	int driver;

	// Populate the format struct:
	poly_format = calloc(1, sizeof(*poly_format));
	if(poly_format == NULL)
	{
		DEBUG_MSG("calloc for *poly_format failed");
		errno = ENOMEM;
		return 1;
	}
	poly_format->bits = bitdepth;
	poly_format->channels = channels;
	poly_format->rate = bitrate;
	poly_format->byte_format = AO_FMT_NATIVE;

	// Are we using a sound card?
	if(filename == NULL)
	{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(WIN64) || defined(_WIN64) || defined(__WIN64) || defined(__WIN32__) || defined(__WIN64__) || defined(__WINDOWS__)
		driver = ao_default_driver_id();
		if(driver == -1)
		{
			DEBUG_MSG("ao_default_driver_id() failed");
			errno = ENODEV;
			free(poly_format);
			return 1;
		}
#else
		driver = ao_driver_id("alsa");
		if(driver == -1)
		{
			DEBUG_MSG("ao_driver_id() failed");
			errno = ENODEV;
			free(poly_format);
			return 1;
		}
#endif
		poly_card = ao_open_live(driver, poly_format, NULL);
		if(poly_card == NULL)
		{
			switch(errno)
			{
			case AO_ENODRIVER:
				DEBUG_MSG("invalid driver");
				break;
			case AO_ENOTLIVE:
				DEBUG_MSG("file driver passed, live driver expected");
				break;
			case AO_EBADOPTION:
				DEBUG_MSG("invalid libao option, check local libao config");
				break;
			case AO_EOPENDEVICE:
				DEBUG_MSG("can't open device");
				break;
			default:
				DEBUG_MSG("unknown libao error");
				break;
			}
			free(poly_format);
			return 1;
		}
	}
	// We're using an output file:
	else
	{
		driver = ao_driver_id("wav");
		if(driver == -1)
		{
			DEBUG_MSG("libao error, can't open WAV driver");
			errno = ENODEV;
			free(poly_format);
			return 1;
		}
		poly_card = ao_open_file(driver, filename, 1, poly_format, NULL);
		if(poly_card == NULL)
		{
			switch(errno)
			{
			case AO_ENODRIVER:
				DEBUG_MSG("invalid driver");
				break;
			case AO_ENOTFILE:
				DEBUG_MSG("non-file driver passed");
				break;
			case AO_EBADOPTION:
				DEBUG_MSG("invalid libao option, check local libao config");
				break;
			case AO_EOPENFILE:
				DEBUG_MSG("can't open file");
				break;
			default:
				DEBUG_MSG("unknown libao error");
				break;
			}
			free(poly_format);
			return 1;
		}
	}

	poly_generators = calloc(poly_max_generators, sizeof(*poly_generators));

	return 0;
}

void poly_shutdown()
{
	if(poly_card != NULL)
	{
		ao_close(poly_card);
		poly_card = NULL;
	}
	if(poly_format != NULL)
	{
		free(poly_format);
		poly_format = NULL;
	}
	if(poly_ao_init == 1)
	{
		ao_shutdown();
	}
	if(poly_generators != NULL)
	{
		free(poly_generators);
		poly_generators = NULL;
	}
	return;
}

int poly_start()
{
	if(poly_playback == 1)
	{
		DEBUG_MSG("called poly_start() but playback was in progress");
		return 0;
	}
	if(!pthread_create(&poly_thread, NULL, poly_gen_kernel, NULL))
	{
		DEBUG_MSG("couldn't create playback thread");
		return 1;
	}
	poly_playback = 1;
	return 0;
}

void poly_stop()
{
	if(poly_playback == 0)
	{
		DEBUG_MSG("called poly_stop() but playback wasn't in progress");
		return;
	}
	poly_playback = 1;
	pthread_join(poly_thread, NULL);
	return;
}
