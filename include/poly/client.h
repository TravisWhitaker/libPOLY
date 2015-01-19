// libPOLY Copyright (C) Travis Whitaker 2014

#ifndef POLY_CLIENT_H
#define POLY_CLIENT_H

int poly_validate_args(int bitdepth, int channels, int bitrate, int max_generators);
int poly_init_min(int bitdepth, int channels, int bitrate, int max_generators);
int poly_init(int bitdepth, int channels, int bitrate, int max_generators, const char *filename);
void poly_shutdown(void);
void poly_start_min(void);
int poly_start(void);
void poly_stop(void);

#endif
