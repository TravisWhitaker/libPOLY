// libPOLY Copyright (C) Travis Whitaker 2014
#include <stdint.h>
#include <string.h>

#include <math.h>

#include <ao/ao.h>

#include <poly/state.h>
#include <poly/waveform.h>
#include <poly/generator.h>
#include <poly/debug.h>

// Function to get generator state:
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

float poly_get_duty_cycle(int index)
{
	return (poly_generators + index)->duty_cycle;
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
//	(poly_generators + index)->init = 0;
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
	if(!(amplitude < 0.0 || amplitude > 1.0))
	{
		(poly_generators + index)->amplitude = amplitude;
	}
	else
	{
		DEBUG_MSG("ignoring call, invalid amplitude");
	}
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

void poly_set_freq(int index, float freq)
{
	if(freq > 0.0)
	{
		(poly_generators + index)->phase = -((((float)poly_time/(poly_format->rate)) - ((fmodf(((float)poly_time/(poly_format->rate)) + ((poly_generators + index)->phase)*(1.0/((poly_generators + index)->freq)), (1.0/((poly_generators + index)->freq)))/(1.0/((poly_generators + index)->freq)))*(1.0/freq)))/(1.0/freq));
	}
	else
	{
		DEBUG_MSG("ignoring call, invalid freq");
	}
	(poly_generators + index)->freq = freq;
	return;
}

void poly_set_phase(int index, float phase)
{
	(poly_generators + index)->phase = phase;
	return;
}

void poly_set_duty_cycle(int index, float duty_cycle)
{
	if(!(duty_cycle < 0.0 || duty_cycle > 1.0))
	{
		(poly_generators + index)->duty_cycle = duty_cycle;
	}
	else
	{
		DEBUG_MSG("ignoring call, invalid duty_cycle");
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

void poly_init_sine_generator(int index, float amplitude, float L_amp, float R_amp, float freq, float phase)
{
	(poly_generators + index)->init = 1;
	(poly_generators + index)->wavetype = sine;
	(poly_generators + index)->amplitude = amplitude;
	(poly_generators + index)->matrix[0] = L_amp;
	(poly_generators + index)->matrix[1] = R_amp;
	(poly_generators + index)->freq = freq;
	(poly_generators + index)->phase = phase;
	return;
}

void poly_init_generator(int index, poly_wavetype wavetype, float amplitude, float freq)
{
	(poly_generators + index)->init = 1;
	(poly_generators + index)->mute = 0;
	(poly_generators + index)->poly_wavetype = wavetype;
	(poly_generators + index)->amplitude = amplitude;
	(poly_generators + index)->matrix[0] = amplitude;
	(poly_generators + index)->matrix[1] = amplitude;
	(poly_generators + index)->freq = freq;
	(poly_generators + index)->phase = 0;
	(poly_generators + index)->duty_cycle = 0.50;
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
					case sine:
						sample[chan] += (int16_t)(poly_sine(gen->amplitude * gen->matrix[chan], gen->freq, gen->phase)/((float) poly_max_generators));
						break;
					default:
						DEBUG_MSG("waveform not yet implemented");
						sample[chan] = 0;
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
