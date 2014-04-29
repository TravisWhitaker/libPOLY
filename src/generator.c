// libPOLY Copyright (C) Travis Whitaker 2014
#include <stdint.h>
#include <string.h>

#include <math.h>

#include <ao/ao.h>

#include <poly/state.h>
#include <poly/waveform.h>
#include <poly/generator.h>
#include <poly/debug.h>

// Functions to get generator state:
char poly_get_init(int index)
{
	return (poly_generators + index)->init;
}

poly_wavetype poly_get_wavetype(int index)
{
	return (poly_generators + index)->wavetype;
}

float poly_get_amplitude(int index)
{
	return (poly_generators + index)->amplitude;
}

float poly_get_L_amp(int index)
{
	return (poly_generators + index)->matrix[0];
}

float poly_get_R_amp(int index)
{
	return (poly_generators + index)->matrix[1];
}

float poly_get_freq(int index)
{
	return (poly_generators + index)->freq;
}

float poly_get_phase(int index)
{
	return (poly_generators + index)->phase;
}

float poly_get_duty(int index)
{
	return (poly_generators + index)->duty;
}

int poly_get_sample_bitdepth(int index)
{
	return (poly_generators + index)->sample_bitdepth;
}

int poly_get_sample_length(int index)
{
	return (poly_generators + index)->sample_length;
}
char *poly_get_sample(int index)
{
	return (poly_generators + index)->sample;
}

// Functions to set generator state.
void poly_mute(int index)
{
	(poly_generators + index)->mute = 1;
	return;
}

void poly_unmute(int index)
{
	(poly_generators + index)->mute = 0;
	return;
}

void poly_set_wavetype(int index, poly_wavetype wavetype)
{
	(poly_generators + index)->wavetype = wavetype;
	return;
}

void poly_set_amplitude(int index, float amplitude)
{
	(poly_generators + index)->amplitude = amplitude;
	return;
}

void poly_set_L_amp(int index, float L_amp)
{
	if(!(L_amp < 0.0 || L_amp > 1.0))
	{
		(poly_generators + index)->matrix[0] = L_amp;
	}
	else
	{
		DEBUG_MSG("ignoring call, invalid L_amp");
	}
	return;
}

void poly_set_R_amp(int index, float R_amp)
{
	if(!(R_amp < 0.0 || R_amp > 1.0))
	{
		(poly_generators + index)->matrix[1] = R_amp;
	}
	else
	{
		DEBUG_MSG("ignoring call, invalid R_amp");
	}
	return;
}

// Change a generator's frequency without any phase correction.
// This may cause distortion.
void poly_bump_freq(int index, float freq)
{
	if(freq > 0.0)
	{
		(poly_generators + index)->freq = freq;
	}
	else
	{
		DEBUG_MSG("ignoring call, invalid freq");
	}
	return;
}

// Change a generator's frequency, ensuring signal continuity
// by computing a phase offset. It is safe to call this function
// as often as possible.
void poly_set_freq(int index, float freq)
{
	if(freq > 0.0)
	{
		(poly_generators + index)->phase = -((((float)poly_time/(poly_format->rate)) - ((fmodf(((float)poly_time/(poly_format->rate)) + ((poly_generators + index)->phase)*(1.0/((poly_generators + index)->freq)), (1.0/((poly_generators + index)->freq)))/(1.0/((poly_generators + index)->freq)))*(1.0/freq)))/(1.0/freq));
		(poly_generators + index)->freq = freq;
	}
	else
	{
		DEBUG_MSG("ignoring call, invalid freq");
	}
	return;
}

void poly_set_phase(int index, float phase)
{
	(poly_generators + index)->phase = phase;
	return;
}

void poly_set_duty(int index, float duty)
{
	if(!(duty < 0.0 || duty > 1.0))
	{
		(poly_generators + index)->duty = duty;
	}
	else
	{
		DEBUG_MSG("ignoring call, invalid duty");
	}
	return;
}

void poly_set_sample_bitdepth(int index, int sample_bitdepth)
{
	(poly_generators + index)->sample_bitdepth = sample_bitdepth;
	return;
}

void poly_set_sample_length(int index, int sample_length)
{
	(poly_generators + index)->sample_length = sample_length;
	return;
}

void poly_set_sample(int index, char *sample)
{
	(poly_generators + index)->sample = sample;
	return;
}

// Initialize a generator with sensible defaults.
void poly_init_generator(int index, poly_wavetype wavetype, float amplitude, float freq)
{
	(poly_generators + index)->init = 1;
	(poly_generators + index)->mute = 0;
	(poly_generators + index)->wavetype = wavetype;
	(poly_generators + index)->amplitude = amplitude;
	(poly_generators + index)->matrix[0] = 1.0;
	(poly_generators + index)->matrix[1] = 1.0;
	(poly_generators + index)->freq = freq;
	(poly_generators + index)->phase = 0;
	(poly_generators + index)->duty = 0.50;
	(poly_generators + index)->sample_bitdepth = 16;
	(poly_generators + index)->sample_length = 16;
	(poly_generators + index)->sample = NULL;
	return;
}

void *poly_gen_kernel(void *ptr)
{
	((void)ptr);
	int16_t *sample = calloc(poly_format->channels, sizeof(*sample));
	poly_gen *gen;
	while(poly_playback == 1)
	{
		for(register int chan = 0; chan < poly_format->channels; chan++)
		{
			for(register int i = 0; i < poly_max_generators; i++)
			{
				gen = poly_generators + i;
				if(gen->init == 1 && gen->mute == 0)
				{
					switch(gen->wavetype)
					{
					case poly_sine:
						sample[chan] += (int16_t)(poly_clip(poly_sine_func(gen->amplitude * gen->matrix[chan], gen->freq, gen->phase)/((float)poly_max_generators), POLY_MAX_AMP));
						break;
					case poly_square:
						sample[chan] += (int16_t)(poly_clip(poly_square_func(gen->amplitude * gen->matrix[chan], gen->freq, gen->duty, gen->phase)/((float)poly_max_generators), POLY_MAX_AMP));
						break;
					case poly_saw:
						sample[chan] += (int16_t)(poly_clip(poly_saw_func(gen->amplitude * gen->matrix[chan], gen->freq, gen->phase)/((float)poly_max_generators), POLY_MAX_AMP));
						break;
					case poly_triangle:
						sample[chan] += (int16_t)(poly_clip(poly_triangle_func(gen->amplitude * gen->matrix[chan], gen->freq, gen->phase)/((float)poly_max_generators), POLY_MAX_AMP));
						break;
					default:
						DEBUG_MSG("waveform not yet implemented");
						break;
					}
				}
			}
		}
		poly_time++;
		ao_play(poly_card, (char *)sample, (sizeof(int16_t) * poly_format->channels));
		memset(sample, 0, (sizeof(int16_t) * poly_format->channels));
	}
	free(sample);
	return NULL;
}
