#ifndef __DEVICE_JOIN_H__
#define __DEVICE_JOIN_H__

#define  device_ok             0
#define  endpint_rep           1
#define  simple_rep            2
#define  read_eeee             3
#define  read_zone_type        4
/*============================== ===========================================
Frame type :Bits: 0-1====00 Command is global ; 01Command is specific 
Manufacturer   :Bits: 2 ===0 no mfgid ;  1: have mfgid
Direction :Bits:3 === 0:c->s  ;1: s->c
Disable  Default Response  :Bits:4   ====1 dis rsp  ,0 need rsp
==============================================================================*/
extern EmberApsFrame globalApsFrame;

#define Pose_IEEE    0
#define Pose_TYPE    8
#define Pose_NWK     12
#define Pose_CEP     14
#define Pose_SEP     15
#define Pose_EEP     16
#define Pose_ESTEP   17

typedef struct _Device_Join_T {
  uint8_t   IeeeAddr[8];
  uint32    DevType;
  uint16    NwkAddr;
  uint8     EndPoint;
  uint8     EPstartIndex;
  uint8     EPendIndex;
  uint8     Step;
} Device_Join_T;

#define SEND_JOIN_REPEAT 3

EmberStatus emberLeaveRequest(EmberNodeId target,
                              EmberEUI64 deviceAddress,
                              uint8_t leaveRequestFlags,
                              EmberApsOption options);
int DeviceHasJoin(Device_Join_T * content);
void ProcessAttributeBasicRsp(EmberAfClusterCommand *cmd,uint32 attrValue); 
void DeviceAnnouceProcess(EmberNodeId nodeId,EmberEUI64 eui64);
void  ReadDeviceDevType(uint16  NwkAddr,uint16 attrID);
void ProcessAttributeRsp(EmberAfClusterCommand *cmd);
void UartRecieveData(void);
void zdoActiveEndpointsRequest(uint16 nwkaddr);
void zdoSimpleDescriptorRequest(uint16 nwkaddr);
void  ReadIasDeviceDevType(uint16  NwkAddr,uint8 Endpoint);
void ProcessAttributeZoneidRsp(EmberAfClusterCommand *cmd,uint32 attrValue);
void findanddelete(uint16 nwkaddr);
void Period_deviceJoinList(void * pcontent);
void Period_deviceJoinList_leave(void * pcontent);
void BingRequest(EmberNodeId target,EmberEUI64 source,uint8_t sourceEndpoint,uint16_t clusterId);
void ZdoActiveEndpointsResponse(EmberNodeId emberNodeId,
                                                  EmberApsFrame* apsFrame,
                                                  uint8_t* message,
                                                  uint16_t length);
void ZdoSimpleDescriptorResponse(EmberNodeId emberNodeId,
                                                  EmberApsFrame* apsFrame,
                                                  uint8_t* message,
                                                  uint16_t length);
#endif 

