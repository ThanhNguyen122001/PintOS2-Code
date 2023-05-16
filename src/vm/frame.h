#ifndef VM_FRAME_H
#define VM_FRAME_H

#include "lib/kernel/hash.h"
#include "lib/kernel/list.h"
#include "threads/palloc.h"
#include "thread.h"

typedef struct hash hashFrameTable;

struct frameEntry{
    void *frame;
    void *page;

    uint32_t *pagedir;

    struct hash_elem hashElement;
    struct list_elem listElement;
}

#endif /* vm/frame.h */