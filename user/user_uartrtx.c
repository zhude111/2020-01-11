#include "user_include.h"

npi_parseinfo_t npi_parseinfo={0};

uint8 *pSendBuf=NULL;

uint16  G_macSn=0;
//==============================================================================================
uint16  GetNextMacSN(void)
{
    G_macSn++;
    
  return   G_macSn; 
}
/***************************************************************************************************
 * @fn      GenericUartCalcFCS
 *
 * @brief   
 *          
 *
 * @param   int8u *msg_ptr - message pointer
 * @param   int8u len - length (in bytes) of message
 *
 * @return  result byte
 ***************************************************************************************************/
uint8_t GenericUartCalcFCS( uint8_t *msg_ptr, uint16_t len)
{
  uint16_t x;
  uint8_t xorResult;
  
  xorResult = 0;

  for ( x = 0; x < len; x++, msg_ptr++ )
    xorResult = xorResult ^ *msg_ptr;

  return ( xorResult );
}
//DMA传输完成
bool UartTransmitComplete(unsigned int channel,
                          unsigned int sequenceNo,
                          void *userParam)
{
  txDmaChFin=1; 
  return true;
}

/*************************************************************************************************
 * @fn      procTx
 *
 * @brief   Process Tx bytes.
 *
 * @param   void
 *
 * @return  void
 *************************************************************************************************/
void procTx(void)
{
  uint16 len,cnt;
  uint16 head;
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_ATOMIC();
  
   if(txDmaChFin==0)
   {
     CORE_EXIT_ATOMIC();
     return;
   }
  CORE_EXIT_ATOMIC();
  
  if(pSendBuf)
  {
    osal_mem_free(pSendBuf);
    pSendBuf=NULL;
  }
  
  len = Hal_UART_TxBufLen();
  //emberAfCorePrintln("Hal_UART_TxBufLen= %d\n ",len);
  if(len==0) return ;
  head = uartRecord.tx.bufferHead;
  
  pSendBuf=osal_mem_alloc(len);
  if(pSendBuf==NULL)
  {
        len=MIN(len,50);
        pSendBuf=osal_mem_alloc(len);
        if(pSendBuf==NULL)
        {
          return; 
        }    
     for (cnt = 0; cnt < len; cnt++)
      {
       pSendBuf[cnt]= uartRecord.tx.pBuffer[head++];

        if (head >= uartRecord.tx.maxBufSize)
        {
          head = 0;
        }
      }
     uartRecord.tx.bufferHead =head;
     UartSendData(pSendBuf,len); 
  }
  else
  {
      for (cnt = 0; cnt < len; cnt++)
        {
         pSendBuf[cnt]= uartRecord.tx.pBuffer[head++];

          if (head >= uartRecord.tx.maxBufSize)
          {
            head = 0;
          }
        }
       uartRecord.tx.bufferHead =head;
       UartSendData(pSendBuf,len); 
  }    
}

//串口接收处理
void USART0_RX_IRQHandler (void)
{
  uint16 tail = uartRecord.rx.bufferTail;

 if(USART0->STATUS & USART_STATUS_RXDATAV) 
  {
   
      uartRecord.rx.pBuffer[tail++] = USART_Rx(USART0);
      if (tail >= uartRecord.rx.maxBufSize)
        {
          tail = 0;
        } 
  }
   uartRecord.rx.bufferTail = tail;

}
//串口接收解析
void procRx(void)
{
     uint8  ch;
   
    while (Hal_UART_RxBufLen())
      {
        
         HalUARTReadIsr(&ch, 1);
         switch (npi_parseinfo.state)
	  {
	     case SOP_STATE:
		  if(ch == GENERIC_UART_SOF)
                    {
			npi_parseinfo.state = LEN_STATE;
                    }
		  break;
             case LEN_STATE:
		  if((ch<4)||(ch>80))
                    {
		        npi_parseinfo.state = SOP_STATE;
                        break;
		    } 
			npi_parseinfo.LEN_Token = ch;
			npi_parseinfo.tempDataLen = 0;
			npi_parseinfo.pMsg = (uint8 *)osal_mem_alloc(npi_parseinfo.LEN_Token);
		  if (npi_parseinfo.pMsg)
		    {
			 npi_parseinfo.pMsg[SOP_STATE]=GENERIC_UART_SOF;
                         npi_parseinfo.pMsg[LEN_STATE]=ch;
                         npi_parseinfo.state = DATA_STATE; 
			 npi_parseinfo.tempDataLen=DATA_STATE;
                      }
		  else
		      {
                        emberAfCorePrintln("Error  Step 0 : uartRxList alloc fail...... ");
			npi_parseinfo.state = SOP_STATE;
                      }
			break;
             case DATA_STATE:
                        npi_parseinfo.pMsg[npi_parseinfo.tempDataLen++] = ch;
			npi_parseinfo.readLen =Hal_UART_RxBufLen();
                    if (npi_parseinfo.readLen <= (npi_parseinfo.LEN_Token-npi_parseinfo.tempDataLen-1))
			 {
			  HalUARTReadIsr(&npi_parseinfo.pMsg[npi_parseinfo.tempDataLen],npi_parseinfo.readLen);
			  npi_parseinfo.tempDataLen +=npi_parseinfo.readLen;
			 }
		   else
			{  
			  HalUARTReadIsr(&npi_parseinfo.pMsg[npi_parseinfo.tempDataLen], npi_parseinfo.LEN_Token - npi_parseinfo.tempDataLen-1);
			  npi_parseinfo.tempDataLen += (npi_parseinfo.LEN_Token - npi_parseinfo.tempDataLen-1);
			}
		  if(npi_parseinfo.tempDataLen == (npi_parseinfo.LEN_Token-1))
			{
			  npi_parseinfo.state = FCS_STA;
			}
			break;
	    case  FCS_STA:
		    npi_parseinfo.FSC_Token = ch;
		    npi_parseinfo.pMsg[npi_parseinfo.LEN_Token - 1]= npi_parseinfo.FSC_Token;
		    if((GenericUartCalcFCS (npi_parseinfo.pMsg, npi_parseinfo.LEN_Token - 1) == npi_parseinfo.FSC_Token)||(0x88==npi_parseinfo.FSC_Token))
			{
                          if(npi_parseinfo.LEN_Token>6)
                          {
			   emberAfCorePrint("uart_rx 0: ");
			   printf_buff(npi_parseinfo.pMsg, npi_parseinfo.LEN_Token); 
                          }
                          if(ListPushBack(uartRxList,npi_parseinfo.pMsg,0,0)==NULL)
                          {  
                             emberAfCorePrintln("Error_1: uartRxList_alloc_fail...... ");
                             osal_mem_free( (uint8 *)npi_parseinfo.pMsg );  
                          } 
			}
		   else
		       {
                             emberAfCorePrintln("Error_0 : uart rx  FCS_ERROR ! \n ");
                             osal_mem_free( (uint8 *)npi_parseinfo.pMsg );
		       }
		             npi_parseinfo.state= SOP_STATE;
		 break;
	  }
      }  
}

void printf_buff(uint8 *pbuf,uint16 len)
{
 uint16 count=0;
  
 for(count=0;count<len;count++) 
  {
    emberAfCorePrint(" %0x",pbuf[count]);
    }
 emberAfCorePrintln("  ");
}


