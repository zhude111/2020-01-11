#include "user_include.h"
#include "zigbee-device-common.h"
//STEP 1:设备入网进来4rr
void DeviceAnnouceProcess(EmberNodeId nodeId,EmberEUI64 eui64)
{
  ListElement* currentElement=NULL;
  Device_Join_T * content=NULL;
  
         currentElement=FindListElement(deviceJoinList,eui64,8,Pose_IEEE);
  if(currentElement !=NULL)
      {
            content=currentElement->content;
            content->NwkAddr=nodeId;
      }
  else
      {
        content= osal_mem_alloc(sizeof(Device_Join_T));
            if(content==NULL)
            {
                emberAfCorePrintln("Error: step 1_0 DeviceAnnouce emberLeaveRequest  !!!!!");
                emberLeaveRequest(nodeId,eui64,0,0);
            }
            else
            {
                 osal_memcpy( content->IeeeAddr,eui64,8);
                 content->NwkAddr=nodeId;
                 content->DevType=0xffffffff;
                 content->Step=read_eeee;
                 content->EndPoint=0;
                 content->EPstartIndex=0;
                 content->EPendIndex=0;
                 currentElement=ListPushBack(deviceJoinList,content,4,4);
                if(currentElement ==NULL)
                {  //把设备u踢出网络
                     osal_mem_free(content);  
                     emberLeaveRequest(nodeId,eui64,0,0);
                     emberAfCorePrintln("Error: step 1_1 DeviceAnnouce emberLeaveRequest  !!!!!");
                }
                else
                {
                     currentElement->content=content;
                     content->Step=read_eeee;
                     currentElement->repeat_cnt=SEND_JOIN_REPEAT; //3次
                     currentElement->timeout=Resend_TIMEOUT;//4秒
                     ReadDeviceDevType(nodeId,0xeeee);//读取设备的属性
                }
            }
      }
  
}

////STEP 2: 读取设备的属性
void  ReadDeviceDevType(uint16  NwkAddr,uint16 attrID)
{
  uint8  Pbuf[5]={0};

    globalApsFrame.sourceEndpoint = 1;
    globalApsFrame.destinationEndpoint = 1;
    globalApsFrame.options = EMBER_AF_DEFAULT_APS_OPTIONS;
    globalApsFrame.clusterId = 0x0000;
    Pbuf[0]=  0x00;                //Fc;
    Pbuf[1]=  0x64;                //Sn;
    Pbuf[2]=  ZCL_READ_ATTRIBUTES_COMMAND_ID;        
    Pbuf[3]=  LO_UINT16(attrID);               
    Pbuf[4]=  HI_UINT16(attrID);
    emberAfSendUnicast(EMBER_OUTGOING_DIRECT,
                       NwkAddr,
                       &globalApsFrame,
                       sizeof(Pbuf),
                       Pbuf);
}
//STEP 3: 获取设备的属性0XEEEE
void ProcessAttributeRsp(EmberAfClusterCommand *cmd)
{
      uint16_t attrId;
      uint8_t stauts;
      uint8_t attrValue;
      EmberAfClusterId clusterId = cmd->apsFrame->clusterId;
      uint8_t zclCmd = cmd->commandId;
      uint8_t *message = cmd->buffer;
      uint16_t msgIndex = cmd->payloadStartIndex;
 
      if(zclCmd==ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID)
      {
             attrId=BUILD_UINT16(message[msgIndex],message[msgIndex+1]);  
             stauts=message[msgIndex+2]; 
        
              switch(clusterId)
              {
                  case ZCL_BASIC_CLUSTER_ID:
                           if(attrId==0xEEEE)
                           {
                                if(stauts==EMBER_ZCL_STATUS_SUCCESS)
                                {
                                   attrValue=message[msgIndex+4]; 
                                  // emberAfCorePrintln("step 3_0 0xEEEE attrValue=%d ",attrValue);
                                   ProcessAttributeBasicRsp(cmd,attrValue); 
                                }
                                else
                                { //读端点数
                                  zdoActiveEndpointsRequest(cmd->source);
                                  //emberAfCorePrintln("step 3_1 0xEEEE stauts =%0x ,read active ep! ",stauts); 
                                } 
                           }
                              break;
                   case ZCL_IAS_ZONE_CLUSTER_ID:
                           if(attrId==0x0001)  //zone type
                           {
                                if(stauts==EMBER_ZCL_STATUS_SUCCESS)
                                {
                                   attrValue=BUILD_UINT16(message[msgIndex+4],message[msgIndex+4]); 
                                  // emberAfCorePrintln("step 3_2 IAS_ZONE  attrValue=%02x ",attrValue);
                                   ProcessAttributeZoneidRsp(cmd,attrValue); 
                                }
                                else
                                { //把设备踢掉
                                   findanddelete(cmd->source);
                                  // emberAfCorePrintln("step 3_3 0xEEEE stauts =%0x ,read active ep! ",stauts); 
                                } 
                           }
                              break;
                  default:
                             break;
              }
      } 
}
//STEP 4: BASIC 0xeeee 属性
void ProcessAttributeBasicRsp(EmberAfClusterCommand *cmd,uint32 attrValue)
{
   ListElement* currentElement=NULL;
   Device_Join_T * content=NULL;
   EmberEUI64 nullEui64 = { 0, 0, 0, 0, 0, 0, 0, 0 };
  
   currentElement= FindListElementAndDelete(deviceJoinList,(uint8 *)&cmd->source,2,Pose_NWK);
      if(currentElement)
            {
                   content=( Device_Join_T *)currentElement->content; 
                   
                   content->DevType=0;
                   
                   switch(attrValue)
                   {
                           case 0:
                             content->DevType=(1L<<16);
                                   break;
                           case 1:
                             content->DevType=(2L<<16);
                                   break;
                           case 2:
                              content->DevType=(3L<<16);
                                   break;
                           case 3:
                              content->DevType=(4L<<16);
                                   break;    
                   }
                   
                   content->DevType |=0x0003;    //稍作修改
                   content->EndPoint=1;
                   content->EPstartIndex=1;
                   content->EPendIndex=1;
                   if(DeviceHasJoin(content)==0)
                   {
                   // BingRequest(content->NwkAddr,content->IeeeAddr,content->EPstartIndex,ZCL_ELECTRICAL_MEASUREMENT_CLUSTER_ID);
                   // BingRequest(content->NwkAddr,content->IeeeAddr,content->EPstartIndex,ZCL_SIMPLE_METERING_CLUSTER_ID);
                   }
                 
                   osal_mem_free(currentElement->content); 
                   osal_mem_free(currentElement);  
             }
      else
           {
            //逻辑错误
                  emberLeaveRequest(cmd->source,nullEui64,0,0);
                  emberAfCorePrintln("Error: STEP_4_device_no_find_List!");
           }
}
//STEP 5: BASIC 0xeeee 属性
int DeviceHasJoin(Device_Join_T * content)
{
     uint8 * pbuf=NULL;
     uint8 len=0;
     uint16 mac_sn=0;
     ListElement* currentElement=NULL; 
     
     emberAfCorePrintln("DeviceHasJoin  =0x%02x",content->NwkAddr);
     
     pbuf= osal_mem_alloc(25);
    if(pbuf) 
    {
         pbuf[P_HEAD]=0xfe;
         pbuf[P_LEN]=25;
         pbuf[P_SDIR]=G_EFR32;
         mac_sn=GetNextMacSN();
         pbuf[P_SN_L]=LO_UINT16(mac_sn);
         pbuf[P_SN_H]=HI_UINT16(mac_sn);
         pbuf[P_CMD_M]=0x00;
         pbuf[P_CMD_S]=0x07;
         pbuf[P_SN]=20;
         pbuf[P_SN+1]=LO_UINT16(content->NwkAddr);
         pbuf[P_SN+2]=HI_UINT16(content->NwkAddr);
         memcpy(&pbuf[P_SN+3],content->IeeeAddr,8);
         memcpy(&pbuf[P_SN+11],(uint8 *)&content->DevType,4);
         pbuf[P_SN+15]=content->EndPoint;
         pbuf[P_SN+16]=content->EPstartIndex;
         pbuf[P_SN+17]=GenericUartCalcFCS(pbuf,pbuf[P_LEN]-1);
         //把数据推送到发送队列，再放入链表
         len=HalUARTWrite(pbuf,pbuf[P_LEN]);
#if 1
          currentElement=ListPushBack(uartSendList,pbuf,Resend_TIMES,Resend_TIMEOUT);
          if(currentElement==NULL)
          {//没空间了，把设备暂时踢出网络
            emberAfCorePrint("error: step 5_0  emberLeaveRequest 0x%02x ",content->NwkAddr); 
            emberLeaveRequest(content->NwkAddr,content->IeeeAddr,0,0);
            osal_mem_free(pbuf);
            return 1;
          }
#endif
    }
    else
    {//把设备踢出去
          emberAfCorePrint("error: step 5_2  emberLeaveRequest 0x%02x ",content->NwkAddr); 
          emberLeaveRequest(content->NwkAddr,content->IeeeAddr,0,0); 
          return 1;
    } 
      return 0;
}

//STEP3  , 其它厂家标准设备
void zdoActiveEndpointsRequest(uint16 nwkaddr)
{
  ListElement* currentElement=NULL;
  Device_Join_T * content=NULL;
  uint8 IeeeAddr[8]={0};
  
      currentElement=FindListElement(deviceJoinList,(uint8*)&nwkaddr,2,Pose_NWK);
  if(currentElement !=NULL)
      {
            content=currentElement->content;
            content->Step=endpint_rep;
            currentElement->repeat_cnt=3; //3次
            currentElement->timeout=4;//4秒
            emberActiveEndpointsRequest(nwkaddr,0);
            emberAfCorePrintln("Ok:step 3_1 zdoActiveEndpointsRequest "); 
      }
  else
     {
          emberAfCorePrintln("Eroor:step 3_1 zdoActiveEndpointsRequest "); 
          emberLeaveRequest(nwkaddr,IeeeAddr,0,0); 
     } 
}
//========================================================
void ZdoActiveEndpointsResponse(EmberNodeId emberNodeId,
                                                  EmberApsFrame* apsFrame,
                                                  uint8_t* message,
                                                  uint16_t length)
 {
   ListElement* currentElement=NULL;
   Device_Join_T * content=NULL;
   uint8 IeeeAddr[8]={0};
  
    
          currentElement=FindListElement(deviceJoinList,(uint8*)&emberNodeId,2,Pose_NWK);
      if(currentElement !=NULL)
          {
                content=currentElement->content;
                content->Step=simple_rep;
                content->EndPoint=message[4];
                content->EPstartIndex=message[5];
                content->EPendIndex=message[4+content->EndPoint];
                currentElement->repeat_cnt=3; //3次
                currentElement->timeout=4;//4秒
                zdoSimpleDescriptorRequest(emberNodeId);
                emberAfCorePrintln("Ok:step 3_2  EP_count= %d,st=%d ",message[4],message[5]); 
          }
      else
         {
              emberAfCorePrintln("Eroor:step 3_2 ZdoActiveEndpointsResponse "); 
              emberLeaveRequest(emberNodeId,IeeeAddr,0,0); 
         }     
 }
//STEP4  , 其它厂家标准设备
void zdoSimpleDescriptorRequest(uint16 nwkaddr)
{
  ListElement* currentElement=NULL;
  Device_Join_T * content=NULL;
  uint8 IeeeAddr[8]={0};
  
      currentElement=FindListElement(deviceJoinList,(uint8*)&nwkaddr,2,Pose_NWK);
  if(currentElement !=NULL)
      {
            content=currentElement->content;
            content->Step=endpint_rep;
            currentElement->repeat_cnt=3; //3次
            currentElement->timeout=4;//4秒
            emberSimpleDescriptorRequest(nwkaddr,content->EPstartIndex,0);
            emberAfCorePrintln("Ok:step 4_1 zdoSimpleDescriptorRequest "); 
      }
  else
     {
          emberAfCorePrintln("Eroor:step 4_1 zdoSimpleDescriptorRequest "); 
          emberLeaveRequest(nwkaddr,IeeeAddr,0,0); 
     } 
}
//========================================================
void ZdoSimpleDescriptorResponse(EmberNodeId emberNodeId,
                                                  EmberApsFrame* apsFrame,
                                                  uint8_t* message,
                                                  uint16_t length)
 {
   ListElement* currentElement=NULL;
   Device_Join_T * content=NULL;
   uint8 IeeeAddr[8]={0};
      
          currentElement=FindListElement(deviceJoinList,(uint8*)&emberNodeId,2,Pose_NWK);
      if(currentElement !=NULL)
          {
                content=currentElement->content;
                content->Step=simple_rep;
                currentElement->repeat_cnt=3; 
                currentElement->timeout=4;
                content->DevType=BUILD_UINT16(message[8], message[9]);
                if(content->DevType==0x0000)
                 {  //三键开关
                  content->DevType=(1L<<16); 
                  //增加绑定三个按键
                  BingRequest(content->NwkAddr,content->IeeeAddr,0x01,ZCL_ON_OFF_CLUSTER_ID);
                  BingRequest(content->NwkAddr,content->IeeeAddr,0x02,ZCL_ON_OFF_CLUSTER_ID);
                  BingRequest(content->NwkAddr,content->IeeeAddr,0x03,ZCL_ON_OFF_CLUSTER_ID);
                 }
                
                if(content->DevType==0x0051)  //ZCL_HA_DEVICEID_SMART_PLUG  
                {
                  BingRequest(content->NwkAddr,content->IeeeAddr,0xff,ZCL_ON_OFF_CLUSTER_ID);
                  BingRequest(content->NwkAddr,content->IeeeAddr,0xff,ZCL_SIMPLE_METERING_CLUSTER_ID);
                   //zclSendCfgReport(CFG_REPORT_T Cfg_Report);  //配置报告表
                }
                emberAfCorePrintln("Ok:step 4_2  DevType= 0x%02x", content->DevType);
                if((content->DevType>0x400)&&(content->DevType<0x404))
                {//ias设备
                  content->Step=read_zone_type;
                  ReadIasDeviceDevType(emberNodeId,content->EPstartIndex);  
                }
                else
                {
                  DeviceHasJoin(content);
                  emberAfCorePrintln("STEP 4 ok SimpleDescriptorResponse");  
                  ListRemoveElement(deviceJoinList,currentElement); 
                  osal_mem_free(currentElement->content); 
                  osal_mem_free(currentElement);  
                 }
          }
      else
         {
              emberAfCorePrintln("Eroor:step 4_2 ZdoSimpleDescriptorResponse "); 
              emberLeaveRequest(emberNodeId,IeeeAddr,0,0); 
         }     
 }
////STEP 5: 读取Ias设备的属性
void  ReadIasDeviceDevType(uint16  NwkAddr,uint8 Endpoint)
{
  uint8  Pbuf[5]={0};

    globalApsFrame.sourceEndpoint = 1;
    globalApsFrame.destinationEndpoint = Endpoint;
    globalApsFrame.options =EMBER_AF_DEFAULT_APS_OPTIONS;
    globalApsFrame.clusterId = 0x0500;
    Pbuf[0]=  0x00;                //Fc;
    Pbuf[1]=  0x64;                //Sn;
    Pbuf[2]=  ZCL_READ_ATTRIBUTES_COMMAND_ID;        
    Pbuf[3]=  0x01;               
    Pbuf[4]=  0x00;  //attribute id
    emberAfSendUnicast(EMBER_OUTGOING_DIRECT,
                       NwkAddr,
                       &globalApsFrame,
                       sizeof(Pbuf),
                       Pbuf);
}
//STEP 5: zoneid 0x0500  0x0001属性
void ProcessAttributeZoneidRsp(EmberAfClusterCommand *cmd,uint32 attrValue)
{
   ListElement* currentElement=NULL;
   Device_Join_T * content=NULL;
    EmberEUI64 nullEui64 = { 0, 0, 0, 0, 0, 0, 0, 0 };
   emberAfCorePrintln("Go into: STEP 5 ZoneidRsp ok");
   currentElement= FindListElementAndDelete(deviceJoinList,(uint8 *)&cmd->source,2,Pose_NWK);
      if(currentElement)
            {
                   content=( Device_Join_T *)currentElement->content;  
                   content->DevType|=(attrValue<<16);    
                   DeviceHasJoin(content);
                   emberAfCorePrintln("STEP 5 ok zoneid DevType=0x%04x",content->DevType);
                   osal_mem_free(currentElement->content); 
                   osal_mem_free(currentElement);  
             }
      else
           {
            //逻辑错误
                  emberLeaveRequest(cmd->source,nullEui64,0,0);
                  emberAfCorePrintln("Error: STEP 5 !!!!!");
          }
}

//找到对应的结点，删除他
void findanddelete(uint16 nwkaddr)
{
   ListElement* currentElement=NULL;
   EmberEUI64 nullEui64 = { 0, 0, 0, 0, 0, 0, 0, 0 };
   emberAfCorePrintln("Go into: STEP 6 findanddelete ");
   currentElement= FindListElementAndDelete(deviceJoinList,(uint8 *)&nwkaddr,2,Pose_NWK);
      if(currentElement)
            {
                   emberLeaveRequest(nwkaddr,nullEui64,0,0);
                   osal_mem_free(currentElement->content); 
                   osal_mem_free(currentElement);  
             }
      else
           {
            //逻辑错误
                  emberLeaveRequest(nwkaddr,nullEui64,0,0);
                  emberAfCorePrintln("Error: findanddelete 0x%02x",nwkaddr);
          }
}

void Period_deviceJoinList(void * pcontent)
{
 Device_Join_T * content=(Device_Join_T *)pcontent;
 
   if(pcontent==NULL) 
   {
       emberAfCorePrintln("Period_deviceJoinList pcontent==NULL!");
      return ;
   }
 
   switch(content->Step)
   {
     case endpint_rep:
            emberActiveEndpointsRequest(content->NwkAddr,0);
            break;
            
     case simple_rep:
          emberSimpleDescriptorRequest(content->NwkAddr,content->EPstartIndex,0);
            break;
            
     case read_eeee:
           ReadDeviceDevType(content->NwkAddr,0xeeee);
            break;
            
     case read_zone_type:
            ReadIasDeviceDevType(content->NwkAddr,content->EPstartIndex);
            break;
     
   default:
        emberAfCorePrintln("content->Step==%x !",content->Step);
       break;
     
   } 
}

void Period_deviceJoinList_leave(void * pcontent)
{
 Device_Join_T * content=(Device_Join_T *)pcontent;
 EmberEUI64 nullEui64 = { 0, 0, 0, 0, 0, 0, 0, 0 };
   if(pcontent==NULL) 
   {
       emberAfCorePrintln("Period_deviceJoinList pcontent==NULL!");
      return ;
   }

    emberLeaveRequest(content->NwkAddr,nullEui64,0,0);
  
}
                 
                 
         
void BingRequest(EmberNodeId target,EmberEUI64 source,uint8_t sourceEndpoint,uint16_t clusterId)
{
  EmberEUI64 destination={0};  
                 
  emberAfGetEui64(destination);                       
  emberBindRequest(target,source,sourceEndpoint,clusterId,UNICAST_BINDING,destination,0,1,0);              
                           
}
