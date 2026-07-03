static void OTA_Run(void)
{
    while (1)
    {
        if (OTA_ReceivePacket(&Packet, 1000U) == HAL_OK)
            OTA_Dispatch(&Packet);
    }
}
