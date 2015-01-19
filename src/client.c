// libPOLY Copyright (C) Travis Whitaker 2014

#include <stdlib.h>
#include <stdint.h>

#include <errno.h>

#include <poly/state.h>
#include <poly/client.h>
#include <poly/debug.h>

#define POLY_INIT_ERROR 1
#define POLY_INIT_SUCCESS 0

#define POLY_FORMAT_UNSET 1337

char poly_ao_init;

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
	return POLY_INIT_SUCCESS;
}
// Initialize the libPOLY global state. Return 0 on success, 1 on error.
int poly_init(int bitdepth, int channels, int bitrate, int max_generators, const char *filename)
{
	if (poly_init_min(bitdepth, channels, bitrate, max_generators) == POLY_INIT_ERROR)
	{
		return POLY_INIT_ERROR;
	}
	char *f = (char *)filename;
	f++;
	poly_ao_init = 1;
	return POLY_INIT_SUCCESS;
}

// De-initialize the libPOLY environment.
void poly_shutdown()
{
	if(poly_playback == 1)
	{
		poly_stop();
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

int poly_start()
{
	poly_start_min();
	return 0;
}

// Stop real-time playback.
void poly_stop()
{
	poly_playback = 0;
	poly_time = 0;
	return;
}
