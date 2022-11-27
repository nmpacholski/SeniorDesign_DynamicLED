#ifndef _SD_DISKIO_H_
#define _SD_DISKIO_H_

#include "Arduino.h"
#include "SPI.h"
#include "sd_defines.h"
// #include "diskio.h"

uint8_t sdcard_init(uint8_t cs, SPIClass * spi, int hz);
uint8_t sdcard_uninit(uint8_t pdrv);

bool sdcard_mount(uint8_t pdrv, const char* path, uint8_t max_files, bool format_if_empty);
uint8_t sdcard_unmount(uint8_t pdrv);

sdcard_type_t sdcard_type(uint8_t pdrv);
uint32_t sdcard_num_sectors(uint8_t pdrv);
uint32_t sdcard_sector_size(uint8_t pdrv);
bool sd_read_raw(uint8_t pdrv, uint8_t* buffer, uint32_t sector);
bool sd_write_raw(uint8_t pdrv, uint8_t* buffer, uint32_t sector);

#endif /* _SD_DISKIO_H_ */
