#ifndef __USER_UARTDMA_H__
#define __USER_UARTDMA_H__

extern unsigned int txDmaChFin;
extern unsigned int txDmaCh;
extern unsigned int rxDmaCh;
void InitUart0(void);
void UartSendData(uint8_t *pbuf,uint16_t len);
bool UartTransmitComplete(unsigned int channel,
                                unsigned int sequenceNo,
                                void *userParam);

#endif 

