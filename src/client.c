// libPOLY Copyright (C) Travis Whitaker 2014

#include <stdlib.h>
#include <stdint.h>

#include <pthread.h>

#include <errno.h>

#include <ao/ao.h>

#include <poly/state.h>
#include <poly/client.h>
#include <poly/debug.h>

#define POLY_INIT_ERROR 1
#define POLY_INIT_SUCCESS 0

#define POLY_FORMAT_UNSET 1337

char poly_ao_init;
ao_device *poly_card;
ao_sample_format *poly_format;

pthread_t poly_thread;
char poly_playback;
int poly_max_generators;
poly_gen *poly_generators;
uint64_t poly_time;

int poly_validate_args(int bitdepth, int channels, int bitrate, int max_generators)
{
	// Make sure arguments are valid:
	if(!((bitdepth == 8) || (bitdepth == 16) || (bitdepth == 24)))
	{
		DEBUG_MSG("bitdepth not one of 8, 16, 26");
		errno = EINVAL;
		return 0;
	}
	if(!((channels == 1) || (channels == 2)))
	{
		DEBUG_MSG("channels not one of 1, 2");
		errno = EINVAL;
		return 0;
	}
	if(!((bitrate == 44100) || (bitrate == 48000) || (bitrate == 96000)))
	{
		DEBUG_MSG("bitrate not one of 44100, 48000, 96000");
		errno = EINVAL;
		return 0;
	}
	if(max_generators <= 0)
	{
		DEBUG_MSG("max_generators <= 0");
		errno = EINVAL;
		return 0;
	}
	return 1;
}

// Set up libPOLY's global state, but do not initialize libAO.
// this is intended for use with other means of getting the samples to a soundcard, and
// a call to poly_start is not required after this. 
int poly_init_min(int bitdepth, int channels, int bitrate, int max_generators)
{
	if (!poly_validate_args(bitdepth, channels, bitrate, max_generators))
	{
		return POLY_INIT_ERROR;
	}

	poly_playback = 1;
	poly_ao_init = 1; // TODO: Remove this hack, have it check when needed only
	poly_max_generators = max_generators;
	poly_generators = calloc(poly_max_generators, sizeof(*poly_generators));
	if (!poly_generators)
	{
		DEBUG_MSG("couldn't allocate memory for generators. Maybe too many?");
		errno = ENOMEM;
		return POLY_INIT_ERROR;
	}
	poly_time = 0;

	// Populate the format struct:
	// Note 01/13/15: this was sizeof(*poly_format) but I don't think that's right.
	// If it's wrong, yell at me - moffitt
	poly_format = calloc(1, sizeof(poly_format));
	if(poly_format == NULL)
	{
		DEBUG_MSG("calloc for *poly_format failed");
		errno = ENOMEM;
		return POLY_INIT_ERROR;
	}
	poly_format->bits = bitdepth;
	poly_format->channels = channels;
	poly_format->rate = bitrate;
	poly_format->byte_format = AO_FMT_NATIVE;
	return POLY_INIT_SUCCESS;
}
// Initialize the libPOLY global state. Return 0 on success, 1 on error.
int poly_init(int bitdepth, int channels, int bitrate, int max_generators, const char *filename)
{
	if (poly_init_min(bitdepth, channels, bitrate, max_generators) == POLY_INIT_ERROR)
	{
		return POLY_INIT_ERROR;
	}
	// Initialize libao:
	ao_initialize();
	poly_ao_init = 1;

	poly_format->byte_format = AO_FMT_NATIVE;
	int driver;
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
			return POLY_INIT_ERROR;
		}
#else
		driver = ao_driver_id("alsa");
		if(driver == -1)
		{
			DEBUG_MSG("ao_driver_id() failed");
			errno = ENODEV;
			free(poly_format);
			return POLY_INIT_ERROR;
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
			return POLY_INIT_ERROR;
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
			return POLY_INIT_ERROR;
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
			return POLY_INIT_ERROR;
		}
	}

	poly_playback = 0; // Unset this, it should not be set until the thread is started.
	return POLY_INIT_SUCCESS;
}

// De-initialize the libPOLY environment.
void poly_shutdown()
{
	if(poly_playback == 1)
	{
		poly_stop();
	}
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

void poly_start_min()
{
	poly_playback = 1;
}

// Start real-time playback.
int poly_start()
{
	if(poly_playback == 1)
	{
		DEBUG_MSG("called poly_start() but playback was in progress");
		return 0;
	}
	poly_playback = 1;
	int status = pthread_create(&poly_thread, NULL, poly_gen_kernel, NULL);
	switch(status)
	{
	case 0:
		break;
	case EAGAIN:
		DEBUG_MSG("insufficient resources to create another thread(EAGAIN)");
		poly_stop();
		return 1;
	case EINVAL:
		DEBUG_MSG("invalid thread attributes(EINVAL)");
		poly_stop();
		return 1;
	case EPERM:
		DEBUG_MSG("insufficient privileges to create thread(EPERM)");
		poly_stop();
		return 1;
	default:
		DEBUG_MSG("unknown pthread_create error");
		poly_stop();
		return 1;
	}
	return 0;
}

// Stop real-time playback.
void poly_stop()
{
	if(poly_playback == 0)
	{
		DEBUG_MSG("called poly_stop() but playback wasn't in progress");
		return;
	}
	poly_playback = 0;
	pthread_join(poly_thread, NULL);
	poly_time = 0;
	return;
}
