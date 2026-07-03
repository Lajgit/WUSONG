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
