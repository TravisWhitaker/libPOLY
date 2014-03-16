// libPOLY Copyright (C) Travis Whitaker 2014
#include <stdint.h>

#include <math.h>

#include <ao/ao.h>

#include <poly/state.h>
#include <poly/waveform.h>
#include <poly/debug.h>

float poly_sine(float amplitude, float freq, float phase)
{
	return amplitude * POLY_MAX_AMP * sin(2.0 * M_PI * (freq * ((float) poly_time/(poly_format->rate)) + phase));
}

float poly_square(float amplitude, float freq, float duty, float phase)
{
	if(fmod(((float)poly_time/poly_format->rate) + phase*(1.0/freq), 1.0/freq)/(1.0/freq) < duty)
	{
		return amplitude * POLY_MAX_AMP;
	}
	else
	{
		return -amplitude * POLY_MAX_AMP;
	}
}

float poly_saw(float amplitude, float freq, float phase)
{
	return amplitude * POLY_MAX_AMP * 2 * (freq * (((float)poly_time)/(poly_format->rate) + phase*(1.0/freq)) - floorf(0.5 + (((float)poly_time)/(poly_format->rate) + phase*(1.0/freq)) * freq));
}
