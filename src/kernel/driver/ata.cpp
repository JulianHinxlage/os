#include "hal/hal.h"
#include "io/printf.h"

//register
#define ATA_REG_DATA            0x1f0
#define ATA_REG_ERROR           0x1f1
#define ATA_REG_FEATURE         0x1f1
#define ATA_REG_SECTOR_COUNT    0x1f2
#define ATA_REG_LBA_LOW         0x1f3
#define ATA_REG_LBA_MID         0x1f4
#define ATA_REG_LBA_HIGH        0x1f5
#define ATA_REG_DRIVE_HEAD      0x1f6
#define ATA_REG_COMMAND         0x1f7
#define ATA_REG_STATUS          0x1f7

//command
#define ATA_CMD_READ_SECTORS       0x20
#define ATA_CMD_WRITE_SECTORS      0x30

//status
#define ATA_STATUS_BUSY                 0x80 
#define ATA_STATUS_DRIVE_READY          0x40 
#define ATA_STATUS_DRIVE_FAULT          0x20 
#define ATA_STATUS_SEEK_COMPLETE        0x10 
#define ATA_STATUS_DATA_READY           0x08 
#define ATA_STATUS_CORRECTED            0x04 
#define ATA_STATUS_INDEX                0x02 
#define ATA_STATUS_ERROR                0x01 

//error
#define ATA_ERROR_BAD_BLOCK             0x80
#define ATA_ERROR_UNCORRECTABLE_DATA    0x40
#define ATA_ERROR_MEDIA_CHANGED         0x20
#define ATA_ERROR_ID_MARK_NOT_FOUND     0x10
#define ATA_ERROR_MEDIA_CHANGE_REQUEST  0x08
#define ATA_ERROR_ABORTED               0x04
#define ATA_ERROR_TRACK_0_NOT_FOUND     0x02
#define ATA_ERROR_NO_ADDRESS            0x01

void ATA_error(uint8_t status, uint8_t error){
    printf("ATA error: status=0x%x, error=0x%x\n", status, error);
}

bool ATA_init(){
    //todo: set up the ATA controller
    return true;
}

bool ATA_wait_for_status(uint8_t statusBit, bool bitValue){
    while(true){
        uint8_t status = inb(ATA_REG_STATUS);
        if(status & ATA_STATUS_ERROR){
            uint8_t error = inb(ATA_REG_ERROR);
            ATA_error(status, error);
            return false;
        } 
        if(bitValue){
            if(status & statusBit){
                break;
            }
        }else{
            if(!(status & statusBit)){
                break;
            }
        }
    }
    return true;
}

void ATA_read_sectors(uint8_t drive, uint32_t sector, uint8_t count, uint8_t *buffer) {
    if(!ATA_wait_for_status(ATA_STATUS_BUSY, 0)){
        return;
    }
    if(!ATA_wait_for_status(ATA_STATUS_DRIVE_READY, 1)){
        return;
    }

    outb(ATA_REG_DRIVE_HEAD, 0xE0 | ((drive & 0x07) << 4) | ((sector >> 24) & 0x0F));
    outb(ATA_REG_SECTOR_COUNT, count);
    outb(ATA_REG_LBA_HIGH, (sector >> 16) & 0xff);
    outb(ATA_REG_LBA_MID, (sector >> 8) & 0xff);
    outb(ATA_REG_LBA_LOW, (sector >> 0) & 0xff);
    outb(ATA_REG_COMMAND, ATA_CMD_READ_SECTORS);

    if(!ATA_wait_for_status(ATA_STATUS_BUSY, 0)){
        return;
    }
    if(!ATA_wait_for_status(ATA_STATUS_DRIVE_READY, 1)){
        return;
    }
    if(!ATA_wait_for_status(ATA_STATUS_DATA_READY, 1)){
        return;
    }

    for (int i = 0; i < 256 * count; i++) {
        if(!ATA_wait_for_status(ATA_STATUS_DATA_READY, 1)){
            return;
        }
        ((uint16_t*)buffer)[i] = inw(ATA_REG_DATA);
    }
}

void ATA_write_sectors(uint8_t drive, uint32_t sector, uint8_t count, uint8_t *buffer) {
    if(!ATA_wait_for_status(ATA_STATUS_BUSY, 0)){
        return;
    }
    if(!ATA_wait_for_status(ATA_STATUS_DRIVE_READY, 1)){
        return;
    }

    outb(ATA_REG_DRIVE_HEAD, 0xE0 | ((drive & 0x07) << 4) | ((sector >> 24) & 0x0F));
    outb(ATA_REG_SECTOR_COUNT, count);
    outb(ATA_REG_LBA_HIGH, (sector >> 16) & 0xff);
    outb(ATA_REG_LBA_MID, (sector >> 8) & 0xff);
    outb(ATA_REG_LBA_LOW, (sector >> 0) & 0xff);
    outb(ATA_REG_COMMAND, ATA_CMD_WRITE_SECTORS);

    if(!ATA_wait_for_status(ATA_STATUS_BUSY, 0)){
        return;
    }
    if(!ATA_wait_for_status(ATA_STATUS_DRIVE_READY, 1)){
        return;
    }

    for (int i = 0; i < 256 * count; i++) {
        if(!ATA_wait_for_status(ATA_STATUS_DATA_READY, 1)){
            return;
        }
        outw(ATA_REG_DATA, ((uint16_t*)buffer)[i]);
    }
}

void ATA_interrupt(int irq){
    uint8_t status = inb(ATA_REG_STATUS);
    uint8_t error = inb(ATA_REG_ERROR);
    //printf("ATA interrupt: status=0x%x, error=0x%x\n", status, error);
}
