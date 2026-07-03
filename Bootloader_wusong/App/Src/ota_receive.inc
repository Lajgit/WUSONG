static HAL_StatusTypeDef OTA_ReceivePacket(OtaPacket_t *packet, uint32_t first_timeout)
{
    uint8_t byte;
    uint16_t length;

    while (1)
    {
        if (HAL_UART_Receive(&huart1, &byte, 1U, first_timeout) != HAL_OK)
            return HAL_TIMEOUT;
        if (byte != OTA_SOF1)
            continue;
        RxFrame[0] = byte;
        if (HAL_UART_Receive(&huart1, &RxFrame[1], 1U, 100U) != HAL_OK ||
            RxFrame[1] != OTA_SOF2)
            continue;
        if (HAL_UART_Receive(&huart1, &RxFrame[2], 6U, 1000U) != HAL_OK)
            return HAL_ERROR;

        packet->command = RxFrame[3];
        packet->sequence = ReadU16LE(&RxFrame[4]);
        length = ReadU16LE(&RxFrame[6]);
        packet->payload_length = length;

        if (RxFrame[2] != OTA_PROTOCOL_VERSION)
        {
            OTA_SendReply(packet->sequence, packet->command,
                          OTA_RESULT_BAD_PROTOCOL, Session.expected_offset);
            return HAL_ERROR;
        }
        if (length > OTA_MAX_PAYLOAD_SIZE)
        {
            OTA_SendReply(packet->sequence, packet->command,
                          OTA_RESULT_BAD_LENGTH, Session.expected_offset);
            return HAL_ERROR;
        }
        if (HAL_UART_Receive(&huart1, &RxFrame[8],
                             (uint16_t)(length + 4U), 3000U) != HAL_OK)
            return HAL_ERROR;
        if (ReadU32LE(&RxFrame[8U + length]) !=
            CRC32_Calculate(&RxFrame[2], 6U + length))
        {
            OTA_SendReply(packet->sequence, packet->command,
                          OTA_RESULT_BAD_FRAME_CRC, Session.expected_offset);
            return HAL_ERROR;
        }
        if (length > 0U)
            memcpy(packet->payload, &RxFrame[8], length);
        return HAL_OK;
    }
}

#include "ota_begin.inc"
