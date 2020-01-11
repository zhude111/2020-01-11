#include "user_include.h"

/***************************************************************************************************
 * @fn      Pack_MacsnRsp
 * @brief  直接推送不放入链表，应答消息,消息丢失也没关系，7688还会继续重新发送
 ***************************************************************************************************/
void Pack_MacsnRsp(uint8 Sn_L,uint8 Sn_H)
{
  uint8  pMsg[6] ={0};
     pMsg[P_HEAD]=  0xfe;
     pMsg[P_LEN] =  0x06;
     pMsg[P_SDIR] = G_M7688;
     pMsg[P_SN_L] = Sn_L;
     pMsg[P_SN_H] = Sn_H;
     pMsg[5] = GenericUartCalcFCS(pMsg,5);
     if(HalUARTWrite(pMsg,6)!=6)
     {
      // emberAfCorePrintln("error : Pack_MacsnRsp fail !"); 
     }
     procTx();
}
/***************************************************************************************************
 * @fn      ProcessRxList
 * @brief  处理接收链表
 ***************************************************************************************************/
void ProcessRxList(void)
{
   ListElement* currentElement = NULL;
   uint8* pbuf=NULL;
   uint8 cmd_m=0;
   ListElement* Element = NULL; 
   
   while(currentElement=ListPopFront(uartRxList))
   {
        pbuf=(uint8*)currentElement->content;
        
      //  emberAfCorePrint("uart_rx 1: ");
     //   printf_buff(pbuf,pbuf[1]);
        
        switch(pbuf[P_SDIR])
          {
              case  G_EFR32: //应答包
                          Element=FindListElementAndDelete(uartSendList,&pbuf[P_SDIR],3,P_SDIR);
                          if(Element)
                          {
                            osal_mem_free(Element->content); 
                            osal_mem_free(Element);  
                          }
                      break;
                      
              case  G_M7688:  
                          cmd_m=pbuf[P_CMD_M];
                          Pack_MacsnRsp(pbuf[P_SN_L],pbuf[P_SN_H]);//先回复数据，表示命令已经收到
                          if(MSG_SYS==cmd_m)
                          {
                                processSysMsg(pbuf);
                          }
                          else if(MSG_ZDO==cmd_m)
                          {
                                processZdoMsg(pbuf);
                          }
                          else if(MSG_ZCL==cmd_m)
                          {
                                processZclMsg(pbuf); 
                           }
                  
                      break;

          }
           osal_mem_free(currentElement->content); 
           osal_mem_free(currentElement); 
           halResetWatchdog(); 
   } 
}
/***************************************************************************************************
 * @fn      AfJoinCallback
 * @brief  设备离开
 ***************************************************************************************************/
void AfJoinCallback(EmberNodeId newNodeId,EmberEUI64 newNodeEui64,EmberDeviceUpdate status)
{
   switch (status) 
    {   
           case  EMBER_STANDARD_SECURITY_SECURED_REJOIN:
                 break;                 
            case EMBER_STANDARD_SECURITY_UNSECURED_JOIN:
                 break;
            case EMBER_DEVICE_LEFT:
                 emberAfCorePrintln("DEVICE_LEFT= %02x", newNodeId);
	         RspDeviceLeave(newNodeId,newNodeEui64);
                 break;
                
          default:
               
                         break;
       }
}
/***************************************************************************************************
 * @fn      RspDeviceLeave
 * @brief  设备离开
 ***************************************************************************************************/
void RspDeviceLeave(EmberNodeId nodeId,EmberEUI64 eui64)
{
     uint8 * pbuf=NULL;
     uint8 len=0x13;
     uint16 mac_sn=0;
     ListElement* currentElement=NULL; 
      
     pbuf= osal_mem_alloc(len); 
    if(pbuf) 
     {
         pbuf[P_HEAD]=0xfe;
         pbuf[P_LEN]=len;
         pbuf[P_SDIR]=G_EFR32;
         mac_sn=GetNextMacSN();
         pbuf[P_SN_L]=LO_UINT16(mac_sn);
         pbuf[P_SN_H]=HI_UINT16(mac_sn);
         pbuf[P_CMD_M]=0x00;
         pbuf[P_CMD_S]=0x08;
         pbuf[P_SN]=1;
         pbuf[P_SN+1]=LO_UINT16(nodeId);
         pbuf[P_SN+2]=HI_UINT16(nodeId);
         memcpy(&pbuf[P_SN+3],eui64,8);
         pbuf[len-1]=GenericUartCalcFCS(pbuf,len-1);
         HalUARTWrite(pbuf,len);
         procTx(); 
         currentElement=ListPushBack(uartSendList,pbuf,Resend_TIMES,Resend_TIMEOUT);
         if(currentElement==NULL)
          {
            emberAfCorePrintln("error: 0 RspDeviceLeave"); 
            osal_mem_free(pbuf);
          }
    }
      else
    {
      emberAfCorePrintln("error: 1 RspDeviceLeave"); 
    }
}
/***************************************************************************************************
 * @fn      RspIeeeAddr
 * @brief   查询IEEE地址 
 ***************************************************************************************************/
void RspIeeeAddr(void)
{
     uint8 * pbuf=NULL;
     uint8 len=0x13;
     uint16 mac_sn=0;
     ListElement* currentElement=NULL; 
     EmberNodeId nodeId;
     
     pbuf= osal_mem_alloc(len); 
    if(pbuf) 
     {
         pbuf[P_HEAD]=0xfe;
         pbuf[P_LEN]=len;
         pbuf[P_SDIR]=G_EFR32;
         mac_sn=GetNextMacSN();
         pbuf[P_SN_L]=LO_UINT16(mac_sn);
         pbuf[P_SN_H]=HI_UINT16(mac_sn);
         pbuf[P_CMD_M]=0x00;
         pbuf[P_CMD_S]=0x03;
         pbuf[P_SN]=0x03;
         nodeId= emberAfGetNodeId();  
         pbuf[P_SN+1]=LO_UINT16(nodeId);
         pbuf[P_SN+2]=HI_UINT16(nodeId);
         memcpy(&pbuf[P_SN+3],emberGetEui64(),8);
         pbuf[len-1]=GenericUartCalcFCS(pbuf,len-1);
         HalUARTWrite(pbuf,len);
         procTx(); 
         currentElement=ListPushBack(uartSendList,pbuf,Resend_TIMES,Resend_TIMEOUT);
         if(currentElement==NULL)
          {
            emberAfCorePrintln("error: 0 RspIeeeAddr"); 
            osal_mem_free(pbuf);
          }
    }
    else
    {
      emberAfCorePrintln("error: 1 RspIeeeAddr"); 
    }
}

/***************************************************************************************************
 * @fn      RspFactory
 * @brief   恢复出厂设置
 ***************************************************************************************************/
void RspFactory(void)
{
   EmberEUI64 nullEui64 = { 0, 0, 0, 0, 0, 0, 0, 0 }; 
   emberLeaveRequest(0XFFFC,nullEui64,0,EMBER_APS_OPTION_RETRY);
   emberLeaveNetwork();
}
/***************************************************************************************************
 * @fn      customEventFunction2
 * @brief   //创建网络之后打开网络
 ***************************************************************************************************/
void customEventFunction2(void)
{

    emberEventControlSetInactive(customEventControl2); 
    emberAfCorePrintln("sys: OpenNetwork"); 
    OpenNetwork(3000);       
}
/***************************************************************************************************
 * @fn      processSysMsg
 * @brief   处理系统消息
 ***************************************************************************************************/
void processSysMsg( uint8 * pbuf )
{
    uint8 cmd_s=pbuf[P_CMD_S];
    uint16 timer=0; 
      
    switch(cmd_s)
    {
     case  OPEN_NWK_CMD:
            timer = BUILD_UINT16(pbuf[P_SN+1], pbuf[P_SN+2]);
            emberAfCorePrintln("sys: open_nwk_time =%d ",timer); 
           if(timer==0)
           {
            emberAfPluginNetworkCreatorSecurityCloseNetwork(); 
           }
           else
           {
               timer=3000;  
               if (emberAfNetworkState() != EMBER_JOINED_NETWORK)
               { 
                  emberAfCorePrintln("sys: Creator Network ！"); 
                  emberAfPluginNetworkCreatorStart(1);
                  emberEventControlSetDelayMS(customEventControl2,5000);
                }
               else
               {
                  OpenNetwork(timer);  
               }
           }
          
           break;
          
     case GET_IEEE_CMD:
              RspIeeeAddr();
          break;
          
    case  RECOVER_FACTORY_CMD:
           emberAfCorePrintln("sys: Rsp To Factory ！"); 
           RspFactory();
           break;
             
    }
}



