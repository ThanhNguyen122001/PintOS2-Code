#ifndef VM_PAGE_H
#define VM_PAGE_H

#include <stdbool.h>
#include <stdint.h>
#include "threads/thread.h"
#include "threads/vaddr.h"

enum pageType{
    ELF,  //It is a page of ELF executable file
    General, //It is a page of general file
    Swap //It is a page of swap file
};

struct vm_entry{
    enum pageType type;
    bool isBinaryFile;
    void *vaddr;
    uint32_t zeroBytes;
    uint32_t readBytes;
    struct file offset;
    struct file vFile;
}; 

struct page_table{
    int64_t virtualPgNumber; //Holds the Virtual Page Number
    bool ableToRead; //If the page is able to read
    bool ableToWrite; //If the page is able to write
    struct hash_elem hash_table_elem;
	struct file *load_file;
    int64_t totalAmountData;
    bool isInMemory;
};

bool load_file (void* kaddr, struct vm_entry *vme);
static bool install_page(void *upage, void *kpage,bool writable);

#endif /* vm/page.h */