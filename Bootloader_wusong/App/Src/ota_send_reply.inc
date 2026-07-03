static void OTA_SendReply(uint16_t sequence,
                          uint8_t request_command,
                          uint8_t result,
                          uint32_t value)
{
    uint8_t payload[10];
    payload[0] = request_command;
    payload[1] = result;
    payload[2] = Session.state;
    payload[3] = 0U;
    WriteU32LE(&payload[4], value);
    WriteU16LE(&payload[8], OTA_MAX_DATA_SIZE);
    (void)OTA_SendPacket(result == OTA_RESULT_OK ? OTA_CMD_ACK : OTA_CMD_NACK,
                         sequence, payload, sizeof(payload));
}
