static void OTA_HandleEnd(const OtaPacket_t *packet)
{
    uint32_t crc;

    if (packet->payload_length != 0U ||
        Session.state != OTA_SESSION_RECEIVING ||
        Session.expected_offset != Session.image_size)
    {
        OTA_SendReply(packet->sequence, packet->command,
                      OTA_RESULT_OFFSET_MISMATCH, Session.expected_offset);
        return;
    }

    crc = CRC32_CalculateFlash(OTA_CACHE_ADDR, Session.image_size);
    if (crc != Session.image_crc32)
    {
        Session.state = OTA_SESSION_ERROR;
        OTA_SendReply(packet->sequence, packet->command,
                      OTA_RESULT_IMAGE_CRC_FAILED, crc);
        return;
    }
    if (!ImageVectorIsValid(OTA_CACHE_ADDR, Session.image_size))
    {
        Session.state = OTA_SESSION_ERROR;
        OTA_SendReply(packet->sequence, packet->command,
                      OTA_RESULT_IMAGE_INVALID, 0U);
        return;
    }

    Session.state = OTA_SESSION_VERIFIED;
    OTA_SendReply(packet->sequence, packet->command,
                  OTA_RESULT_OK, Session.image_size);
}

#include "part_e2.inc"
