// libPOLY Copyright (C) Travis Whitaker 2014

#ifndef POLY_GENERATOR_H
#define POLY_GENERATOR_H

// Functions to get generator state:
char poly_get_init(int index);
poly_wavetype poly_get_wavetype(int index);
float poly_get_amplitude(int index);
float poly_get_L_amp(int index);
float poly_get_R_amp(int index);
float poly_get_freq(int index);
float poly_get_phase(int index);
float poly_get_duty(int index);
poly_sample *poly_get_sample(int index);

// Functions to set generator state:
void poly_mute(int index);
void poly_unpute(int index);
void poly_set_wavetype(int index, poly_wavetype wavetype);
void poly_set_amplitude(int index, float amplitude);
void poly_set_L_amp(int index, float L_amp);
void poly_set_R_amp(int index, float R_amp);
void poly_bump_freq(int index, float freq);
void poly_set_freq(int index, float freq);
void poly_set_phase(int index, float phase);
void poly_set_duty(int index, float duty);
void poly_set_sample(int index, poly_sample *sample);
// Set all of a generator's state at once:
void poly_init_generator(int index, poly_wavetype wavetype, float amplitude, float freq);

#endif
