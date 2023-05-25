#ifndef VM_PAGE_H
#define VM_PAGE_H

#include <stdbool.h>
#include <stdint.h>
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "lib/kernel/hash.h"
#include "devices/block.h"
#include "lib/stdbool.h"
#include "filesys/off_t.h"
#include "filesys/file.h"
#include <hash.h>

typedef struct hash hashPageTable;

enum STATUS{
    INSTALLED,
    SWAPPED,
    FILESYS,
    ALLZERO
}

struct vm_entry{
    void *page;
    void *frame;

    enum STATUS status;

    struct hash_elem hashElement;
    struct file *file;

    block_sector_t blockIndex;
    off_t offset;
    uint32_t readBytes;
    uint32_t zeroBytes;
    bool dirtyVaddr;
    bool isWritable;
    
}; 

bool load_file (void* kaddr, struct vm_entry *vme);
static bool install_page(void *upage, void *kpage,bool writable);

#endif /* vm/page.h */