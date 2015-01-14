// libPOLY Copyright (C) Travis Whitaker 2014

#ifndef POLY_STATE_H
#define POLY_STATE_H

typedef enum
{
	poly_sine,
	poly_square,
	poly_saw,
	poly_triangle,
	poly_noise,
	poly_onesample,
	poly_loopsample
} poly_wavetype;

typedef struct
{
	float *data;
	unsigned int len;
	unsigned int div;
} poly_sample;

typedef struct
{
	unsigned char init;
	unsigned char mute;
	poly_wavetype wavetype;
	float amplitude;
	float matrix[2];
	float freq;
	float phase;
	float duty;
	poly_sample *sample;
	unsigned int noise_counter;
	unsigned int noise_state;
	unsigned int noise_size;
	unsigned int noise_tap;
	unsigned int noise_mult;
} poly_gen;


// Generates a single 44.1khz 1-bit sample and increments poly_time
// sample should be a [n channels] long array
extern void poly_next_frame(int16_t *frame);

// Generator thread main loop:
extern void *poly_gen_kernel(void *ptr);

// libao stuff:
extern char poly_ao_init;
extern ao_device *poly_card;
extern ao_sample_format *poly_format;

// Playback bookkeeping:
extern pthread_t poly_thread;
extern char poly_playback;
extern int poly_max_generators;
extern poly_gen *poly_generators;
extern uint64_t poly_time;

#endif
