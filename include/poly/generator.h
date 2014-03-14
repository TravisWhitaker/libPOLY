// libPOLY Copyright (C) Travis Whitaker 2014

#ifndef POLY_GENERATOR_H
#define POLY_GENERATOR_H

// Functions to get generator state:
char poly_get_init(int index);
poly_wavetype poly_get_wavetype(int index);
float poly_get_amplitude(int index);
float poly_get_freq(int index);
float poly_get_phase(int index);
int poly_get_sample_bitdepth(int index);
int poly_get_sample_length(int index);
char *poly_get_sample(int index);

// Functions to set generator state:
void poly_mute(int index);
void poly_set_wavetype(int index, poly_wavetype wavetype);
void poly_set_amplitude(int index, float amplitude);
void poly_set_freq(int index, float freq);
void poly_set_phase(int index, float phase);
void poly_set_sample_bitdepth(int index, int sample_bitdepth);
void poly_set_sample_length(int index, int sample_length);
void poly_set_sample(int index, char *sample);

// Set all of a generator's state at once:
void poly_set_generator(int index, poly_wavetype wavetype, float amplitute, float freq, float phase, int sample_bitdepth, int sample_length, char *sample);

#endif
