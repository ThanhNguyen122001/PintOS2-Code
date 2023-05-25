#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "vm/page.h"
#include "vm/frame.h"
#include "filesys/file.h"
#include "process.h"
#include "threads/thread.h"

static struct lock page_lock;

bool load_file (void* kaddr, struct vm_entry *vme){
   
}

static bool install_page(void *upage, void *kpage,bool writable){

}
