// libPOLY Copyright (C) Travis Whitaker 2014
#include <stdint.h>

#include <ao/ao.h>

#include <poly/state.h>
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

float poly_get_freq(int index)
{
	return (poly_generators + index)->freq;
}

float poly_get_phase(int index)
{
	return (poly_generators + index)->phase;
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
	(poly_generators + index)->init = GEN_OFF;
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

void poly_set_freq(int index, float freq)
{
	(poly_generators + index)->freq = freq;
	return;
}

void poly_set_phase(int index, float phase)
{
	(poly_generators + index)->phase = phase;
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

// Set all of a generator's state at once
void poly_set_generator(int index, poly_wavetype wavetype, float amplitude, float freq, float phase, int sample_bitdepth, int sample_length, char *sample)
{
	(poly_generators + index)->init = GEN_ON;
	(poly_generators + index)->wavetype = wavetype;
	(poly_generators + index)->amplitude = amplitude;
	(poly_generators + index)->freq = freq;
	(poly_generators + index)->phase = phase;
	(poly_generators + index)->sample_bitdepth = sample_bitdepth;
	(poly_generators + index)->sample_length = sample_length;
	(poly_generators + index)->sample = sample;
	return;
}
