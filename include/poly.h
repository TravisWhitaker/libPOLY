// libPOLY Copyright (C) Travis Whitaker 2014

#ifndef POLY_H
#define POLY_H

typedef enum
{
	sine,
	square,
	triangle,
	saw,
	sample,
	loopsample,
} poly_wavetype;

int poly_init(int bitdepth, int channels, int bitrate, int max_generators, const char *filename);
void poly_shutdown();
int poly_start();
void poly_stop();

char poly_get_init(int index);
poly_wavetype poly_get_wavetype(int index);
float poly_get_amplitude(int index);
float poly_get_L_amp(int index);
float poly_get_R_amp (int index);
float poly_get_freq(int index);
float poly_get_phase(int index);
float poly_get_duty_cycle(int index);
int poly_get_sample_bitdepth(int index);
int poly_get_sample_length(int index);
char *poly_get_sample(int index);

void poly_mute(int index);
void poly_unmute(int index);
void poly_set_wavetype(int index, poly_wavetype wavetype);
void poly_set_amplitude(int index, float amplitude);
void poly_set_L_amp(int index, float L_amp);
void poly_set_R_amp(int index, float R_amp);
void poly_set_freq(int index, float freq);
void poly_set_phase(int index, float phase);
void poly_set_duty_cycle(int index, float duty_cycle);
void poly_set_sample_bitdepth(int index, int sample_bitdepth);
void poly_set_sample_length(int index, int sample_length);
void poly_set_sample(int index, char *sample);

void poly_init_generator(int index, poly_wavetype wavetype, float amplitute, float freq);

#endif
