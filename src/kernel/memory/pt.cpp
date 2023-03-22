#include "pt.h"
#include "mm.h"
#include "util/memory.h"

#pragma pack(1)
struct PageDirectoryEntry {
    uint8_t flags;
    //only 4 upper bits
    uint8_t addressLow;
    uint16_t addressHigh;
};

#pragma pack(1)
struct PageTableEntry {
    uint8_t flags;
    //only 4 upper bits
    uint8_t addressLow;
    uint16_t addressHigh;
};

#pragma pack(1)
struct PageTable{
    PageTableEntry entries[1024];
};

#pragma pack(1)
struct PageDirectory{
    PageDirectoryEntry entries[1024];
};

enum PT_Flags{
    PT_FLAG_PRESENT = 0x1,
    PT_FLAG_READ_WRITE = 0x2,
    PT_FLAG_USER_SUPERVISOR = 0x4,
    PT_FLAG_WRITE_THROUGH = 0x8,
    PT_FLAG_CACHE_DISABLE = 0x10,
    PT_FLAG_ACCESSED = 0x20,
    PT_FLAG_DIRTY = 0x40,
    PT_FLAG_PAGE_ATTRIBUTE_TABLE = 0x80,
    PT_FLAG_GLOBAL = 0x100,
};

enum PD_Flags{
    PD_FLAG_PRESENT = 0x1,
    PD_FLAG_READ_WRITE = 0x2,
    PD_FLAG_USER_SUPERVISOR = 0x4,
    PD_FLAG_WRITE_THROUGH = 0x8,
    PD_FLAG_CACHE_DISABLE = 0x10,
    PD_FLAG_ACCESSED = 0x20,
    PD_FLAG_PAGE_SIZE = 0x80,
    PD_FLAG_GLOBAL = 0x100,
    PD_FLAG_PAGE_ATTRIBUTE_TABLE = 0x800,
};



void PT_setAddress(PageTableEntry *pt, void *address){
    pt->addressHigh = (((uint32_t)address) >> 16) & 0xffff;
    uint8_t low = pt->addressLow;
    pt->addressLow = (((uint32_t)address) >> 8) & 0xf0;
    pt->addressLow |= low & 0x0f;
}

void *PT_getAddress(PageTableEntry *pt){
    uint32_t address = 0;
    address |= (uint32_t)pt->addressHigh << 16;
    address |= ((uint32_t)pt->addressLow & 0xf0) << 8;
    return(void*)address;
}

void PT_setAddress(PageDirectoryEntry *pd, void *address){
    if(pd->flags & PD_FLAG_PAGE_SIZE){
        pd->addressHigh = (((uint32_t)address) >> 16) & 0xff30;
        pd->addressLow &= 0x0f;
    }else{
        pd->addressHigh = (((uint32_t)address) >> 16) & 0xffff;
        uint8_t low = pd->addressLow;
        pd->addressLow = (((uint32_t)address) >> 8) & 0xf0;
        pd->addressLow |= low & 0x0f;
    }
}

void *PT_getAddress(PageDirectoryEntry *pd){
    uint32_t address = 0;
    address |= (uint32_t)pd->addressHigh << 16;
    address |= ((uint32_t)pd->addressLow & 0xf0) << 8;
    return(void*)address;
}

PageDirectory *PT_create_directory(){
    PageDirectory *pd = (PageDirectory*)MM_map(4096, 4096);
    memset((char*)pd, sizeof(PageDirectory), 0);
    return pd;
}

void PT_create_identity_mapping(PageDirectory *pd){
    for(int i = 0; i < 1024; i++){
        pd->entries[i].flags |= (PD_FLAG_PRESENT | PD_FLAG_PAGE_SIZE | PD_FLAG_READ_WRITE);
        PT_setAddress(&pd->entries[i], (void*)(i * (1 << 22)));
    }
}

void PT_map_page(PageDirectory *pd, void *virtualAddress, void *physicalAddress, bool readOnly){
    uint32_t dindex = ((uint32_t)virtualAddress >> 22) & 0x3ff;
    uint32_t pindex = ((uint32_t)virtualAddress >> 12) & 0x3ff;

    if(pd->entries[dindex].flags & PD_FLAG_PAGE_SIZE){
        memset((char*)&pd->entries[dindex], sizeof(PageDirectoryEntry), 0);
        //todo: recreate 4mb mapping with 4kb pages
    }

    PageTable *pt = (PageTable*)PT_getAddress(&pd->entries[dindex]);
    if(pt == nullptr){
        pt = (PageTable*)MM_map(4096, 4096);
        memset((char*)pt, sizeof(PageTable), 0);
        PT_setAddress(&pd->entries[dindex], (void*)pt);
        uint8_t flags = PD_FLAG_PRESENT;
        if(!readOnly){
            flags |= PD_FLAG_READ_WRITE;
        }
        pd->entries[dindex].flags |= flags;
    }

    uint8_t flags = PT_FLAG_PRESENT;
    if(!readOnly){
        flags |= PT_FLAG_READ_WRITE;
    }
    pt->entries[pindex].flags |= flags;

    PT_setAddress(&pt->entries[pindex], physicalAddress);
}

void PT_map_page_4mb(PageDirectory *pd, void *virtualAddress, void *physicalAddress, bool readOnly){
    uint32_t dindex = ((uint32_t)virtualAddress >> 22) & 0x3ff;

    PT_setAddress(&pd->entries[dindex], (void*)physicalAddress);
    uint8_t flags = PD_FLAG_PRESENT | PD_FLAG_PAGE_SIZE;
    if(!readOnly){
        flags |= PD_FLAG_READ_WRITE;
    }
    pd->entries[dindex].flags |= flags;
}

void PT_unmap_page(PageDirectory *pd, void *virtualAddress){
    uint32_t dindex = ((uint32_t)virtualAddress >> 22) & 0x3ff;
    uint32_t pindex = ((uint32_t)virtualAddress >> 12) & 0x3ff;

    PageTable *pt = (PageTable*)PT_getAddress(&pd->entries[dindex]);
    if(pt != 0){
        if(pd->entries[dindex].flags & PD_FLAG_PAGE_SIZE){
            uint8_t flags = PD_FLAG_PRESENT;
            pd->entries[dindex].flags &= ~flags;
            PT_setAddress(&pd->entries[dindex], 0);
        }else{
            uint8_t flags = PT_FLAG_PRESENT;
            pt->entries[pindex].flags &= ~flags;
            PT_setAddress(&pt->entries[pindex], 0);
        }
    }
}

extern "C" void x86_PT_load(PageDirectory *pd);

void PT_enable(PageDirectory *pd){
    x86_PT_load(pd);
}

void PT_delete_directory(PageDirectory *pd){
    for(int i = 0; i < 1024; i++){
        PageTable *pt = (PageTable*)PT_getAddress(&pd->entries[i]);
        if(pt != 0){
            MM_unmap(pt);
        }
    }
    MM_unmap(pd);
}
