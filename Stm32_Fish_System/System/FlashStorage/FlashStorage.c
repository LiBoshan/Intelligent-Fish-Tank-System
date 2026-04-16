#include "FlashStorage.h"

static uint8_t calc_crc8(uint8_t *data, uint8_t len);
static bool is_page_valid(uint32_t page_addr);
static void erase_page(uint32_t page_addr);
static void write_data_to_page(uint32_t page_addr, uint8_t *data, uint8_t len);

bool FlashStorage_Write(uint8_t *data, uint8_t data_len)
{
    uint32_t active_page = 0;
    uint32_t backup_page = 0;
    uint8_t new_data_with_crc[data_len + 1];
    uint8_t crc;

    if (is_page_valid(FLASH_STORAGE_PAGE0_BASE))
    {
        active_page = FLASH_STORAGE_PAGE0_BASE;
        backup_page = FLASH_STORAGE_PAGE1_BASE;
    }
    else if (is_page_valid(FLASH_STORAGE_PAGE1_BASE))
    {
        active_page = FLASH_STORAGE_PAGE1_BASE;
        backup_page = FLASH_STORAGE_PAGE0_BASE;
    }
    else
    {
        erase_page(FLASH_STORAGE_PAGE0_BASE);
        active_page = FLASH_STORAGE_PAGE0_BASE;
        backup_page = FLASH_STORAGE_PAGE1_BASE;
    }

    crc = calc_crc8(data, data_len);
    memcpy (new_data_with_crc, data, data_len);
    new_data_with_crc[data_len] = crc;

    uint8_t same = 1;
    for (uint8_t i = 0; i < data_len; i++)
    {
        if (*(volatile uint8_t*)(active_page + i) != new_data_with_crc[i])
        {
            same = 0;
            break;
        }
    }
    if (same) return true;
    erase_page(backup_page);
    write_data_to_page(backup_page, new_data_with_crc, data_len + 1);


    uint32_t flag_addr = backup_page + FLASH_PAGE_SIZE - 2;
    FLASH_Unlock();
    FLASH_ProgramHalfWord(flag_addr, STORAGE_FLAG_VALID);
    FLASH_Lock();

    return true;
}

bool FlashStorage_Read(uint8_t *data, uint8_t data_len)
{
    uint32_t active_page = 0;
    uint8_t stored_data[data_len + 1];

    if (is_page_valid(FLASH_STORAGE_PAGE0_BASE))
    {
        active_page = FLASH_STORAGE_PAGE0_BASE;
    }
    else if (is_page_valid(FLASH_STORAGE_PAGE1_BASE))
    {
        active_page = FLASH_STORAGE_PAGE1_BASE;
    }
    else
    {
        return false;
    }
 
    for (uint8_t i = 0; i < data_len + 1; i++)
    {
        stored_data[i] = *(volatile uint8_t*)(active_page + i);
    }
    
    uint8_t calc_crc = calc_crc8(stored_data, data_len);
    if (calc_crc != stored_data[data_len])
    {
        return false;
    }
    memcpy(data, stored_data, data_len);
    return true;
}

static uint8_t calc_crc8(uint8_t *data, uint8_t len)
{
    uint8_t crc = 0;
    for (uint8_t i = 0; i < len; i++)
    {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++)
        {
            if (crc & 0x80) crc = (crc << 1) ^ 0x07;

            else crc <<= 1;
        }
    }
    return crc;
}

static bool is_page_valid(uint32_t page_addr)
{
    uint16_t flag = *(volatile uint16_t*)(page_addr + FLASH_PAGE_SIZE - 2);
    return (flag == STORAGE_FLAG_VALID);
}

static void erase_page(uint32_t page_addr)
{
    FLASH_Unlock();
    FLASH_ErasePage(page_addr);
    FLASH_Lock();
}

static void write_data_to_page(uint32_t page_addr, uint8_t *data, uint8_t len)
{
    FLASH_Unlock();

    uint32_t addr = page_addr;
    uint8_t remaining = len;
    uint8_t *ptr = data;

    if (addr & 0x01)
    {
        uint16_t tmp = *ptr;
        tmp |= 0xFF00;
        FLASH_ProgramHalfWord(addr, tmp);
        addr += 1;
        ptr += 1;
        remaining -= 1;
    }

    while (remaining >= 2)
    {
        uint16_t halfword = *((uint16_t*)ptr);
        FLASH_ProgramHalfWord(addr, halfword);
        addr += 2;
        ptr += 2;
        remaining -= 2;
    }

    if (remaining > 0)
    {
        uint16_t last = *ptr;
        last |= 0xFF00;
        FLASH_ProgramHalfWord(addr, last);
    }

    FLASH_Lock();
}
