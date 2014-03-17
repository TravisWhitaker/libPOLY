libPOLY
=======

A Musical Instrument for Computers

LibPOLY is a wave generation library made for easy creation of different wave forms. LibPOLY allows the user to create math based sound in their C/C++ programs, fun and serious uses included.

###Features
Easy wave generation including support for multiple waveforms:
  - Sine
  - Square
  - Saw
  - Triangle

Simple invokation:
```C
poly_init(int bitdepth, int channels, int bitrate, int max_generators, const char *filename);
poly_init_generator(int index, poly_wavetype wavetype, float amplitute, float freq);
```

Change your waveforms on the fly with simply setters:
```C
void poly_set_wavetype(int index, poly_wavetype wavetype);
void poly_set_amplitude(int index, float amplitude);
void poly_set_L_amp(int index, float L_amp);
void poly_set_R_amp(int index, float R_amp);
void poly_set_freq(int index, float freq);
void poly_set_phase(int index, float phase);
```

Simple shutdown:
```C
poly_stop();
poly_shutdown();
```

Usage information is currently in creation while the rest of LibPOLY is being written.
