static bool StackPointerIsValid(uint32_t value)
{
    return value >= 0x20000000U && value < 0x20020000U;
}

static bool ResetHandlerIsValid(uint32_t value)
{
    uint32_t address = value & ~1U;
    return (value & 1U) != 0U && address >= APP_ADDR && address < APP_END_ADDR;
}

static bool ImageVectorIsValid(uint32_t address, uint32_t image_size)
{
    if (image_size < 8U || image_size > APP_MAX_SIZE || image_size > OTA_CACHE_MAX_SIZE)
        return false;
    return StackPointerIsValid(*(volatile uint32_t *)address) &&
           ResetHandlerIsValid(*(volatile uint32_t *)(address + 4U));
}

static bool AppIsValid(void)
{
    return StackPointerIsValid(*(volatile uint32_t *)APP_ADDR) &&
           ResetHandlerIsValid(*(volatile uint32_t *)(APP_ADDR + 4U));
}

static void LightWaiting(void)
{
    sm16306s_data[0] = 0x00U;
    sm16306s_data[1] = 0x00U;
    SM16306S_SetLight(sm16306s_data);
}

static void LightSuccess(void)
{
    sm16306s_data[0] = 0xFFU;
    sm16306s_data[1] = 0xFFU;
    SM16306S_SetLight(sm16306s_data);
}

static void LightError(void)
{
    sm16306s_data[0] = 0xAAU;
    sm16306s_data[1] = 0x55U;
    SM16306S_SetLight(sm16306s_data);
}

static HAL_StatusTypeDef InstallCachedImage(uint32_t image_size, uint32_t image_crc32)
{
    uint32_t first_sector;
    uint32_t last_sector;
    uint32_t offset = 0U;
    HAL_StatusTypeDef status = HAL_ERROR;

    if (!ImageVectorIsValid(OTA_CACHE_ADDR, image_size) ||
        CRC32_CalculateFlash(OTA_CACHE_ADDR, image_size) != image_crc32)
        return HAL_ERROR;

    first_sector = Flash_GetSector(APP_ADDR);
    last_sector = Flash_GetSector(APP_ADDR + image_size - 1U);
    if (Flash_EraseRange(first_sector, last_sector - first_sector + 1U) != HAL_OK ||
        HAL_FLASH_Unlock() != HAL_OK)
        return HAL_ERROR;

    Flash_ClearFlags();
    while (offset < image_size)
    {
        uint32_t block_size = image_size - offset > sizeof(r_buffer) ?
                              sizeof(r_buffer) : image_size - offset;
        if (Flash_WriteUnlocked(APP_ADDR + offset,
                                (const uint8_t *)(OTA_CACHE_ADDR + offset),
                                block_size) != HAL_OK)
            goto finish;
        offset += block_size;
    }

    if (CRC32_CalculateFlash(APP_ADDR, image_size) == image_crc32 && AppIsValid())
        status = HAL_OK;

finish:
    HAL_FLASH_Lock();
    return status;
}

#include "ota_transport.inc"
