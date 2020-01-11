#include "user_include.h"

#include <math.h>
halUARTCfg_t uartRecord;

uint8 RxBuf[MAX_RX_BUFF]={0};
uint8 TxBuf[MAX_TX_BUFF]={0};


void RecordRst(void)
{
  uartRecord.rx.bufferHead     = 0;
  uartRecord.rx.bufferTail     = 0;
  uartRecord.rx.pBuffer        = (uint8 *)RxBuf;
  uartRecord.tx.bufferHead     = 0;
  uartRecord.tx.bufferTail     = 0;
  uartRecord.tx.pBuffer        = (uint8 *)TxBuf;
  uartRecord.rxChRvdTime       = 0;  
  uartRecord.rx.maxBufSize        = MAX_RX_BUFF;
  uartRecord.tx.maxBufSize        = MAX_TX_BUFF;
  uartRecord.idleTimeout          = IDLE_TIMEOUT;
  //uartRecord.callBackFunc         = MT_UartProcessZToolData;
}
/*************************************************************************************************
 * @brief   Calculate Rx Buffer length of a port
 * @return  length of current Rx Buffer
 *************************************************************************************************/
uint16 Hal_UART_RxBufLen(void)
{
  int16 length = uartRecord.rx.bufferTail;

  length -= uartRecord.rx.bufferHead;
  if  (length < 0)
    length += uartRecord.rx.maxBufSize;

  return (uint16)length;
}
/*************************************************************************************************
 * @fn      Hal_UART_TxBufLen()
 * @brief   Calculate Tx Buffer length of a port
 * @return  length of current Tx buffer
 *************************************************************************************************/
uint16 Hal_UART_TxBufLen( void )
{
  int16 length = uartRecord.tx.bufferTail;

  length -= uartRecord.tx.bufferHead;
  if  (length < 0)
    length += uartRecord.tx.maxBufSize;

  return (uint16)length;
}
/*************************************************************************************************
 * @fn      HalUARTWrite()
 * @brief   Write a buffer to the UART
 * @param   pBuffer - pointer to the buffer that will be written
 *          length  - length of
 * @return  length of the buffer that was sent
 *************************************************************************************************/
uint16 HalUARTWrite(uint8 *pBuffer, uint16 length)
{
  uint16 idx = uartRecord.tx.bufferHead;
  uint16 cnt = uartRecord.tx.bufferTail;

  if (cnt == idx)
  {
    cnt = uartRecord.tx.maxBufSize;
  }
  else if (cnt > idx)
  {
    cnt = uartRecord.tx.maxBufSize - cnt + idx;
  }
  else 
  {
    cnt = idx - cnt;
  }
  if (cnt < length)
  {
    return 0;
  }
 // txMT = false;
  idx = uartRecord.tx.bufferTail;

  for (cnt = 0; cnt < length; cnt++)
  {
    uartRecord.tx.pBuffer[idx++] = pBuffer[cnt];

    if (idx >= uartRecord.tx.maxBufSize)
    {
      idx = 0;
    }
  }
  uartRecord.tx.bufferTail = idx;
  
  return length;  
}

/*************************************************************************************************
 * @fn      HalUARTReadIsr()
 *
 * @brief   Read a buffer from the UART
 *
 * @param   port - UART port (not used.)
 *          ppBuffer - pointer to a pointer that points to the data that will be read
 *          length - length of the requested buffer
 *
 * @return  length of buffer that was read
 *************************************************************************************************/
uint16 HalUARTReadIsr (uint8 *pBuffer, uint16 length )
{
  uint16 cnt, idx;
  cnt = Hal_UART_RxBufLen();
  if (cnt < length)
  {
    length = cnt;
  }

  idx = uartRecord.rx.bufferHead;
  for (cnt = 0; cnt < length; cnt++)
  {
    pBuffer[cnt] = uartRecord.rx.pBuffer[idx++];

    if (idx >= uartRecord.rx.maxBufSize)
    {
      idx = 0;
    }
  }
  uartRecord.rx.bufferHead = idx;
  return length;  
}

