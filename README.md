libPOLY
=======

A Musical Instrument for Computers

libPOLY is a sound synthesis library for the C programming language designed for real-time music generation and output. libPOLY attempts to balance mathematical correctness with latency minimization, leading to ultra-low distortion levels and support for hundreds of simultaneous generators (channels).

###Features
Real-time analytical synthesis of the the following waveforms:
  - Sine Wave
  - Square Wave
  - Sawtooth Wave
  - Triangle Wave
  - Noise (coming soon)
  - Arbitrary Sampling (coming soon)

Simple API:
```C
poly_init(int bitdepth, int channels, int bitrate, int max_generators, const char *filename);
poly_init_generator(int index, poly_wavetype wavetype, float amplitute, float freq);
poly_start();
// Rocking-out hard core...
poly_stop();
poly_shutdown();
```

Modify synthesis parameters in real-time:
```C
poly_set_wavetype(int index, poly_wavetype wavetype);
poly_set_amplitude(int index, float amplitude);
poly_set_freq(int index, float freq);
poly_set_phase(int index, float phase);
```

libPOLY is under active develpment and the API changes frequently.

###Requirements:
  - high-speed `lm` implementation for your platform
  - `libao`
  - `pthreads`
  - ALSA if on Linux, ALSA emulation (e.g. via PulseAudio) will not suffice
