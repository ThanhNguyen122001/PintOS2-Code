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

    bool isPinned;
}

void frame_init(void);
void* frame_allocate(enum palloc_flags, void *);
void frame_free(void *);
void frame_pin(void);
void frame_unpin(void);

#endif /* vm/frame.h */