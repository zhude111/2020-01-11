#ifndef __USER_INIT_H__
#define __USER_INIT_H__


typedef struct {
  uint16_t nwk_addr;
  uint8_t  sn;
  uint8_t  endpoint;
  uint8_t  payload[8];
}RadioData_t;

void InitZigbee(void);

#endif 

