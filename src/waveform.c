// libPOLY Copyright (C) Travis Whitaker 2014
#include <stdint.h>

#include <math.h>

#include <ao/ao.h>

#include <poly/state.h>
#include <poly/waveform.h>
#include <poly/debug.h>

float poly_sine_func(float amplitude, float freq, float phase)
{
	// (signal amplitude) * (peak amplitude) * sin(2 * pi * (t + phase))
	return amplitude * POLY_MAX_AMP * sin(2.0 * M_PI * (freq * ((float) poly_time/(poly_format->rate)) + phase));
}

float poly_square_func(float amplitude, float freq, float duty, float phase)
{
	// Piece-wise square wave implementation:
	// if (adj_t) / (period) < (fractional duty cycle), return (signal amplitude) * (peak amplitude)
	// otherwise, return -(signal amplitude) * (peak amplitude)
	// where adj_t = (t + phase) % period, i.e. t relative to start of period,
	// and period = 1/freq
	if(fmod(((float)poly_time/poly_format->rate) + phase*(1.0/freq), 1.0/freq)/(1.0/freq) < duty)
	{
		return amplitude * POLY_MAX_AMP;
	}
	else
	{
		return -amplitude * POLY_MAX_AMP;
	}
}

float poly_saw_func(float amplitude, float freq, float phase)
{
	// (signal amplitude) * (peak amplitude) * 2 * ((time + phase)/period - floor(1/2 + (time + phase)/period))
	// where period = 1/freq
	return amplitude * POLY_MAX_AMP * 2 * (freq * (((float)poly_time)/(poly_format->rate) + phase*(1.0/freq)) - floorf(0.5 + (((float)poly_time)/(poly_format->rate) + phase*(1.0/freq)) * freq));
}

// The sample here is passed as a char pointer, but in the case of 16 and 24 bit samples
// it is in fact an int. Thus, it will have to be cast accordingly. 
float poly_loopsample_func(float amplitude, float freq, char *sample, int len, int depth)
{
	// Pull out the correct part of the sample. Tuned to C-4
	int index = (freq/POLY_SAMPLE_TUNE) * ((float)poly_time)/(poly_format->rate);
	
	// Not quite modulus for looping samples
	while (index > len)
	{
		index -= len;
	}

	// Construct the division for the sample
	float sample_div = 0x0000FF;
	switch (depth)
	{
		case 24:
			sample_div += 0xFF0000; // This fall through is intentional so please leave it
		case 16:
			sample_div += 0x00FF00;
		break;
	}	
	return amplitude * POLY_MAX_AMP * sample[index] / sample_div;
}

float poly_triangle_func(float amplitude, float freq, float phase)
{
	// Triangle wave implemented as absolute value of sawtooth wave:
	// (signal amplitude) * (peak amplitude) * (2 * abs(sawtooth(t, phase)) - 1)
	return amplitude * POLY_MAX_AMP * (2 * fabsf(2 * (freq * (((float)poly_time)/(poly_format->rate) + phase*(1.0/freq)) - floorf(0.5 + (((float)poly_time)/(poly_format->rate) + phase*(1.0/freq)) * freq))) - 1);
}

float poly_clip(float x, float max)
{
	if(x > 0.0)
	{
		return ((fmodf(x, max) == x) ? (x) : (max));
	}
	else
	{
		return ((fmod(-x, max) == -x) ? (x) : (-max));
	}
}
