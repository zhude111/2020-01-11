#include "user_include.h"

#include "em_gpio.h"
#include "em_usart.h"
#include "em_cmu.h"
#include "em_bus.h"
#include "em_assert.h"
#include "uartdrv.h"

unsigned int txDmaCh=0;
unsigned int rxDmaCh=0; 
unsigned int txDmaChFin=1;
unsigned int rxDmaChFin=1;

void InitUart0(void)
{  
   USART_InitAsync_TypeDef usartInit = USART_INITASYNC_DEFAULT;
#if defined(_CMU_HFPERCLKEN0_MASK)
  CMU_ClockEnable(cmuClock_HFPER, true);
#endif
    CMU_ClockEnable(cmuClock_GPIO, true);
    CMU_ClockEnable(cmuClock_USART0, true);
  
    GPIO_PinModeSet(gpioPortA, 0, gpioModePushPull, 1); //tx 
    GPIO_PinModeSet(gpioPortA, 1, gpioModeInputPull, 1); //rx
    
    usartInit.baudrate=57600;//115200
    
    usartInit.enable = usartDisable;
    USART_InitAsync(USART0, &usartInit);
 
  
     USART0->ROUTELOC0 = (USART0->ROUTELOC0
                               & ~(_USART_ROUTELOC0_TXLOC_MASK
                                   | _USART_ROUTELOC0_RXLOC_MASK))
                                   | (0 << _USART_ROUTELOC0_TXLOC_SHIFT)
                                   | (0 << _USART_ROUTELOC0_RXLOC_SHIFT)  ;
     USART0->ROUTEPEN = USART_ROUTEPEN_TXPEN
                        | USART_ROUTEPEN_RXPEN;
     
     USART_IntClear(USART0, ~0x0);
     USART0->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;
    
     USART_IntClear(USART0, USART_IF_RXDATAV);
     NVIC_ClearPendingIRQ(USART0_RX_IRQn);
     
     USART_Enable(USART0, usartEnable);
     
    USART_IntEnable(USART0, USART_IF_RXDATAV);
    NVIC_EnableIRQ(USART0_RX_IRQn);
     
     // USART_IntEnable(USART0, USART_IF_TXC);
     //  NVIC_EnableIRQ(USART0_TX_IRQn);
     //  USART_IntSet(USART0,USART_IF_TXC);
     //  emberEventControlSetDelayMS(customEventControl2,1000);
     //DMADRV_Init();
    DMADRV_AllocateChannel(&txDmaCh, NULL);
    //emberAfCorePrintln("txDmaCh=%d",txDmaCh);
    //ret=DMADRV_AllocateChannel(&rxDmaCh, NULL); 
     RecordRst(); 
    
#if 0
      UartSendData("1234567890\n",11);
#endif      
 
}

void UartSendData(uint8_t *pbuf,uint16_t len)
{  
  txDmaChFin=0;
  DMADRV_MemoryPeripheral(txDmaCh,
                          dmadrvPeripheralSignal_USART0_TXBL,
                          (void*)&(USART0->TXDATA),
                          pbuf,
                          true,
                          len,
                          dmadrvDataSize1,
                          UartTransmitComplete,
                          NULL);    
}





