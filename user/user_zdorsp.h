#ifndef __USER_ZDO_RSP_H__
#define __USER_ZDO_RSP_H__

void DeviceAnnouceProcess(EmberNodeId nodeId,EmberEUI64 eui64);
bool AfPreZDOMessageReceivedCallbackzgm(EmberNodeId emberNodeId,
                                          EmberApsFrame* apsFrame,
                                          uint8_t* message,
                                          uint16_t length);

#endif 

