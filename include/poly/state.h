// libPOLY Copyright (C) Travis Whitaker 2014

#ifndef POLY_STATE_H
#define POLY_STATE_H

typedef enum
{
	poly_sine,
	poly_square,
	poly_saw,
	poly_triangle,
	poly_sample,
	poly_loopsample
} poly_wavetype;

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
	int sample_bitdepth;
	int sample_length;
	char *sample;
} poly_gen;

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
