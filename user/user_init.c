#include "user_include.h"


void InitZigbee(void)
{
  int ret=0;
  
     mem_init();
     ret= createList();
     InitUart0(); 
     if(ret==1)
     {
       //emberAfCorePrintln("Error: ********createList********* Fail  !!!!!");  
     }
     else
     {
      //emberAfCorePrintln("Note: ********createList********* Success  !!!!!");  
     }
     emberEventControlSetDelayMS(customEventControl,1000);
     emberEventControlSetDelayMS(customEventControl1,1000);  
}
       
void ProcessUart(void)
{
    procRx();
    procTx();
    ProcessRxList();
}
