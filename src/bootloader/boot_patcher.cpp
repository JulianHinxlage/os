/*
    Tool to patch an image with the bootloader.
    Stage 1 is copied into the boot sector of the image.
    The sector table of stage 1 if filled with the sector locations of stage 2.
    Stage 1 will then use the sector table to load stage 2.
*/

#include <fstream>
#include <vector>

//configuration
uint16_t stage2_destination = 0x500;
uint32_t stage1_sector_table_size = 60;
uint32_t stage1_sector_table_position = 512 - 2 - stage1_sector_table_size;
uint32_t stage1_header_size = 0x5A;
const uint32_t sector_size = 512;

//globals
uint32_t last_sector = 0;
uint32_t last_sector_count = 0;
uint32_t sector_table_ptr = stage1_sector_table_position;
const char *image_file = nullptr;

//streams
std::ofstream image_stream;
std::ifstream stage1_stream;
std::ifstream stage2_stream;

int file_size(std::ifstream& stream){
    int pos = stream.tellg();
    stream.seekg(0, stream.end);
    int size = stream.tellg();
    stream.seekg(pos);
    return size;
}

int file_size_sectors(std::ifstream& stream){
    return ((file_size(stream) - 1) / sector_size) + 1;
}

void sector_table_write_destination(uint16_t destination){
    image_stream.seekp(stage1_sector_table_position - 2);
    image_stream.write((char*)&destination, 2);
}

void sector_table_write_flush(){
    if(sector_table_ptr >= stage1_sector_table_position + stage1_sector_table_size){
        printf("end of sector table reached\n");
        exit(1);
    }

    if(last_sector_count > 0){
        uint32_t index = last_sector - last_sector_count + 1;
        uint8_t length = last_sector_count;

        image_stream.seekp(sector_table_ptr);
        image_stream.write((char*)&index, 4);
        image_stream.seekp(sector_table_ptr + 4);
        image_stream.write((char*)&length, 1);
        sector_table_ptr += 5;

        last_sector = 0;
        last_sector_count = 0;
    }
}

void sector_table_write_sector(uint32_t sector){
    if(last_sector_count == 0){
        last_sector = sector;
        last_sector_count++;
    }else{
        if(last_sector == sector - 1){
            last_sector = sector;
            last_sector_count++;
        }else{
            sector_table_write_flush();
            last_sector = sector;
            last_sector_count = 1;
        }
    }
}

void copy_boot_sector(){
    for(int i = 0; i < sector_size; i++){
        if(i >= stage1_header_size || i < 3){
            char c;
            stage1_stream.seekg(i);
            stage1_stream.read(&c, 1);
            image_stream.seekp(i);
            image_stream.write(&c, 1);
        }
    }
}

bool cmp_buffer(uint8_t *a, uint8_t *b, int size){
    for(int i = 0; i < size; i++){
        if(a[i] != b[i]){
            return false;
        }
    }
    return true;
}

std::vector<uint32_t> seach_stage2_content(){
    uint32_t image_buffer_size = 1024 * 1024 * 32;

    std::ifstream image_stream_read;
    image_stream_read.open(image_file);
    if(!image_stream_read.is_open()){
        printf("file not found %s\n", image_file);
        exit(1);
    }

    image_buffer_size = std::max(image_buffer_size, (uint32_t)file_size(image_stream_read));
    uint8_t *image_buffer = (uint8_t*)malloc(image_buffer_size);
    image_stream_read.seekg(0);
    image_stream_read.read((char*)image_buffer, image_buffer_size);

    uint32_t stage2_size = file_size(stage2_stream);
    uint32_t sector_count = file_size_sectors(stage2_stream);
    uint8_t buffer[sector_size];

    std::vector<uint32_t> result;

    for(int i = 0; i < sector_count; i++){
        int buffer_size = 0;
        if(i == sector_count - 1){
            buffer_size = stage2_size - i * sector_size;
        }else{
            buffer_size = 512;
        }
        stage2_stream.seekg(i * sector_size);
        stage2_stream.read((char*)buffer, buffer_size);


        bool found = false;
        for(int j = 0; j < image_buffer_size / sector_size; j++){
            if(cmp_buffer(buffer, &image_buffer[j * sector_size], buffer_size)){
                result.push_back(j);
                found = true;
                break;
            }
        }

        if(!found){
            printf("stage 2 sector %i not found in image\n", i);
            exit(1);
        }

    }

    free(image_buffer);
    return result;
}

void sector_table_fill() {
    std::vector<uint32_t> sector_list = seach_stage2_content();
    sector_table_write_destination(stage2_destination);
    for(uint32_t i : sector_list){
        sector_table_write_sector(i);
    }
    sector_table_write_flush();
}

int main(int argc, char *argv[]){
    if(argc > 3){
        image_file = argv[1];
        const char *stage1_file = argv[2];
        const char *stage2_file = argv[3];

        image_stream.open(image_file, std::ios::binary | std::ios::out | std::ios::in);
        if(!image_stream.is_open()){
            printf("file not found: %s\n", image_file);
            return 1;
        }

        stage1_stream.open(stage1_file, std::ios::binary);
        if(!stage1_stream.is_open()){
            printf("file not found: %s\n", stage1_file);
            return 1;
        }

        stage2_stream.open(stage2_file, std::ios::binary);
        if(!stage2_stream.is_open()){
            printf("file not found: %s\n", stage2_file);
            return 1;
        }

        copy_boot_sector();
        sector_table_fill();
    }else{
        printf("usage: boot_patcher <image> <stage1> <stage2>\n");
        return 1;
    }
    return 0;
}
