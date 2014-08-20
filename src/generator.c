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
    return poly_generators[index].init;
}

poly_wavetype poly_get_wavetype(int index)
{
    return poly_generators[index].wavetype;
}

float poly_get_amplitude(int index)
{
    return poly_generators[index].amplitude;
}

float poly_get_L_amp(int index)
{
    return poly_generators[index].matrix[0];
}

float poly_get_R_amp(int index)
{
    return poly_generators[index].matrix[1];
}

float poly_get_freq(int index)
{
    return poly_generators[index].freq;
}

float poly_get_phase(int index)
{
    return poly_generators[index].phase;
}

float poly_get_duty(int index)
{
    return poly_generators[index].duty;
}

int poly_get_sample_bitdepth(int index)
{
    return poly_generators[index].sample_bitdepth;
}

int poly_get_sample_length(int index)
{
    return poly_generators[index].sample_length;
}
char *poly_get_sample(int index)
{
    return poly_generators[index].sample;
}

// Functions to set generator state.
void poly_mute(int index)
{
    poly_generators[index].mute = 1;
}

void poly_unmute(int index)
{
    poly_generators[index].mute = 0;
}

void poly_set_wavetype(int index, poly_wavetype wavetype)
{
    poly_generators[index].wavetype = wavetype;
}

void poly_set_amplitude(int index, float amplitude)
{
    poly_generators[index].amplitude = amplitude;
}

void poly_set_L_amp(int index, float L_amp)
{
    if(!(L_amp < 0.0 || L_amp > 1.0))
    {
        poly_generators[index].matrix[0] = L_amp;
    }
    else
    {
        DEBUG_MSG("ignoring call, invalid L_amp");
    }
}

void poly_set_R_amp(int index, float R_amp)
{
    if(!(R_amp < 0.0 || R_amp > 1.0))
    {
        poly_generators[index].matrix[1] = R_amp;
    }
    else
    {
        DEBUG_MSG("ignoring call, invalid R_amp");
    }
}

// Change a generator's frequency without any phase correction.
// This may cause distortion.
void poly_bump_freq(int index, float freq)
{
    if(freq > 0.0)
    {
        poly_generators[index].freq = freq;
    }
    else
    {
        DEBUG_MSG("ignoring call, invalid freq");
    }
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
    poly_generators[index].phase = phase;
}

void poly_set_duty(int index, float duty)
{
    if(!(duty < 0.0 || duty > 1.0))
    {
        poly_generators[index].duty = duty;
    }
    else
    {
        DEBUG_MSG("ignoring call, invalid duty");
    }
}

void poly_set_sample_bitdepth(int index, int sample_bitdepth)
{
    poly_generators[index].sample_bitdepth = sample_bitdepth;
}

void poly_set_sample_length(int index, int sample_length)
{
    poly_generators[index].sample_length = sample_length;
}

void poly_set_sample(int index, char *sample)
{
    poly_generators[index].sample = sample;
}

// Initialize a generator with sensible defaults.
void poly_init_generator(int index, poly_wavetype wavetype, float amplitude, float freq)
{
    poly_generators[index] = (poly_gen) {
        .init = 1,
        .mute = 0,
        .wavetype = wavetype,
        .amplitude = amplitude,
        .matrix = {1.0, 1.0},
        .freq = freq,
        .phase = 0,
        .duty = 0.50,
        .sample_bitdepth = 16,
        .sample_length = 16,
        .sample = NULL
    };
}

void *poly_gen_kernel(void *ptr)
{
    (void)ptr;
    int16_t *sample = calloc(poly_format->channels, sizeof(*sample));
    poly_gen *gen;
    float(*poly_funcs[])(float, float, float, float) = {
        poly_sine_func, poly_square_func, poly_saw_func, poly_triangle_func
    };
    while(poly_playback == 1)
    {
        for(register int chan = 0; chan < poly_format->channels; chan++)
        {
            for(register int i = 0; i < poly_max_generators; i++)
            {
                gen = poly_generators + i;
                if(gen->init == 1 && gen->mute == 0)
                {
                    if(gen->wavetype < poly_sample)
                    {
                        sample[chan] += (int16_t)(poly_clip(poly_funcs[gen->wavetype](gen->amplitude * gen->matrix[chan], gen->freq, gen->phase, gen->duty)/((float)poly_max_generators), POLY_MAX_AMP));
                    }
                    else
                    {
                        DEBUG_MSG("waveform not yet implemented");
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
