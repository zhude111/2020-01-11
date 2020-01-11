#ifndef __USER_ZCL_H__
#define __USER_ZCL_H__
typedef struct
{
  uint32_t change;
  uint16_t nwkAddr;
  uint16_t clusterid;
  uint16_t attributeid;
  uint16_t min_t;
  uint16_t max_t;
  uint8_t  type;
  uint8_t  ep;
}CFG_REPORT_T;

extern uint16_t appZclBufferLen;
extern uint8_t appZclBuffer[EMBER_AF_MAXIMUM_SEND_PAYLOAD_LENGTH];
void zclSendCfgReport(CFG_REPORT_T Cfg_Report);
void zclBufferAddByte(uint8_t byte);
void zclBufferAddWord(uint16_t word);
void zclBufferSetup(uint8_t frameType, uint16_t clusterId, uint8_t commandId);
void processZclMsg(uint8 * pbuf );
void Period_SendList(void * pcontent);
void Period_free_SendList(void * pcontent);
void publishMqttZclCommand(EmberAfClusterCommand* cmd);
#endif 

