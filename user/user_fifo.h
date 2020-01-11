#ifndef  __USER_FIFO_H__
#define  __USER_FIFO_H__


#define MAX_RX_BUFF   1024
#define MAX_TX_BUFF   1024
#define IDLE_TIMEOUT   6

typedef void (*halUARTCBack_t) (uint8 port, uint8 event);

typedef struct
{
  // The head or tail is updated by the Tx or Rx ISR respectively, when not polled.
  volatile uint16 bufferHead;
  volatile uint16 bufferTail;
  uint16 maxBufSize;
  uint8 *pBuffer;
} halUARTBufControl_t;

typedef struct
{
  uint8               idleTimeout;
  halUARTBufControl_t rx;
  halUARTBufControl_t tx;
  uint32              rxChRvdTime;
  halUARTCBack_t      callBackFunc;
}halUARTCfg_t;

extern halUARTCfg_t uartRecord;  

void RecordRst(void);
uint16 Hal_UART_RxBufLen(void);
uint16 Hal_UART_TxBufLen( void ); 
uint16 HalUARTWrite(uint8 *pBuffer, uint16 length);
uint16 HalUARTReadIsr(uint8 *pBuffer, uint16 length );  
#endif 

