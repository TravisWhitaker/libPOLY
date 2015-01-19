// libPOLY Copyright (C) Travis Whitaker 2014

#ifndef POLY_H
#define POLY_H

typedef enum
{
	poly_sine,
	poly_square,
	poly_saw,
	poly_triangle,
	poly_noise,
	poly_onesample,
	poly_loopsample,
} poly_wavetype;

typedef struct poly_sample
{
	float *data;
	unsigned int len;
	unsigned int div;
} poly_sample;

// Functions to manage global libPOLY state:
int poly_init(int bitdepth, int channels, int bitrate, int max_generators, const char *filename);
void poly_shutdown(void);

// Start and stop playback:
void poly_start_min(void);
int poly_start(void);
void poly_stop(void);

// Functions to get generator state:
char poly_get_init(int index);
poly_wavetype poly_get_wavetype(int index);
float poly_get_amplitude(int index);
float poly_get_L_amp(int index);
float poly_get_R_amp (int index);
float poly_get_freq(int index);
float poly_get_phase(int index);
float poly_get_duty(int index);
poly_sample *poly_get_sample(int index);
unsigned int poly_get_noise_tap(int index);
unsigned int poly_get_noise_size(int index);
unsigned int poly_get_noise_mult(int index);

// Functions to set generator state:
void poly_mute(int index);
void poly_unmute(int index);
void poly_set_wavetype(int index, poly_wavetype wavetype);
void poly_set_amplitude(int index, float amplitude);
void poly_set_L_amp(int index, float L_amp);
void poly_set_R_amp(int index, float R_amp);
void poly_bump_freq(int index, float freq);
void poly_set_freq(int index, float freq);
void poly_set_phase(int index, float phase);
void poly_set_duty(int index, float duty);
void poly_set_sample(int index, poly_sample *sample);
void poly_set_noise_tap(int index, unsigned int tap);
void poly_seed_noise(int index, unsigned int state);
void poly_set_noise_size(int index, unsigned int size);
void poly_set_noise_mult(int index, unsigned int mult);

// Initialize a generator with usable defaults:
void poly_init_generator(int index, poly_wavetype wavetype, float amplitute, float freq);

// For attachment to a sample stream besides libao:
void poly_next_frame(int16_t *frame);
#endif
