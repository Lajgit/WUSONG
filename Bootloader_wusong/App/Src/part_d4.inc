static bool OTA_DataAcceptDuplicate(const OtaPacket_t *packet,
                                    uint32_t offset,
                                    uint16_t length)
{
    if (offset >= Session.expected_offset ||
        offset + length > Session.expected_offset)
        return false;
    OTA_SendReply(packet->sequence, packet->command,
                  OTA_RESULT_OK, Session.expected_offset);
    return true;
}

#include "part_d5.inc"
