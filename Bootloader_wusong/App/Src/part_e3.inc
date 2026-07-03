static void OTA_Dispatch(const OtaPacket_t *packet)
{
    switch (packet->command)
    {
    case OTA_CMD_HELLO:
        OTA_HandleHello(packet);
        break;
    case OTA_CMD_BEGIN:
        OTA_HandleBegin(packet);
        break;
    case OTA_CMD_DATA:
        OTA_HandleData(packet);
        break;
    case OTA_CMD_END:
        OTA_HandleEnd(packet);
        break;
    case OTA_CMD_INSTALL:
        OTA_HandleInstall(packet);
        break;
    case OTA_CMD_STATUS:
        OTA_SendReply(packet->sequence, packet->command,
                      OTA_RESULT_OK, Session.expected_offset);
        break;
    case OTA_CMD_ABORT:
        memset(&Session, 0, sizeof(Session));
        Session.state = OTA_SESSION_IDLE;
        OTA_SendReply(packet->sequence, packet->command, OTA_RESULT_OK, 0U);
        break;
    case OTA_CMD_REBOOT:
        OTA_SendReply(packet->sequence, packet->command, OTA_RESULT_OK, 0U);
        HAL_Delay(100U);
        NVIC_SystemReset();
        break;
    default:
        OTA_SendReply(packet->sequence, packet->command,
                      OTA_RESULT_UNKNOWN_COMMAND, Session.expected_offset);
        break;
    }
}

#include "ota_run.inc"
