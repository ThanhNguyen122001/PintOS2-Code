#ifndef VM_SWAP_H
#define VM_SWAP_H

#include "devices/block.h"

void swap_init(void);
void swap_in(void *);
void swap_out(void *, void *);

#endif /* vm/swap.h */