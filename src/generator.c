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

poly_sample *poly_get_sample(int index)
{
	return (poly_generators + index)->sample;
}

unsigned int poly_get_noise_tap(int index)
{
	return (poly_generators + index)->noise_tap;
}

unsigned int poly_get_noise_size(int index)
{
	return (poly_generators + index)->noise_size;
}

unsigned int poly_get_noise_mult(int index)
{
	return (poly_generators + index)->noise_mult;
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
		(poly_generators + index)->phase = -((((float)poly_time/(poly_format->rate)) -
		((fmodf(((float)poly_time/(poly_format->rate)) + ((poly_generators + index)->phase)
		*(1.0/((poly_generators + index)->freq)), (1.0/((poly_generators + index)->freq)))/
		(1.0/((poly_generators + index)->freq)))*(1.0/freq)))/(1.0/freq));
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

void poly_set_sample(int index, poly_sample *sample)
{
	(poly_generators + index)->sample = sample;
	return;
}

void poly_set_noise_tap(int index, unsigned int tap)
{
	(poly_generators + index)->noise_tap = tap;
}

void poly_seed_noise(int index, unsigned int state)
{
	(poly_generators + index)->noise_state = state;
}

void poly_set_noise_size(int index, unsigned int size)
{
	(poly_generators + index)->noise_size = size;
}

void poly_set_noise_mult(int index, unsigned int mult)
{
	if (mult == 0)
	{
		mult = 1;
	}
	(poly_generators + index)->noise_mult = mult;
}

// Initialize a generator with sensible defaults.
void poly_init_generator(int index, poly_wavetype wavetype, float amplitude, float freq)
{
	poly_gen *gen = (poly_generators + index);
	gen->init = 1;
	gen->mute = 0;
	gen->wavetype = wavetype;
	gen->amplitude = amplitude;
	gen->matrix[0] = 1.0;
	gen->matrix[1] = 1.0;
	gen->freq = freq;
	gen->phase = 0;
	gen->duty = 0.50;
	gen->sample = NULL;
	gen->noise_counter = 0;
	gen->noise_state = 1;
	gen->noise_size = 31;
	gen->noise_tap = 6;
	gen->noise_mult = 2;
	return;

}

void poly_next_frame(int16_t *frame)
{
	memset(frame, 0, (sizeof(int16_t) * poly_format->channels));
	for (register int chan = 0; chan < poly_format->channels; chan++)
	{
		poly_gen *gen = poly_generators;
		for(register int i = 0; i < poly_max_generators; i++)
		{
			if(gen->init == 1 && gen->mute == 0)
			{
				float val = 0.0;
				float arg_amp = gen->amplitude * gen->matrix[chan];
				switch(gen->wavetype)
				{
				case poly_sine:
					val = poly_sine_func(
						arg_amp, 
						gen->freq, 
						gen->phase);
					break;
				case poly_square:
					val = poly_square_func(
						arg_amp, 
						gen->freq, 
						gen->duty, 
						gen->phase);
					break;
				case poly_saw:
					val = poly_saw_func(
						arg_amp, 
						gen->freq, 
						gen->phase);
					break;
				case poly_triangle:
					val = poly_triangle_func(
						arg_amp, 
						gen->freq, 
						gen->phase);
					break;
				case poly_loopsample:
					val = poly_loopsample_func(
						gen->sample, 	
						arg_amp, 
						gen->freq, 
						gen->phase);
					break;
				case poly_noise:
					val = poly_noise_func(
						arg_amp, 
						gen->freq * gen->noise_mult, 
						&(gen->noise_counter), 
						&(gen->noise_state), 
						gen->noise_tap, 
						gen->noise_size, 
						(chan == 0)); // Only enable shifting if it's #0
					break;
				default:
					DEBUG_MSG("waveform not yet implemented");
					break;
				}
				val = val / (float)poly_max_generators;
				frame[chan] += poly_clip((int16_t)val,POLY_MAX_AMP);
			}
			gen = gen + 1;
		}	
	}
	poly_time++;
}

void *poly_gen_kernel(void *ptr)
{
	((void)ptr);
	// We will modify this sample and present it as one frame
	int16_t *sample = calloc(poly_format->channels, sizeof(*sample));
	while(poly_playback == 1) 
	{
		poly_next_frame(sample); 
		// Send to sound device, block until frame requested
		uint32_t frame_size = (sizeof(int16_t) * poly_format->channels);
		ao_play(poly_card, (char *)sample, frame_size);
	}
	free(sample);
	return NULL;
}
