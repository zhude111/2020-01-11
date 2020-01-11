#ifndef __USER_UARTRTX_H__
#define __USER_UARTRTX_H__

#define Resend_TIMEOUT                  3
#define Resend_TIMES                    3
#define P_HEAD                         0x00
#define P_LEN                          0x01
#define P_SDIR                         0x02
#define P_SN_L                         0x03
#define P_SN_H                         0x04
#define P_CMD_M                        0x05
#define P_CMD_S                        0x06
#define P_SN                           0x07
#define P_AMODE                        0x08
#define P_NWKL                         0x09
#define P_NWKH                         0x0A
#define P_CU_L                         0x0B
#define P_CU_H                         0x0C
#define P_EP                           0x0D
#define P_FC                           0x0E
#define P_MFG_L                        0x0F
#define P_MFG_H                        0x10
#define P_ZCLCMD                       0x11



#define G_EFR32                         0x01
#define G_M7688                         0x02
#define G_JOIN                          0x03

#define GENERIC_UART_SOF                0xFE

#define SOP_STATE                       0x00
#define LEN_STATE                       0x01
#define DATA_STATE                      0x02
#define FCS_STA                         0x03

typedef struct _npi_parseinfo_str {
	uint8 state;
	uint8 LEN_Token;
	uint8 readLen;
	uint8 FSC_Token;
	uint8 tempDataLen;
	uint8 *pMsg;
} npi_parseinfo_t;

uint16  GetNextMacSN(void);
uint8_t GenericUartCalcFCS( uint8_t *msg_ptr, uint16_t len);
void printf_buff(uint8 *pbuf,uint16 len);
bool UartTransmitComplete(unsigned int channel,unsigned int sequenceNo,void *userParam);
void procTx(void);
void procRx(void);
void ProcessUart(void);
void Pack_MacsnRsp(uint8 Sn_L,uint8 Sn_H);
void ProcessRxList(void);
#endif 

