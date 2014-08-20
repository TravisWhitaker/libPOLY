// libPOLY Copyright (C) Travis Whitaker 2014


#ifndef POLY_WAVEFORM_H
#define POLY_WAVEFORM_H

#define POLY_MAX_AMP 32766.0

float poly_sine_func(float amplitude, float freq, float phase, float duty);
float poly_square_func(float amplitude, float freq, float phase, float duty);
float poly_saw_func(float amplitude, float freq, float phase, float duty);
float poly_triangle_func(float amplitude, float freq, float phase, float duty);
float poly_clip(float x, float max);

#endif
