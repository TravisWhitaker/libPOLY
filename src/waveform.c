// libPOLY Copyright (C) Travis Whitaker 2014
#include <stdint.h>

#include <math.h>

#include <ao/ao.h>

#include <poly/state.h>
#include <poly/waveform.h>
#include <poly/debug.h>

float poly_sine(float amplitude, float freq, float phase)
{
	return amplitude * POLY_MAX_AMP * sin((2.0 * M_PI * freq * ((float) poly_time/(poly_format->rate))) + phase);
}
