static void OTA_HandleData(const OtaPacket_t *packet)
{
    uint32_t offset;
    uint16_t length;
    const uint8_t *data;

    if (packet->payload_length < 6U)
    {
        OTA_SendReply(packet->sequence, packet->command,
                      OTA_RESULT_BAD_LENGTH, Session.expected_offset);
        return;
    }

    offset = ReadU32LE(&packet->payload[0]);
    length = ReadU16LE(&packet->payload[4]);
    data = &packet->payload[6];

    if (!OTA_DataFrameValid(packet, offset, length))
    {
        OTA_SendReply(packet->sequence, packet->command,
                      OTA_RESULT_BAD_LENGTH, Session.expected_offset);
        return;
    }

    if (OTA_DataAcceptNew(packet, offset, length, data))
        return;
    if (OTA_DataAcceptDuplicate(packet, offset, length))
        return;

    OTA_SendReply(packet->sequence, packet->command,
                  OTA_RESULT_OFFSET_MISMATCH, Session.expected_offset);
}

#include "ota_end.inc"
