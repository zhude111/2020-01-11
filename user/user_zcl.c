#include "user_include.h"

/*****************************
 * @fn      processZclMsg
 * @brief   处理ZCL命令
********************************/
#define FIX_MEM  1
void processZclMsg(uint8 * pbuf )
{
   EmberStatus Status=0;
   uint16_t  len=0;
   uint16_t zclbuflen=0;
   uint16_t NwkAddr=BUILD_UINT16(pbuf[P_NWKL], pbuf[P_NWKH]);
#if(FIX_MEM==1)
   uint8_t  zclbuf[100]={0};
#else 
   uint8_t  *zclbuf=NULL;
#endif 
   
        globalApsFrame.sourceEndpoint = 1;
        globalApsFrame.destinationEndpoint = pbuf[P_EP];
	globalApsFrame.options =EMBER_AF_DEFAULT_APS_OPTIONS;
        globalApsFrame.clusterId = BUILD_UINT16(pbuf[P_CU_L], pbuf[P_CU_H]);
        
        if(pbuf[P_LEN]<19)  return ;//长度不够
        
         len=pbuf[P_LEN]-19;//只留下负载的长度
          
        if((pbuf[P_FC]&(1<<2))==0)
        {//没有MFGID  
            zclbuflen=3+len; 
        }
        else
        {//有MFGID
          zclbuflen=5+len;
        }
        
#if(FIX_MEM==0)
   zclbuf=osal_mem_alloc(zclbuflen);
        if(zclbuf==NULL)
        {
          Status=0xf0;
          emberAfCorePrintln("processZclMsg:osal_mem_alloc Fail !");
          goto  state_errot;
        }
#endif
             
        if((pbuf[P_FC]&(1<<2))==0)
        {//没有MFGID  
           zclbuf[0]=pbuf[P_FC];
           zclbuf[1]=pbuf[P_SN];
           zclbuf[2]=pbuf[P_ZCLCMD];
           if(len!=0)
           {
             MEMMOVE(&zclbuf[3],&pbuf[P_ZCLCMD+1],len);
           }
        }
        else
        {//有MFGID
           zclbuf[0]=pbuf[P_FC];
           zclbuf[1]=pbuf[P_SN];
           zclbuf[2]=pbuf[P_MFG_L];
           zclbuf[3]=pbuf[P_MFG_H];
           zclbuf[4]=pbuf[P_ZCLCMD];
           if(len!=0)
           {
             MEMMOVE(&zclbuf[5],&pbuf[P_ZCLCMD+1],len);
           }
        }
        
	
      switch(pbuf[P_AMODE])
      	{
      	case 1:
         
               Status= emberAfSendUnicast(EMBER_OUTGOING_DIRECT,
                                           NwkAddr,
                                           &globalApsFrame,
                                           zclbuflen,
                                           zclbuf);
					    break;
	case 2:	
		
		Status=emberAfSendMulticast( NwkAddr,
                                              &globalApsFrame,
                                               zclbuflen,
                                               zclbuf);
		       break;

	case 3:	
		Status=emberAfSendBroadcast( NwkAddr,
                                             &globalApsFrame,
                                             zclbuflen,
                                              zclbuf);
		               break;
      	} 
#if(FIX_MEM==0)
         osal_mem_free(zclbuf);
state_errot:
#endif
       if(Status!=0)
       {
        emberAfCorePrintln("Error：processZclMsg:Status=0x%0x",Status);
//        pRspMsg=(ZCL_RSP_ERR_T *)pData->asdu;  
//        pRspMsg->Len=sizeof(ZCL_RSP_ERR_T);
//        pRspMsg->Status=Status;
//        pRspMsg->S_Cmd =0x82;
//        pRspMsg->Crc=GenericUartCalcFCS((uint8_t *)pRspMsg,pRspMsg->Len-1);  
//        emberSerialWriteData(comPortUsart0,(uint8_t *)pRspMsg,pRspMsg->Len);
       }
}
/*****************************************************
 * @fn      publishMqttZclCommand
 * @brief   接收设备的ZCL命令，然后发给7688
********************************************************/
void publishMqttZclCommand(EmberAfClusterCommand* cmd)
{
     uint16_t  len=0;
     uint16_t zclbuflen=0;   
     uint8_t  *pMsg=NULL;
     uint16 mac_sn=0;
     ListElement* currentElement=NULL; 
     
//     if(cmd->apsFrame->clusterId==0x0000)
//     {
//        emberAfCorePrintln("clusterSpecific=0x%x",cmd->clusterSpecific);
//        emberAfCorePrintln("mfgSpecific=0x%x",cmd->mfgSpecific);
//        emberAfCorePrintln("commandId=0x%x",cmd->commandId);
//        emberAfCorePrintln("attrid=0x%02x",BUILD_UINT16(cmd->buffer[3], cmd->buffer[4]));
//     }
     
  //  emberAfCorePrintln("publishMqttZclCommand =0x%02x",cmd->commandId);
     if(cmd->clusterSpecific==0x00)
     {
        if(cmd->mfgSpecific ==0x00)
        {
           if(cmd->commandId==ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID)
           {
               if((BUILD_UINT16(cmd->buffer[3], cmd->buffer[4])==0xeeee)&&(cmd->apsFrame->clusterId==0x0000))
               { //读0x0000的0XEEEE的属性不上报
                 return ;
               }
             if((BUILD_UINT16(cmd->buffer[3], cmd->buffer[4])==0x0001)&&(cmd->apsFrame->clusterId==0x0500))
               { //读0x0500的0X0001的属性不上报 IAS设备
                 return ;
               }  
           }
        } 
     }
     else
     {//ias 的设备注册，不上报
       if(cmd->mfgSpecific==0x00)
        { 
         if(cmd->direction== ZCL_DIRECTION_SERVER_TO_CLIENT)
         {
          if((cmd->apsFrame->clusterId==0x0500)&&(cmd->commandId==0x01))
          { //zone enroll request
            return ; 
          }
         }
        }
     }
    //  emberAfCorePrintln("rx commandId=0x%x",cmd->commandId);
     zclbuflen= cmd->bufLen-cmd->payloadStartIndex; 
     len=19+zclbuflen;
     pMsg = osal_mem_alloc(len); 
     if(pMsg==NULL)
     {
         emberAfCorePrintln("AllocMem:#########fail#########");
           return ;
      }
     

         pMsg[P_HEAD]=0xfe;
         pMsg[P_LEN]=len;
         pMsg[P_SDIR]=G_EFR32;
         mac_sn=GetNextMacSN();
         pMsg[P_SN_L]=LO_UINT16(mac_sn);
         pMsg[P_SN_H]=HI_UINT16(mac_sn);
         pMsg[P_CMD_M]=0x02;
         pMsg[P_CMD_S]=0x01;
         pMsg[P_SN]=cmd->buffer[cmd->payloadStartIndex-2];
         pMsg[P_AMODE]=1;
         pMsg[P_NWKL]=cmd->source;
         pMsg[P_NWKH]=cmd->source>>8;
         pMsg[P_CU_L]=cmd->apsFrame->clusterId;
         pMsg[P_CU_H]=cmd->apsFrame->clusterId>>8;
         pMsg[P_EP]=cmd->apsFrame->sourceEndpoint; 
         pMsg[P_FC]=cmd->buffer[0];
         pMsg[P_MFG_L]=cmd->mfgCode;
         pMsg[P_MFG_H]=cmd->mfgCode>>8;
         pMsg[P_ZCLCMD]=cmd->commandId; 
          if(zclbuflen!=0)
          {
            memcpy(&pMsg[18],&cmd->buffer[cmd->payloadStartIndex],zclbuflen);
          } 
          pMsg[len-1]=GenericUartCalcFCS((uint8_t *)pMsg,len-1);
          len=HalUARTWrite(pMsg,pMsg[P_LEN]);
         currentElement=ListPushBack(uartSendList,pMsg,Resend_TIMES,Resend_TIMEOUT);
          if(currentElement==NULL)
          {//没空间了，把设备暂时踢出网络
            emberAfCorePrintln("error: publishMqttZclCommand ! "); 
            osal_mem_free(pMsg);
          }
  
  
}

void SetCfgReportTable( )
{
  CFG_REPORT_T Cfg_Report={0};
  
  Cfg_Report.clusterid=0x0702;
  Cfg_Report.attributeid=0x0000l;
  Cfg_Report.type=0x25;
  Cfg_Report.min_t=15;
  Cfg_Report.max_t=0xffff;
  Cfg_Report.change=100;
  Cfg_Report.ep=255;
  Cfg_Report.nwkAddr=0x111;
}

void zclSendCfgReport(CFG_REPORT_T Cfg_Report)
{ 
  zclBufferSetup(ZCL_GLOBAL_COMMAND | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER,
                 Cfg_Report.clusterid,
                 ZCL_CONFIGURE_REPORTING_COMMAND_ID);
  zclBufferAddByte(EMBER_ZCL_REPORTING_DIRECTION_REPORTED);
  zclBufferAddWord(Cfg_Report.attributeid);
  zclBufferAddByte(Cfg_Report.type);         
  zclBufferAddWord(Cfg_Report.min_t); 
  zclBufferAddWord( Cfg_Report.max_t); 
  if (emberAfGetAttributeAnalogOrDiscreteType(Cfg_Report.type)== EMBER_AF_DATA_TYPE_ANALOG) 
  {
    uint8_t dataSize = emberAfGetDataSize(Cfg_Report.type);   
    memcpy(appZclBuffer + appZclBufferLen, (uint8_t*)&Cfg_Report.change, dataSize);
    appZclBufferLen += dataSize;
  }
        globalApsFrame.sourceEndpoint = 1;
        globalApsFrame.destinationEndpoint = Cfg_Report.ep;
	globalApsFrame.options = EMBER_AF_DEFAULT_APS_OPTIONS;
        globalApsFrame.clusterId = Cfg_Report.clusterid; 

        emberAfSendUnicast(EMBER_OUTGOING_DIRECT,
                                Cfg_Report.nwkAddr,
                                &globalApsFrame,
                                appZclBufferLen,
                                appZclBuffer);  
  
}
