#include "fat.h"
#include "hal/hal.h"
#include "io/printf.h"
#include "util/memory.h"
#include "memory/mm.h"
#include "arch/x86/instructions.h"

#pragma pack(1)
struct FAT_Header_Common {
    uint16_t jmp;
    uint8_t nop;
    char oem[8];
};

//bios parameter block
#pragma pack(1)
struct FAT_Header_BPB {
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t fat_count;
    uint16_t dir_entries_count;
    uint16_t total_sectors;
    uint8_t media_descriptor_type;
    uint16_t sectors_per_fat;

    //BPB 3.31
    uint16_t sectors_per_track;
    uint16_t heads;
    uint32_t hidden_sectors;
    uint32_t large_sectors_count;
};

//FAT32 extended bios parameter block
#pragma pack(1)
struct FAT_Header_FAT32EBPB {
    uint32_t sectors_per_fat;
    uint16_t drive_description;
    uint16_t version;
    uint32_t root_dir_start_cluster;
    uint16_t information_sector_number;
    uint16_t boot_copy_sector_number;
    uint8_t reserved[12];
};

//extended bios parameter block
#pragma pack(1)
struct FAT_Header_EBPB{
    uint8_t drive_number;
    uint8_t reserved;
    uint8_t signature;
    uint8_t volume_id[4];
    char volume_label[11];
    char system_id[8];
};

#pragma pack(1)
struct FAT12_Header{
    FAT_Header_Common common;
    FAT_Header_BPB bpb;
    FAT_Header_EBPB ebpb;
};

#pragma pack(1)
struct FAT16_Header{
    FAT_Header_Common common;
    FAT_Header_BPB bpb;
    FAT_Header_EBPB ebpb;
};

#pragma pack(1)
struct FAT32_Header{
    FAT_Header_Common common;
    FAT_Header_BPB bpb;
    FAT_Header_FAT32EBPB fat32;
    FAT_Header_EBPB ebpb;
};


#pragma pack(1)
struct FAT_Entry{
    char name[8];
    char extension[3];
    uint8_t attributes;
    uint8_t user_attributes;
    uint8_t first_char;
    uint16_t create_time;
    uint16_t create_date;
    uint16_t last_access_date;
    uint16_t access_rights;
    uint16_t last_modified_time;
    uint16_t last_modified_date;
    uint16_t file_start_cluster;
    uint32_t file_size;
};

#pragma pack(1)
struct VFAT_Entry{
    uint8_t sequenceNumber;
    uint16_t name1[5];
    uint8_t attributes;
    uint8_t type;
    uint8_t checksum;
    uint16_t name2[6];
    uint16_t file_start_cluster;
    uint16_t name3[2];
};

#define FAT_ATTRIBUTE_READ_ONLY     0x01
#define FAT_ATTRIBUTE_HIDDEN        0x02
#define FAT_ATTRIBUTE_SYSTEM_FILE   0x04
#define FAT_ATTRIBUTE_VOLUME_LABLE  0x08
#define FAT_ATTRIBUTE_DIRECTORY     0x10
#define FAT_ATTRIBUTE_ARCHIV        0x20


uint8_t drive = 0;
uint8_t *alocation_table = nullptr;
uint32_t alocation_table_size = 0;
uint8_t *root_table = nullptr;
uint32_t root_table_size = 0;

char name_buffer[255];
int name_buffer_len = 0;

FAT32_Header header;
uint8_t fs_bits = 0;

uint32_t first_data_sector = 0;
uint32_t first_root_sector = 0;

uint32_t FAT_next_cluster(uint32_t cluster){
    int bitIndex = cluster * fs_bits;
    int byteIndex = bitIndex / 8;
    bitIndex = bitIndex % 8;
    uint8_t *ptr = &alocation_table[byteIndex];

    if(fs_bits == 12){
        uint16_t v = *((uint16_t*)ptr);
        v >>= bitIndex;
        v &= 0x0fff;
        if(v >= 0xFF8){
            return -1;
        }
        return v;
    }else if(fs_bits == 16){
        uint16_t v = *((uint16_t*)ptr);
        if(v >= 0xFFF8){
            return -1;
        }
        return v;
    }else if(fs_bits == 32){
        uint32_t v = *((uint32_t*)ptr);
        if(v >= 0x0FFFFFF8){
            return -1;
        }
        return v;
    }
    return -1;
}

//returns the size of a cluster chain in the allocation table (size in clusters)
uint32_t FAT_cluser_chain_size(uint32_t startCluster){
    uint32_t size = 0;
    uint32_t cluster = startCluster;
    while(true){
        uint32_t next = FAT_next_cluster(cluster);
        size++;
        if(next == -1){
            break;
        }
        cluster = next;
    }
    return size;
}

void FAT_read_clusters(uint32_t startCluster, uint8_t *buffer, uint32_t bufferSize);

bool FAT_init(uint32_t drive) {
    ::drive = drive;
    uint8_t boot_sector[512];
    disk_read_sectors(drive, 0, 1, boot_sector);
    FAT12_Header header12 = *(FAT12_Header*)boot_sector;
    FAT16_Header header16 = *(FAT16_Header*)boot_sector;
    header = *(FAT32_Header*)boot_sector;

    if(mem_cmp(header12.ebpb.system_id, "FAT12   ", 8)){
        fs_bits = 12;
        header = FAT32_Header();
        header.common = header12.common;
        header.bpb = header12.bpb;
        header.ebpb = header12.ebpb;
    }
    else if(mem_cmp(header16.ebpb.system_id, "FAT16   ", 8)){
        fs_bits = 16;
        header = FAT32_Header();
        header.common = header16.common;
        header.bpb = header16.bpb;
        header.ebpb = header16.ebpb;
    }
    else if(mem_cmp(header.ebpb.system_id, "FAT32   ", 8)){
        fs_bits = 32;
    }else{
        fs_bits = 0;
    }

    if(fs_bits == 0){
        return false;
    }

    printf("FAT: detected FAT%i\n", fs_bits);

    if(fs_bits != 32){
        first_root_sector = header.bpb.reserved_sectors + header.bpb.fat_count * header.bpb.sectors_per_fat;
        first_data_sector = first_root_sector + (header.bpb.dir_entries_count * 32 - 1) / 512 + 1;
    
        //read allocation table    
        alocation_table_size = header.bpb.sectors_per_fat * header.bpb.bytes_per_sector;
        alocation_table = (uint8_t*)MM_map(alocation_table_size);
        disk_read_sectors(drive, header.bpb.reserved_sectors, header.bpb.sectors_per_fat, alocation_table);

        //read root directory table
        root_table_size = header.bpb.dir_entries_count * 32;
        root_table = (uint8_t*)MM_map(root_table_size);
        disk_read_sectors(drive, first_root_sector, first_data_sector - first_root_sector, root_table);
    }else{
        first_data_sector = header.bpb.reserved_sectors + header.bpb.fat_count * header.fat32.sectors_per_fat;
        first_root_sector = first_data_sector + header.fat32.root_dir_start_cluster * header.bpb.sectors_per_cluster;
    
        //read allocation table    
        alocation_table_size = header.fat32.sectors_per_fat * header.bpb.bytes_per_sector;
        alocation_table = (uint8_t*)MM_map(alocation_table_size);
        disk_read_sectors(drive, header.bpb.reserved_sectors, header.fat32.sectors_per_fat, alocation_table);

        //read root directory table
        root_table_size = FAT_cluser_chain_size(header.fat32.root_dir_start_cluster);
        root_table_size *= header.bpb.sectors_per_cluster * header.bpb.bytes_per_sector;
        root_table = (uint8_t*)MM_map(root_table_size);
        FAT_read_clusters(header.fat32.root_dir_start_cluster, root_table, root_table_size);
    }

    return true;
}

void FAT_deinit(){
    MM_unmap(root_table);
    root_table= 0;
    root_table_size = 0;
    MM_unmap(alocation_table);
    alocation_table = 0;
    alocation_table_size = 0;
}

void FAT_read_clusters(uint32_t startCluster, uint8_t *buffer, uint32_t bufferSize){
    uint32_t cluster = startCluster;
    uint32_t cluster_size_bytes = header.bpb.bytes_per_sector * header.bpb.sectors_per_cluster;
    while(true){
        if(bufferSize < cluster_size_bytes){
            printf("FAT error can't read file, supplied buffer is to small\n");
            return;
        }

        disk_read_sectors(drive, first_data_sector + (cluster - 2) * header.bpb.sectors_per_cluster, header.bpb.sectors_per_cluster, buffer);
        buffer += cluster_size_bytes;
        bufferSize -= cluster_size_bytes;

        uint32_t next= FAT_next_cluster(cluster);
        if(next == -1){
            break;
        }
        cluster = next;
    }
}

bool FAT_short_name_match(const char *path, int len, FAT_Entry *entry){
    int index1 = mem_find(path, len, '.');
    int index2 = mem_find(path, len, '/');

    if(index1 == -1){
        index1 = len;
    }
    if(index2 == -1){
        index2 = len;
    }
    if(index1 > index2){
        index1 = index2;
    }

    bool match = true;
    for(int i = 0; i < min(index1, 8); i++){
        if(entry->name[i] == ' '){
            match = false;
            break;
        }else{
            if(to_upper(path[i]) != entry->name[i]){
                match = false;
                break;
            }
        }
    }
    for(int i = min(index1, 8); i < 8; i++){
        if(entry->name[i] != ' '){
            match = false;
            break;
        }
    }

    if(!match){
        return false;
    }

    for(int i = 0; i < min(index2 - index1 - 1, 3); i++){
        if(entry->extension[i] == ' '){
            match = false;
            break;
        }else{
            if(to_upper(path[i + index1 + 1]) != entry->extension[i]){
                match = false;
                break;
            }
        }
    }

    if(!match){
        return false;
    }

    return true;
}

bool FAT_name_buffer_match(const char *path, int len){
    int index = mem_find(path, len, '/');
    if(index == -1){
        index = len;
    }
    if(index != name_buffer_len){
        return false;
    }
    for(int i = 0; i < index; i++){
        if(to_upper(path[i]) != to_upper(name_buffer[i])){
            return false;
        }
    }
    return true;
}

void FAT_add_to_name_buffer(FAT_Entry *entry){
    if(entry->attributes == 0xF){
        //VFAT name entry
        VFAT_Entry *ventry = (VFAT_Entry*)entry;
        for(int i = 0; i < 5; i++){
            if((int8_t)ventry->name1[i] == -1){
                break;
            }
            if((int8_t)ventry->name1[i] == 0){
                break;
            }
            name_buffer[name_buffer_len++] = ventry->name1[i];
        }
        for(int i = 0; i < 6; i++){
            if((int8_t)ventry->name2[i] == -1){
                break;
            }
            if((int8_t)ventry->name2[i] == 0){
                break;
            }
            name_buffer[name_buffer_len++] = ventry->name2[i];
        }
        for(int i = 0; i < 2; i++){
            if((int8_t)ventry->name3[i] == -1){
                break;
            }
            if((int8_t)ventry->name3[i] == 0){
                break;
            }
            name_buffer[name_buffer_len++] = ventry->name3[i];
        }
    }else{
        if(!(entry->attributes & FAT_ATTRIBUTE_VOLUME_LABLE)){
            if(entry->attributes & FAT_ATTRIBUTE_DIRECTORY){
                for(int i = 0; i < 8; i++){
                    if(entry->name[i] != ' '){
                        name_buffer[name_buffer_len++] = entry->name[i];
                    }
                }
            }else{
                for(int i = 0; i < 8; i++){
                    if(entry->name[i] != ' '){
                        name_buffer[name_buffer_len++] = entry->name[i];
                    }
                }
                name_buffer[name_buffer_len++] = '.';
                for(int i = 0; i < 3; i++){
                    if(entry->extension[i] != ' '){
                        name_buffer[name_buffer_len++] = entry->extension[i];
                    }
                }
            }            
        }
    }
}

FAT_Entry FAT_get_file_entry(const char *path, uint8_t *dirBuffer, uint32_t dirBufferSize){
    FAT_Entry *entry = (FAT_Entry*)dirBuffer;
    name_buffer_len = 0;
    int len = str_len(path);
    bool isLongName = false;

    for(int i = 0; i < dirBufferSize / sizeof(FAT_Entry); i++){
        if(entry->attributes == 0x0f){
            //VFAT long file name 
            isLongName = true;

            FAT_Entry *entry2 = entry;
            int j = 0;
            for(j = i; j < dirBufferSize / sizeof(FAT_Entry); j++){
                if(entry2->attributes != 0x0f){
                    break;
                }
                entry2++;
            }
            entry2--;
            j--;
            for(FAT_Entry *e = entry2; e >= entry; e--){
                FAT_add_to_name_buffer(e);
            }

            i = j;
            entry = entry2;
        }
        
        if(entry->attributes & FAT_ATTRIBUTE_VOLUME_LABLE){
            entry++;
            continue;
        }

        if(!isLongName){
            FAT_add_to_name_buffer(entry);
        }

        name_buffer[name_buffer_len] = '\0';

        if(FAT_name_buffer_match(path, len)){
            name_buffer_len = 0;
            isLongName = false;
            if(entry->attributes & FAT_ATTRIBUTE_DIRECTORY){

                int pathOffset = mem_find(path, len, '/');
                if(pathOffset == -1 || pathOffset == len){
                    return FAT_Entry();
                }

                //read next directory and search in it
                int newDirBufferSize = FAT_cluser_chain_size(entry->file_start_cluster);
                newDirBufferSize *= header.bpb.sectors_per_cluster * header.bpb.bytes_per_sector;
                uint8_t *newDirBuffer = (uint8_t*)MM_map(newDirBufferSize);
                FAT_read_clusters(entry->file_start_cluster, newDirBuffer, newDirBufferSize);
                FAT_Entry res = FAT_get_file_entry(path + pathOffset + 1, newDirBuffer, newDirBufferSize);
                MM_unmap(newDirBuffer);
                return res;

            }else{
                return *entry;
            }
        }
        name_buffer_len = 0;
        isLongName = false;
        entry++;
    }
    return FAT_Entry();
}

uint32_t FAT_file_size(const char *path){
    FAT_Entry entry =  FAT_get_file_entry(path, root_table, root_table_size);
    if(entry.file_start_cluster == 0){
        return 0;
    }else{
        return entry.file_size;
    }
}

bool FAT_read_file(const char *path, uint8_t *buffer, uint32_t bufferSize){
    FAT_Entry entry =  FAT_get_file_entry(path, root_table, root_table_size);
    if(entry.file_start_cluster == 0){
        return false;
    }else{
        FAT_read_clusters(entry.file_start_cluster, buffer, bufferSize);
        return true;
    }
}

void FAT_write_file(const char *path, const uint8_t *buffer, uint32_t bufferSize);

void FAT_create_directory(const char *path);
void FAT_list_directory(const char *path, uint8_t *buffer, uint32_t bufferSize);

void FAT_remove_file(const char *path);
void FAT_remove_directory(const char *path);
