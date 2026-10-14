#include "MainTask.h"
#include "MesgTask.h"
#include "CommTask.h"
#include "CtrlTask.h"
#include "port_communicate.h"
#include "port_event.h"

extern Tx_HandleTypeDef Tx1;
extern Rx_HandleTypeDef Rx1;
extern Tx_HandleTypeDef Tx3;
extern Rx_HandleTypeDef Rx3;

extern Motor_Hoolle Motor_Hoolle1;
extern Motor_Card Card;
extern Switch_Valve Lock_Valve, Valve;

Event_Handle_t Mesg_event;
// 发送给NFC开锁消息
static uint8_t NFCUnlock_mesg[11] = {0xaa, 0x0f, 0x01, 0x01, 0x01, 0x01, 0x5c, 0x77, 0x08, 0x7f, 0x55};
void Mesg_Task(void)
{
    // 按键进入设置
    if (EventGroupCheckBits(&Mesg_event, MesgEvent_ButtonEnterSetting))
    {
        CommTransmitFillData(&Tx1, Board_to_Android, t_SettingButton, 0x03, 0x01);
        EventGroupClearBits(&Mesg_event, MesgEvent_ButtonEnterSetting);
    }
    // 开锁
    if (EventGroupCheckBits(&Mesg_event, MesgEvent_Unlock) == true)
    {
        Tx3.Transimit(&Tx3, NFCUnlock_mesg, sizeof(NFCUnlock_mesg)); // 向NFC发送开锁
        CommTransmitFillData(&Tx1, Board_to_Android, t_Unlock, 0x00, 0x00);                // 向安卓发送已开锁
        Valve_Start(&Lock_Valve,1);
        EventGroupClearBits(&Mesg_event, MesgEvent_Unlock);
    }
    // 投珠
    if (EventGroupCheckBits(&Mesg_event, MesgEvent_HoolleInput) == true)
    {
        CommTransmitFillData(&Tx1, Board_to_Android, t_HoolleInput, 0x00, 0x00);
        EventGroupClearBits(&Mesg_event, MesgEvent_HoolleInput);
    }
    // 投币
    if (EventGroupCheckBits(&Mesg_event, MesgEvent_CoinInput) == true)
    {
        CommTransmitFillData(&Tx1, Board_to_Android, t_Coin, 0x00, 0x00);
        EventGroupClearBits(&Mesg_event, MesgEvent_CoinInput);
    }
#ifndef BEIJING_NXH_VERSION
    // 吐珠超时
    if (EventGroupCheckBits(&Mesg_event, MesgEvent_HoolleOutputTimeout))
    {
        CommTransmitFillData(&Tx1, Board_to_Android, t_HoolleTimeout, Motor_Hoolle1.Hoolle_num, 0x00);
        EventGroupClearBits(&Mesg_event, MesgEvent_HoolleOutputTimeout);
    }
#endif
    // 吐卡超时
    if (EventGroupCheckBits(&Mesg_event, MesgEvent_CardOutputTimeout))
    {
        CommTransmitFillData(&Tx1, Board_to_Android, t_CardTimeout, Card.Card_num, 0x00);
        EventGroupClearBits(&Mesg_event, MesgEvent_CardOutputTimeout);
    }
    // 剩余珠子数
    if (EventGroupCheckBits(&Mesg_event, MesgEvent_RemainingHoolle) == true)
    {
        CommTransmitFillData(&Tx1, Board_to_Android, t_Remaining, Motor_Hoolle1.Hoolle_num, 0x00);
        EventGroupClearBits(&Mesg_event, MesgEvent_RemainingHoolle);
    }
    // NFC进入设置
    if (EventGroupCheckBits(&Mesg_event, MesgEvent_NFCEnterSetting) == true)
    {
        // 已废弃的安卓上报停用，保留NFC事件处理。
        EventGroupClearBits(&Mesg_event, MesgEvent_NFCEnterSetting);
    }
    // 吐卡一次
    if (EventGroupCheckBits(&Mesg_event, MesgEvent_CardOutputOnce) == true)
    {
        CommTransmitFillData(&Tx1, Board_to_Android, t_CardOnce, 0x00, 0x00);
        EventGroupClearBits(&Mesg_event, MesgEvent_CardOutputOnce);
    }
    // 吐卡完成
    if (EventGroupCheckBits(&Mesg_event, MesgEvent_CardOutputFinish) == true)
    {
        CommTransmitFillData(&Tx1, Board_to_Android, t_CardFinish, 0x00, 0x00);
        EventGroupClearBits(&Mesg_event, MesgEvent_CardOutputFinish);
    }
}