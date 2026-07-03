#ifndef __COMM_TASK_H__
#define __COMM_TASK_H__

#include "stdint.h"
#include "port_communicate.h"
#include "app_list.h"

/*
 * APP版本号编码：0xMMNNPPBB
 * MM=主版本，NN=次版本，PP=修订版本，BB=构建版本。
 * 当前版本：V1.0.0。
 */
#define WUSONG_APP_VERSION 0x01000000U

/*
 * 武松原安卓通信为固定7字节：
 * AA Code Data1 Data2 CRC16_H CRC16_L 55
 *
 * 新增系统命令0xF0：
 *   F0 00 00：查询APP版本，APP依次返回F1和F2两帧；
 *   F0 42 4F：写入RTC升级请求并复位进入Bootloader。
 */
#define WUSONG_SYSTEM_COMMAND 0xF0U
#define WUSONG_VERSION_HIGH_RESPONSE 0xF1U
#define WUSONG_VERSION_LOW_RESPONSE 0xF2U
#define WUSONG_OTA_KEY_HIGH 0x42U
#define WUSONG_OTA_KEY_LOW 0x4FU
#define OTA_REQUEST_MAGIC 0x424F5441U /* ASCII "BOTA" */

// 串口1 消息帧结构体
typedef struct
{
    uint8_t Head;
    uint8_t Code;
    uint8_t Data1;
    uint8_t Data2;
    uint8_t CRC16_H;
    uint8_t CRC16_L;
    uint8_t Tail;
} USART1_MesgTypeDef;

// 串口3 消息帧结构体
typedef struct
{
    uint8_t Head;
    uint8_t Code;
    uint8_t Data1;
    uint8_t Data2;
    uint8_t Data3;
    uint8_t Data4;
    uint8_t CRC32_1;
    uint8_t CRC32_2;
    uint8_t CRC32_3;
    uint8_t CRC32_4;
    uint8_t Tail;
} USART3_MesgTypeDef;

void CommInit(void);
void CommTask(void);
void CommTransmitFillData(Tx_HandleTypeDef *Tx, uint8_t code, uint8_t data1, uint8_t data2);

#endif
