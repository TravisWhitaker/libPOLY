// libPOLY Copyright (C) Travis Whitaker 2014

#ifndef POLY_CLIENT_H
#define POLY_CLIENT_H

#include <stdlib.h>
#include <time.h>

int poly_init(int bitdepth, int channels, int bitrate, int max_generators, const char *filename);
void poly_shutdown();
int poly_start();
void poly_stop();

#endif
