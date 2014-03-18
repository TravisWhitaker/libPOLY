// libPOLY Copyright (C) Travis Whitaker 2014


#ifndef POLY_WAVEFORM_H
#define POLY_WAVEFORM_H

#define POLY_MAX_AMP 32768.0
#include <stdlib.h>
#include <time.h>

float poly_sine(float amplitude, float freq, float phase);
float poly_square(float amplitude, float freq, float duty, float phase);
float poly_saw(float amplitude, float freq, float phase);
float poly_triangle(float amplitude, float freq, float phase);
float poly_clip(float x, float max);
float poly_noise(float amplitude, float freq, float duty, float phase);

#endif
