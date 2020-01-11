#ifndef __USER_PROC_RX_H__
#define __USER_PROC_RX_H__

#define MSG_SYS                 0x00
#define MSG_ZDO                 0x01
#define MSG_ZCL                 0x02
//sys cmd
#define   CREATE_NETWORK_CMD        0X01
#define   QUERY_NETWORK_PARA_CMD    0X02  
#define   GET_IEEE_CMD              0X03
#define   RECOVER_FACTORY_CMD       0X04
#define   RESET_CMD                 0X05
#define   OPEN_NWK_CMD              0X06
#define   DEVICE_JOIN_CMD           0X07
#define   DEVICE_LEAVE_CMD          0X08
#define   DEVICE_OTA_CMD            0X09
//zdo cmd
#define    ACTIVE_REQ_CMD                       0X01
#define    SIMPLE_REQ_CMD                       0X02
#define    NETWORK_ADDRESS_REQ_CMD              0X03
#define    IEEE_ADDRESS_REQ_CMD                 0X04
#define    LEAVE_REQ_CMD                        0X05
#define    BIND_REQ_CMD                         0X06
#define    UNBIND_REQ_CMD                       0X07
#define    BIND_TABLE_REQ_CMD                   0X08
#define    MATCH_REQ_CMD                        0X09
#define    NODE_REQ_CMD                         0X0A

EmberStatus OpenNetwork(uint16_t OpenNetworktime);
EmberStatus emberAfPluginNetworkCreatorSecurityOpenNetwork(void);
EmberStatus emberAfPluginNetworkCreatorStart(bool centralizedNetwork);
void AfJoinCallback(EmberNodeId newNodeId,EmberEUI64 newNodeEui64,EmberDeviceUpdate status);
void RspDeviceLeave(EmberNodeId nodeId,EmberEUI64 eui64);
EmberStatus emberAfPluginNetworkCreatorSecurityCloseNetwork(void);
void processSysMsg( uint8 * pbuf );
void processZdoMsg( uint8 * pbuf );
void RspLeaveFail(uint8 * buf);
#endif 

