extern FATFS fs;
extern uint8_t r_buffer[1024];
extern uint8_t sm16306s_data[2];

typedef struct
{
    uint8_t command;
    uint16_t sequence;
    uint16_t payload_length;
    uint8_t payload[OTA_MAX_PAYLOAD_SIZE];
} OtaPacket_t;

typedef struct
{
    uint8_t state;
    uint32_t version_code;
    uint32_t image_size;
    uint32_t image_crc32;
    uint32_t expected_offset;
} OtaSession_t;

static OtaSession_t Session;
static OtaPacket_t Packet;
static FIL SdFirmwareFile;
static uint8_t RxFrame[2U + 6U + OTA_MAX_PAYLOAD_SIZE + 4U];
static uint8_t TxFrame[2U + 6U + 10U + 4U];

static uint16_t ReadU16LE(const uint8_t *data)
{
    return (uint16_t)data[0] | ((uint16_t)data[1] << 8U);
}

static uint32_t ReadU32LE(const uint8_t *data)
{
    return (uint32_t)data[0] | ((uint32_t)data[1] << 8U) |
           ((uint32_t)data[2] << 16U) | ((uint32_t)data[3] << 24U);
}

static void WriteU16LE(uint8_t *data, uint16_t value)
{
    data[0] = (uint8_t)value;
    data[1] = (uint8_t)(value >> 8U);
}

static void WriteU32LE(uint8_t *data, uint32_t value)
{
    data[0] = (uint8_t)value;
    data[1] = (uint8_t)(value >> 8U);
    data[2] = (uint8_t)(value >> 16U);
    data[3] = (uint8_t)(value >> 24U);
}

static uint32_t CRC32_UpdateByte(uint32_t crc, uint8_t data)
{
    uint8_t i;
    crc ^= data;
    for (i = 0U; i < 8U; i++)
        crc = (crc & 1U) != 0U ? (crc >> 1U) ^ 0xEDB88320U : crc >> 1U;
    return crc;
}

static uint32_t CRC32_Update(uint32_t crc, const uint8_t *data, uint32_t length)
{
    uint32_t i;
    for (i = 0U; i < length; i++)
        crc = CRC32_UpdateByte(crc, data[i]);
    return crc;
}

static uint32_t CRC32_Calculate(const uint8_t *data, uint32_t length)
{
    if (data == NULL && length != 0U)
        return 0U;
    return CRC32_Update(0xFFFFFFFFU, data, length) ^ 0xFFFFFFFFU;
}

static uint32_t CRC32_CalculateFlash(uint32_t address, uint32_t length)
{
    return CRC32_Calculate((const uint8_t *)address, length);
}

static void Flash_ClearFlags(void)
{
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                           FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
}

static uint32_t Flash_GetSector(uint32_t address)
{
    if (address < 0x08004000U) return FLASH_SECTOR_0;
    if (address < 0x08008000U) return FLASH_SECTOR_1;
    if (address < 0x0800C000U) return FLASH_SECTOR_2;
    if (address < 0x08010000U) return FLASH_SECTOR_3;
    if (address < 0x08020000U) return FLASH_SECTOR_4;
    if (address < 0x08040000U) return FLASH_SECTOR_5;
    if (address < 0x08060000U) return FLASH_SECTOR_6;
    if (address < 0x08080000U) return FLASH_SECTOR_7;
    if (address < 0x080A0000U) return FLASH_SECTOR_8;
    if (address < 0x080C0000U) return FLASH_SECTOR_9;
    if (address < 0x080E0000U) return FLASH_SECTOR_10;
    return FLASH_SECTOR_11;
}

static HAL_StatusTypeDef Flash_WriteUnlocked(uint32_t address,
                                              const uint8_t *data,
                                              uint32_t size)
{
    uint32_t offset = 0U;

    if ((address & 3U) != 0U || data == NULL || size == 0U)
        return HAL_ERROR;

    while (offset < size)
    {
        uint32_t word = 0xFFFFFFFFU;
        uint32_t copy_size = size - offset >= 4U ? 4U : size - offset;
        memcpy(&word, &data[offset], copy_size);
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, word) != HAL_OK)
            return HAL_ERROR;
        address += 4U;
        offset += copy_size;
    }
    return HAL_OK;
}

static HAL_StatusTypeDef Flash_Write(uint32_t address,
                                     const uint8_t *data,
                                     uint32_t size)
{
    HAL_StatusTypeDef status;

    if (HAL_FLASH_Unlock() != HAL_OK)
        return HAL_ERROR;
    Flash_ClearFlags();
    status = Flash_WriteUnlocked(address, data, size);
    HAL_FLASH_Lock();
    return status;
}

HAL_StatusTypeDef Flash_Program(uint32_t StartAddress, uint8_t *Data, uint32_t Size)
{
    return Flash_WriteUnlocked(StartAddress, Data, Size);
}

static HAL_StatusTypeDef Flash_EraseRange(uint32_t first_sector, uint32_t count)
{
    FLASH_EraseInitTypeDef erase = {0};
    uint32_t sector_error = 0U;
    HAL_StatusTypeDef status;

    if (HAL_FLASH_Unlock() != HAL_OK)
        return HAL_ERROR;

    Flash_ClearFlags();
    erase.TypeErase = FLASH_TYPEERASE_SECTORS;
    erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    erase.Sector = first_sector;
    erase.NbSectors = count;
    status = HAL_FLASHEx_Erase(&erase, &sector_error);
    HAL_FLASH_Lock();
    return status;
}

#include "ota_vector.inc"
