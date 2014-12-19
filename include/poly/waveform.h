// libPOLY Copyright (C) Travis Whitaker 2014


#ifndef POLY_WAVEFORM_H
#define POLY_WAVEFORM_H

#define POLY_MAX_AMP 32766.0

float poly_sample_func(poly_sample *s, int *pos, float amplitude, float freq, float phase);
float poly_loopsample_func(poly_sample *s, float amplitude, float freq, float phase);
float poly_noise_func(float amplitude, float freq, unsigned int *counter, unsigned int *noise_state, unsigned int tap, unsigned int size, int shift);
float poly_sine_func(float amplitude, float freq, float phase);
float poly_square_func(float amplitude, float freq, float duty, float phase);
float poly_saw_func(float amplitude, float freq, float phase);
float poly_triangle_func(float amplitude, float freq, float phase);
float poly_clip(float x, float max);

#endif
