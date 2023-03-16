/*
    Tool to patch the stage 1 of the bootloader (boot sector).
    The sector table of stage 1 if filled with the sectors of stage 2.
    Stage 1 will then use the sector table to load stage 2.
*/

#include <fstream>

//configuration
uint16_t stage2_destination = 0x500;
uint32_t stage2_sector_table_size = 60;
uint32_t stage2_sector_table_position = 512 - 2 - stage2_sector_table_size;

//globals
std::ofstream stream;
uint32_t last_sector = 0;
uint32_t last_sector_count = 0;
uint32_t sector_table_ptr = stage2_sector_table_position;

void write_destination(uint16_t destination){
    stream.seekp(stage2_sector_table_position - 2);
    stream.write((char*)&destination, 2);
}

void write_flush(){
    if(sector_table_ptr >= stage2_sector_table_position + stage2_sector_table_size){
        printf("end of sector table reached\n");
        exit(1);
    }

    if(last_sector_count > 0){
        uint32_t index = last_sector - last_sector_count + 1;
        uint8_t length = last_sector_count;

        stream.seekp(sector_table_ptr);
        stream.write((char*)&index, 4);
        stream.seekp(sector_table_ptr + 4);
        stream.write((char*)&length, 1);
        sector_table_ptr += 5;

        last_sector = 0;
        last_sector_count = 0;
    }
}

void write_sector(uint32_t sector){
    if(last_sector_count == 0){
        last_sector = sector;
        last_sector_count++;
    }else{
        if(last_sector == sector - 1){
            last_sector = sector;
            last_sector_count++;
        }else{
            write_flush();
            last_sector = sector;
            last_sector_count = 1;
        }
    }
}

int main(int argc, char *argv[]){
    if(argc > 2){
        const char *stage1_file = argv[1];
        
        uint32_t first_sector = 0x21;
        uint32_t count = 0; //20 kB

        const char *stage2_file = argv[2];
        std::ifstream stream2(stage2_file);
        if(stream2.is_open()){
            stream2.seekg(0, stream2.end);
            int size = stream2.tellg();
            count = ((size - 1) / 512) + 1;
        }else{
            printf("file %s not found\n", stage2_file);
            return  1;
        }

        stream.open(stage1_file, std::ios::binary | std::ios::out | std::ios::in);
        if(stream.is_open()){
            write_destination(stage2_destination);


            for(int i = first_sector; i < first_sector + count; i++){
                write_sector(i);
            }
            write_flush();
        }else{
            printf("file %s not found\n", stage1_file);
            return  1;
        }
    }else{
        printf("usage: stage1_patcher <file>\n");
        return  1;
    }
    return 0;
}
