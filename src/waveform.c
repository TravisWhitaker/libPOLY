// libPOLY Copyright (C) Travis Whitaker 2014
#include <stdint.h>

#include <math.h>

#include <ao/ao.h>

#include <poly/state.h>
#include <poly/waveform.h>
#include <poly/debug.h>



float poly_sine(float amplitude, float freq, float phase)
{
	// (signal amplitude) * (peak amplitude) * sin(2 * pi * (t + phase))
	return amplitude * POLY_MAX_AMP * sin(2.0 * M_PI * (freq * ((float) poly_time/(poly_format->rate)) + phase));
}

float poly_square(float amplitude, float freq, float duty, float phase)
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

float poly_saw(float amplitude, float freq, float phase)
{
	// (signal amplitude) * (peak amplitude) * 2 * ((time + phase)/period - floor(1/2 + (time + phase)/period))
	// where period = 1/freq
	return amplitude * POLY_MAX_AMP * 2 * (freq * (((float)poly_time)/(poly_format->rate) + phase*(1.0/freq)) - floorf(0.5 + (((float)poly_time)/(poly_format->rate) + phase*(1.0/freq)) * freq));
}

float poly_triangle(float amplitude, float freq, float phase)
{
	// Triangle wave implemented as absolute value of sawtooth wave:
	// (signal amplitude) * (peak amplitude) * (2 * abs(sawtooth(t, phase)) - 1)
	return amplitude * POLY_MAX_AMP * (2 * fabsf(2 * (freq * (((float)poly_time)/(poly_format->rate) + phase*(1.0/freq)) - floorf(0.5 + (((float)poly_time)/(poly_format->rate) + phase*(1.0/freq)) * freq))) - 1);
}

float poly_clip(float x, float max)
{
	if(x > 0.0)
	{
		if(fmodf(x, max) == x)
		{
			return x;
		}
		else
		{
			return max;
		}
	}
	
	{
		if(fmodf(-x, max) == -x)
		{
			return x;
		}
		else
		{
			return -max;
		}
	}
}

#define POLY_NOISE_DUTY_INFLUENCE 8.0

float poly_noise(float amplitude, float freq, float duty, float phase)
{
	float arg1 = (float)poly_rand_freq;
	float arg2 = POLY_NOISE_DUTY_INFLUENCE * duty;
	return poly_square(amplitude, freq + fmod(arg1,arg2), 0.5, phase);
}
