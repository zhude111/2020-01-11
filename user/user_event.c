#include "user_include.h"

EmberEventControl customEventControl; 
EmberEventControl customEventControl1; 
EmberEventControl customEventControl2; 
EmberEventControl customEventControl3;

void customEventFunction(void)
{
   emberEventControlSetInactive(customEventControl);
   
   Period_Process_List(deviceJoinList,Period_deviceJoinList,Period_deviceJoinList_leave);
     
   emberEventControlSetDelayMS(customEventControl,3000);
}

void customEventFunction1(void)
{
   emberEventControlSetInactive(customEventControl1);  
   Period_Process_List(uartSendList,Period_SendList,LossPacketNotice);
   emberEventControlSetDelayMS(customEventControl1,2000);
}

void customEventFunction3(void)
{
   emberEventControlSetInactive(customEventControl3);  
   //Period_Process_List(uartSendList,Period_SendList,LossPacketNotice);
   emberEventControlSetDelayMS(customEventControl3,2000); 
}

/*****************************
 * @fn      Period_SendList
 * @brief   防止串口丢包
********************************/
void Period_SendList(void * pcontent)
{
  uint8  *pMsg=(uint8 *)pcontent;
     if(pcontent==NULL) 
     {
        return ;
      }
     HalUARTWrite(pMsg,pMsg[P_LEN]); //写入到发送队列
     procTx();
}
/*****************************
 * @fn      LossPacketNotice
 * @brief   丢包通知
********************************/
void LossPacketNotice(void * pcontent)
{
  uint8  *pMsg=(uint8 *)pcontent;
  uint16 count=0;
     if(pcontent==NULL) 
     {
        return ;
      }
 emberAfCorePrint("Eorror:LossPacket  ");
 for(count=0;count<pMsg[1];count++) 
  {
    emberAfCorePrint(" %0x",pMsg[count]);
    }
 emberAfCorePrintln("  ");   
    
}
