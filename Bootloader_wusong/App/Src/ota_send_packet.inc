static HAL_StatusTypeDef OTA_SendPacket(uint8_t command,
                                        uint16_t sequence,
                                        const uint8_t *payload,
                                        uint16_t length)
{
    if (length > 10U)
        return HAL_ERROR;
    TxFrame[0] = OTA_SOF1;
    TxFrame[1] = OTA_SOF2;
    TxFrame[2] = OTA_PROTOCOL_VERSION;
    TxFrame[3] = command;
    WriteU16LE(&TxFrame[4], sequence);
    WriteU16LE(&TxFrame[6], length);
    if (length > 0U && payload != NULL)
        memcpy(&TxFrame[8], payload, length);
    WriteU32LE(&TxFrame[8U + length],
               CRC32_Calculate(&TxFrame[2], 6U + length));
    return HAL_UART_Transmit(&huart1, TxFrame, (uint16_t)(12U + length), 1000U);
}
