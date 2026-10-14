#ifndef __COMM_TASK_H__
#define __COMM_TASK_H__

#include "stdint.h"
#include "port_communicate.h"
#include "app_list.h"

/*
 * 武松新协议：14字节通信帧
 * 与弹界通信层保持一致，不兼容旧7字节协议。
 */
typedef struct
{
    uint8_t Head;       // 0xAA
    uint8_t RetryID;    // 重传ID
    uint8_t ID;         // 唯一ID

    uint8_t Code1;      // 方向
    uint8_t Code2;      // 功能码

    uint8_t Data1;
    uint8_t Data2;
    uint8_t Data3;
    uint8_t Data4;

    uint8_t ACK;
    uint8_t ExpandCode;

    uint8_t CRC16_H;
    uint8_t CRC16_L;

    uint8_t Tail;       // 0x55
} Mesg_TypeDef;

/* 通信方向 */
#define Board_to_Android 0x00U
#define Android_to_Board 0x01U

/* 安卓->主板功能码 */
#define r_HoolleOutput       0x01U // 吐珠
#define r_CardOutput         0x02U // 出卡
#define r_ValveOutput        0x03U // 挡珠电磁阀
#define r_LightBrightness    0x04U // 亮度
#define r_SceneChange        0x07U // 场景
#define r_ClearOutput        0x0BU // 清珠
#define r_DeviceStop         0xFFU // 停止所有输出

/* 主板->安卓功能码 */
#define t_Coin               0x02U
#define t_Button             0x03U
#define t_Remaining          0x05U
#define t_HoolleFinish       0x13U

void CommInit(void);
void CommTask(void);

void CommTransmitFillData(Tx_HandleTypeDef *Tx,
                          uint8_t code1,
                          uint8_t code2,
                          uint32_t data);

#endif
