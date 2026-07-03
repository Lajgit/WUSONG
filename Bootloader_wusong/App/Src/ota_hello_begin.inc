static void OTA_HandleHello(const OtaPacket_t *packet)
{
    OTA_SendReply(packet->sequence, packet->command,
                  OTA_RESULT_OK, OTA_BOOT_VERSION);
}

static void OTA_HandleBegin(const OtaPacket_t *packet)
{
    uint32_t target_magic;
    uint32_t image_size;

    if (packet->payload_length != 16U)
    {
        OTA_SendReply(packet->sequence, packet->command,
                      OTA_RESULT_BAD_LENGTH, 0U);
        return;
    }

    target_magic = ReadU32LE(&packet->payload[0]);
    Session.version_code = ReadU32LE(&packet->payload[4]);
    image_size = ReadU32LE(&packet->payload[8]);
    Session.image_crc32 = ReadU32LE(&packet->payload[12]);

    if (target_magic != OTA_TARGET_MAGIC)
    {
        OTA_SendReply(packet->sequence, packet->command,
                      OTA_RESULT_BAD_TARGET, target_magic);
        return;
    }
    if (image_size < 8U || image_size > APP_MAX_SIZE ||
        image_size > OTA_CACHE_MAX_SIZE)
    {
        OTA_SendReply(packet->sequence, packet->command,
                      OTA_RESULT_BAD_IMAGE_SIZE, image_size);
        return;
    }
    if (Flash_EraseRange(FLASH_SECTOR_9, 2U) != HAL_OK)
    {
        Session.state = OTA_SESSION_ERROR;
        OTA_SendReply(packet->sequence, packet->command,
                      OTA_RESULT_FLASH_ERASE_FAILED, 0U);
        return;
    }

    Session.state = OTA_SESSION_RECEIVING;
    Session.image_size = image_size;
    Session.expected_offset = 0U;
    OTA_SendReply(packet->sequence, packet->command, OTA_RESULT_OK, 0U);
}
