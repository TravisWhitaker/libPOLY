// libPOLY Copyright (C) Travis Whitaker 2014


#ifndef POLY_WAVEFORM_H
#define POLY_WAVEFORM_H

#define POLY_MAX_AMP 32766.0
#define POLY_SAMPLE_TUNE 261.626 // This is a tuning to middle C (4th octave)

float poly_sine_func(float amplitude, float freq, float phase);
float poly_square_func(float amplitude, float freq, float duty, float phase);
float poly_saw_func(float amplitude, float freq, float phase);
float poly_loopsample_func(float amplitude, float freq, char *sample, int len, int depth);
float poly_triangle_func(float amplitude, float freq, float phase);
float poly_clip(float x, float max);

#endif
