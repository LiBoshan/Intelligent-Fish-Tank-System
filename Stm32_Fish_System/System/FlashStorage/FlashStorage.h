#ifndef  __FLASH_STORAGE_H
#define  __FLASH_STORAGE_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f10x_flash.h"
#include <string.h>

#define FLASH_PAGE_SIZE             1024
#define STORAGE_FLAG_VALID          0xA5A5

#define FLASH_STORAGE_PAGE0_BASE    0x0800FC00
#define FLASH_STORAGE_PAGE1_BASE    0x0800F800

bool FlashStorage_Write(uint8_t *data, uint8_t data_len);
bool FlashStorage_Read(uint8_t *data, uint8_t data_len);

#endif
