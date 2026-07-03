static bool OTA_DataStore(uint32_t offset, const uint8_t *data, uint16_t length)
{
    if (Flash_Write(OTA_CACHE_ADDR + offset, data, length) != HAL_OK)
        return false;
    return memcmp((const void *)(OTA_CACHE_ADDR + offset), data, length) == 0;
}

#include "part_d3.inc"
