// libPOLY Copyright (C) Travis Whitaker 2014

#include <stdlib.h>
#include <stdint.h>

#include <errno.h>

#include <ao/ao.h>

#include <poly/state.h>
#include <poly/client.h>
#include <poly/debug.h>

// Initialize the libPOLY global state. Return 0 on success.
int poly_init(int bitdepth, int channels, int bitrate, const char *filename)
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

	// Initialize libao:
	ao_initialize();

	ao_device *poly_card;
	ao_sample_format *poly_format;
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

	return 0;
}
