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

// 串口3 NFC消息帧结构体，保持原11字节CRC32协议
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

/* 通信方向 */
#define Board_to_Android 0x00U
#define Android_to_Board 0x01U

/* 安卓->主板功能码 */
#define r_HoolleOutput       0x01U // 吐珠
#define r_CardOutput         0x02U // 出卡
#define r_ValveOutput        0x03U // 挡珠电磁阀
#define r_LightBrightness    0x04U // 亮度
#define r_LightBeltBrightness 0x06U // 灯带亮度
#define r_SceneChange        0x07U // 场景
#define r_ChannelLight      0x09U // 中奖通道和灯
#define r_ClearOutput        0x0BU // 清珠
#define r_OutputRemaining   0x0CU // 吐出剩余物品
#define r_SaveSetting       0x0EU // 保存设置
#define r_HoleValveOutput   0x0FU // 洞内电磁阀
#define r_Unlock            0x10U // 开锁
#define r_ResumeSetting     0x11U // 恢复默认设置
#define r_ButtonLight       0x17U // 按键灯
#define r_SystemReset       0xF0U // 球盘重启
#define r_DeviceStop         0xFFU // 停止所有输出

/* 主板->安卓功能码 */
#define t_HoolleInput        0x01U
#define t_Coin               0x02U
#define t_Button             0x03U
#define t_SettingButton      0x04U
#define t_Remaining          0x05U
#define t_HoolleTimeout      0x07U
#define t_CardTimeout        0x08U
#define t_Unlock             0x0DU
#define t_Switch             0x0EU
#define t_Hole               0x12U
#define t_HoolleFinish       0x13U

#define t_CardOnce           0x14U
#define t_CardFinish         0x15U
#define t_Win                0x16U
#define t_Lose               0x17U
#define t_AntiShake          0x18U

void CommInit(void);
void CommTask(void);

void CommTransmitFillData(Tx_HandleTypeDef *Tx,
                          uint8_t code_1,
                          uint8_t code_2,
                          uint32_t data,
                          uint8_t expandCode);

#endif
