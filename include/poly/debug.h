// libPOLY Copyright (C) Travis Whitaker 2014

#ifndef POLY_DEBUG_H
#define POLY_DEBUG_H

#ifdef DEBUG_MSG_ENABLE
#define DEBUG_MSG(msg) printf("libPOLY: %s:%d |-> %s: %s\n", __FILE__, __LINE__, __func__, msg)
#else
#define DEBUG_MSG(ignore) ((void) 0)
#endif

#endif
