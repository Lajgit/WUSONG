static void OTA_HandleInstall(const OtaPacket_t *packet)
{
    if (packet->payload_length != 0U || Session.state != OTA_SESSION_VERIFIED)
    {
        OTA_SendReply(packet->sequence, packet->command,
                      OTA_RESULT_NO_VALID_IMAGE, 0U);
        return;
    }

    Session.state = OTA_SESSION_INSTALLING;
    if (InstallCachedImage(Session.image_size, Session.image_crc32) != HAL_OK)
    {
        Session.state = OTA_SESSION_ERROR;
        LightError();
        OTA_SendReply(packet->sequence, packet->command,
                      OTA_RESULT_INSTALL_FAILED, 0U);
        return;
    }

    Session.state = OTA_SESSION_INSTALLED;
    OTA_SendReply(packet->sequence, packet->command,
                  OTA_RESULT_OK, Session.version_code);
    LightSuccess();
    HAL_Delay(300U);
    NVIC_SystemReset();
}

#include "part_e3.inc"
