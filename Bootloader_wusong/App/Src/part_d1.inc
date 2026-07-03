static bool OTA_DataFrameValid(const OtaPacket_t *packet,
                               uint32_t offset,
                               uint16_t length)
{
    return Session.state == OTA_SESSION_RECEIVING &&
           length > 0U && length <= OTA_MAX_DATA_SIZE &&
           packet->payload_length == (uint16_t)(length + 6U) &&
           offset <= Session.image_size &&
           length <= Session.image_size - offset;
}

#include "part_d2.inc"
