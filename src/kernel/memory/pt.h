// PT - Page Table

#pragma once
#include <stdint.h>

struct PageDirectoryEntry;
struct PageTableEntry;
struct PageTable;
struct PageDirectory;

void PT_setAddress(PageTableEntry *pt, void *address);
void *PT_getAddress(PageTableEntry *pt);
void PT_setAddress(PageDirectoryEntry *pd, void *address);
void *PT_getAddress(PageDirectoryEntry *pd);

PageDirectory *PT_create_directory();
void PT_create_identity_mapping(PageDirectory *pd);
void PT_map_page(PageDirectory *pd, void *virtualAddress, void *physicalAddress, bool readOnly);
void PT_map_page_4mb(PageDirectory *pd, void *virtualAddress, void *physicalAddress, bool readOnly);
void PT_unmap_page(PageDirectory *pd, void *virtualAddress);
void PT_enable(PageDirectory *pd);
void PT_delete_directory(PageDirectory *pd);
