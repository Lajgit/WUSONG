static bool TrySdUpgrade(void)
{
    UINT bytes_read;
    uint32_t file_size;
    uint32_t offset = 0U;
    uint32_t crc = 0xFFFFFFFFU;
    bool ready = false;

    if (mount_disk(&fs, "", 0) != FR_OK)
    {
        (void)f_mount(NULL, "", 0);
        return false;
    }
    if (f_open(&SdFirmwareFile, FileName_Bin, FA_READ) != FR_OK)
    {
        (void)f_mount(NULL, "", 0);
        return false;
    }

    file_size = f_size(&SdFirmwareFile);
    if (file_size < 8U || file_size > APP_MAX_SIZE ||
        file_size > OTA_CACHE_MAX_SIZE ||
        Flash_EraseRange(FLASH_SECTOR_9, 2U) != HAL_OK)
        goto finish;

    while (offset < file_size)
    {
        uint32_t request_size = file_size - offset > sizeof(r_buffer) ?
                                sizeof(r_buffer) : file_size - offset;
        if (read_file(&SdFirmwareFile, r_buffer, request_size, &bytes_read) != FR_OK ||
            bytes_read == 0U ||
            Flash_Write(OTA_CACHE_ADDR + offset, r_buffer, bytes_read) != HAL_OK)
            goto finish;
        crc = CRC32_Update(crc, r_buffer, bytes_read);
        offset += bytes_read;
    }

    crc ^= 0xFFFFFFFFU;
    if (offset == file_size && ImageVectorIsValid(OTA_CACHE_ADDR, file_size) &&
        CRC32_CalculateFlash(OTA_CACHE_ADDR, file_size) == crc &&
        InstallCachedImage(file_size, crc) == HAL_OK)
        ready = true;

finish:
    (void)close_file(&SdFirmwareFile);
    (void)f_mount(NULL, "", 0);
    return ready;
}

void App_Bootloader(void)
{
    uint32_t start;
    memset(&Session, 0, sizeof(Session));
    Session.state = OTA_SESSION_IDLE;
    LightWaiting();

    if (TrySdUpgrade())
    {
        LightSuccess();
        HAL_Delay(300U);
        JumpToApplication();
    }

    start = HAL_GetTick();
    while ((uint32_t)(HAL_GetTick() - start) < 1500U)
    {
        if (OTA_ReceivePacket(&Packet, 100U) == HAL_OK &&
            Packet.command == OTA_CMD_HELLO)
        {
            OTA_HandleHello(&Packet);
            OTA_Run();
        }
    }

    if (AppIsValid())
        JumpToApplication();

    LightError();
    OTA_Run();
}
