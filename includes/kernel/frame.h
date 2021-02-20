#ifndef _KERNEL_FRAME_H
#define _KERNEL_FRAME_H

#include <kernel/system.h>
#include <kernel/logger.h>

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#define INDEX_FROM_BIT(a)   (a / (8 * 4))
#define OFFSET_FROM_BIT(a)  (a % (8 * 4))

extern int* frames;
extern int nframes;

void frame_alloc(page_t*, bool, bool);
void frame_free(page_t*);

#endif