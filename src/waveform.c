// libPOLY Copyright (C) Travis Whitaker 2014
#include <stdint.h>

#include <math.h>

#include <ao/ao.h>

#include <poly/state.h>
#include <poly/waveform.h>
#include <poly/debug.h>

float poly_sample_func(poly_sample *s, int *pos, float amplitude, float freq, float phase)
{
	// TODO: Play a sample without looping
	return poly_loopsample_func(s, amplitude, freq, phase + *pos);
}

float poly_loopsample_func(poly_sample *s, float amplitude, float freq, float phase)
{
	// The sample array is assumed to be 440Hz (A4) so a longer length gives a
	// higher resolution to the wave, not a different pitch. It should be a 
	// power of two optimally. 
	// There is no filtering (bilinear, trilinear, etc.) so wave scaling is nearest-neighbor.
	// There will be some data loss at high frequencies or for very high resolution waves, but for a 
	// chiptune-oriented synth library this won't be a problem for a typically-sized sample
	// The Game Boy has a 32-step wave channel. Even something high like 256 won't experience sample 
	// drops at any useful frequencies, and nearest neighbor sounds much crisper than the smoothed out
	// muffled sounds of bilinear filtering. 
	unsigned int len = s->len;
	float coef = amplitude * POLY_MAX_AMP;
	int phase_off = (int)(phase * s->len);
	unsigned int idx = (unsigned int)(phase_off + (len * poly_time * (freq / (pow(2,s->div) * poly_format->rate)))) % len; 
	return (s->data[idx]) * coef;
}

float poly_noise_func(float amplitude, float freq, unsigned int *counter, unsigned int *noise_state, unsigned int tap, unsigned int size, int shift)
{
	// Freq is used to calculate the limit *counter must reach before a
	// shift is registered. *counter should point to an integer kept for
	// each generator. Phase is not used for noise.
	float coef = amplitude * POLY_MAX_AMP;
	if (shift)
	{
		if (*counter == 0)
		{
			*counter = (unsigned int)(poly_format->rate / (float)freq);
			// Shift noise_state over to the right 1, and give it the tap bit as a feedback
			int bit0 = *noise_state & 1;
			int tap_bit = (*noise_state & (2 << tap)) ? 1 : 0;
			tap_bit = tap_bit ^ bit0;
			*noise_state = (tap_bit << size) | (*noise_state >> 1);
	
			// Feedback bit should be the leftmost bit
			if (*noise_state > (unsigned int)(2 << tap))
			{
				*noise_state -= (2 << tap);
			}
	
		}
		else
		{
			// Decrement the counter
			*counter -= 1;
		}
	}
	// Use the last bit of noise_state as the output
	float ret = (*noise_state & 1) ? 1.0 : -1.0;
	return ret * coef;
}

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
	return((fmod(((float)poly_time/poly_format->rate) + phase*(1.0/freq), 1.0/freq)/(1.0/freq) < duty))?(amplitude * POLY_MAX_AMP):(-amplitude * POLY_MAX_AMP);
}

float poly_saw_func(float amplitude, float freq, float phase)
{
	// (signal amplitude) * (peak amplitude) * 2 * ((time + phase)/period - floor(1/2 + (time + phase)/period))
	// where period = 1/freq
	return amplitude * POLY_MAX_AMP * 2 * (freq * (((float)poly_time)/(poly_format->rate) + phase*(1.0/freq)) - floorf(0.5 + (((float)poly_time)/(poly_format->rate) + phase*(1.0/freq)) * freq));
}

float poly_triangle_func(float amplitude, float freq, float phase)
{
	// Triangle wave implemented as absolute value of sawtooth wave:
	// (signal amplitude) * (peak amplitude) * (2 * abs(sawtooth(t, phase)) - 1)
	return amplitude * POLY_MAX_AMP * (2 * fabs(2 * (freq * (((float)poly_time)/(poly_format->rate) + phase*(1.0/freq)) - floorf(0.5 + (((float)poly_time)/(poly_format->rate) + phase*(1.0/freq)) * freq))) - 1);
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
	else
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
