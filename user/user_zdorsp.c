#include "user_include.h"

bool AfPreZDOMessageReceivedCallbackzgm(EmberNodeId emberNodeId,
                                          EmberApsFrame* apsFrame,
                                          uint8_t* message,
                                          uint16_t length)
{
   bool ishandled=false;
    
    switch (apsFrame->clusterId)
    {
     case ACTIVE_ENDPOINTS_RESPONSE:
          emberAfCorePrintln("ACTIVE_ENDPOINTS_RESPONSE...%02x",emberNodeId);
          ZdoActiveEndpointsResponse(emberNodeId, apsFrame, message, length);
         // publishMqttActiveEndpointsResponse(emberNodeId, apsFrame, message, length);
          //ishandled=true;
          break;
    case SIMPLE_DESCRIPTOR_RESPONSE:
          emberAfCorePrintln("SIMPLE_DESCRIPTOR_RESPONSE...%02x",emberNodeId);
          ZdoSimpleDescriptorResponse(emberNodeId,apsFrame, message,length);
         // publishMqttSimpleDescriptorResponse(emberNodeId, apsFrame, message, length);
          // ishandled=true;
           break;
           
    case  NODE_DESCRIPTOR_RESPONSE:
          emberAfCorePrintln("NODE_DESCRIPTOR_RESPONSE...%02x",emberNodeId);
          //publishMqttNodeDescriptorResponse(emberNodeId, apsFrame, message, length);
         //  ishandled=true;
   	   break;  
           
    case MATCH_DESCRIPTORS_RESPONSE:
         emberAfCorePrintln("MATCH_DESCRIPTORS_RESPONSE...%02x",emberNodeId);
	// publishMqttMatchDescriptorResponse(emberNodeId, apsFrame, message, length);
         // ishandled=true;
	   break;
 
    case END_DEVICE_ANNOUNCE:	//END_DEVICE_ANNOUNCE_IEEE_OFFSET
          emberAfCorePrintln("END_DEVICE_ANNOUNCE...%02x",emberNodeId);
          DeviceAnnouceProcess(emberNodeId,message + 3);
	  break;
          
    case PERMIT_JOINING_RESPONSE:
      
          break;
    case LEAVE_RESPONSE:
        // emberAfCorePrintln("LEAVE_RESPONSE...%02x",emberNodeId);
	//publishMqttLeaveEndpointsResponse(emberNodeId, apsFrame, message, length);
         //ishandled=true;
          break;
    case  UNBIND_RESPONSE:	
    case  BIND_RESPONSE:
          emberAfCorePrintln("UNBIND_RESPONSE/BIND_RESPONSE...%02x",emberNodeId);
         // publishMqttBindResponse(emberNodeId, apsFrame, message, length);
          // ishandled=true;
          break;   
    case BINDING_TABLE_RESPONSE:
          // publishMqttBindTableReponse(emberNodeId, apsFrame, message, length);
           // ishandled=true;
	     break;
    
    case NETWORK_ADDRESS_RESPONSE:
    case IEEE_ADDRESS_RESPONSE:
         emberAfCorePrintln("NETWORK_ADDRESS_RESPONSE/IEEE_ADDRESS_RESPONSE...%02x",emberNodeId);
	 //publishMqttIeeeDescriptorResponse(emberNodeId, apsFrame, message, length);
         // ishandled=true;
              break;
	  
    default:
              break;
           
    }
  
  return ishandled;
}
/***************************************************************************************************
 * @fn      RspLeaveFail
 * @brief   ∑¢ÀÕ¿ÎÕ¯÷∏¡Ó ß∞‹
 ***************************************************************************************************/
void RspLeaveFail(uint8 * buf)
{
     uint8 * pbuf=NULL;
     uint8 len=12;
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
         pbuf[P_CMD_M]=0x01;
         pbuf[P_CMD_S]=0x05;
         pbuf[P_SN]=buf[P_SN];
         pbuf[P_SN+1]=buf[P_SN+1];
         pbuf[P_SN+2]=buf[P_SN+2];
         pbuf[P_SN+3]=0x01;//◊¥Ã¨£¨ ß∞‹
         pbuf[len-1]=GenericUartCalcFCS(pbuf,len-1);
         HalUARTWrite(pbuf,len);
         procTx(); 
         currentElement=ListPushBack(uartSendList,pbuf,Resend_TIMES,Resend_TIMEOUT);
         if(currentElement==NULL)
          {
            emberAfCorePrintln("error: 0 RspLeaveFail"); 
            osal_mem_free(pbuf);
          }
    }
    else
    {
      emberAfCorePrintln("error: 1 RspLeaveFail"); 
    }
}
/***************************************************************************************************
 * @fn      SendLeaveCmd
 ***************************************************************************************************/
void  SendLeaveCmd(uint8 * pbuf)
{
     EmberStatus Status=0;
     EmberEUI64 nullEui64 = { 0, 0, 0, 0, 0, 0, 0, 0 };
     uint16 source=BUILD_UINT16(pbuf[P_SN+1], pbuf[P_SN+2]);   
     
     Status=emberLeaveRequest(source,nullEui64,0,0); 
     emberAfCorePrintln("emberLeaveRequestStatus= %d",Status); 
     
     if(Status!=0)
     {
       RspLeaveFail(pbuf);
     }
}                                       
/***************************************************************************************************
 * @fn      processZdoMsg
 * @brief   ¥¶¿Ìzdoœ˚œ¢
 ***************************************************************************************************/
void processZdoMsg( uint8 * pbuf )
{
    uint8 cmd_s=pbuf[P_CMD_S];
      
    switch(cmd_s)
    {
      case  LEAVE_REQ_CMD:
               SendLeaveCmd(pbuf);
            break;
            
      default:
        
            break;      
         
    }
}