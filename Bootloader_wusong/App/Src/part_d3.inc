static bool OTA_DataAcceptNew(const OtaPacket_t *packet,
                              uint32_t offset,
                              uint16_t length,
                              const uint8_t *data)
{
    if (offset != Session.expected_offset)
        return false;
    if (!OTA_DataStore(offset, data, length))
    {
        Session.state = OTA_SESSION_ERROR;
        OTA_SendReply(packet->sequence, packet->command,
                      OTA_RESULT_FLASH_WRITE_FAILED, Session.expected_offset);
        return true;
    }
    Session.expected_offset += length;
    OTA_SendReply(packet->sequence, packet->command,
                  OTA_RESULT_OK, Session.expected_offset);
    return true;
}

#include "part_d4.inc"
