#ifndef  __INCLUDE_H_ZGM__
#define  __INCLUDE_H_ZGM__

/*===================================================================
UART1:
       TX=== PC6   ==引出来，调试用 
       RX=== PC7   ==引出来，调试用
UART0:
      TX=== PA0   与LINUX系统的RX相连 
      RX===PA1    与LINUX系统的TX相连 
===================================================================*/

/**************************
串口命令格式：
按次序：依次如下：
     HEAD ===  1字节  (0xFE)
     LEN  ===  1字节  (0x06)
     FCS  ===  1字节  (0x88)
$PROJ_DIR$\..\..\v4_2\developer\sdks\gecko_sdk_suite\v2.6\protocol\zigbee\app\util\zigbee-framework	
$PROJ_DIR$\..\..\v4_2\developer\sdks\gecko_sdk_suite\v2.6\user		 		
******************************/
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include "stack/include/ember-types.h"
#include "hal/hal.h"
#include "app/framework/include/af.h"
#include "app/framework/util/attribute-storage.h"
#include "stack/include/ember-types.h"
#include "stack/include/event.h"
#include "stack/config/token-phy.h"
#include "../include/af.h"
#include "../plugin/time-server/time-server.h"
#include "../plugin/concentrator/source-route-common.h"
#include "app/framework/util/af-event.h"
#include "app/framework/util/time-util.h"
//#include "afv2-bookkeeping.h"
#include "app/framework/util/util.h"
#include "app/framework/util/attribute-table.h"
#include "platform/base/hal/micro/generic/compiler/platform-common.h"

#include <stdio.h>
#include <math.h>
#include <string.h>


/* ------------------------------------------------------------------------------------------------
 *                                               Types
 * ------------------------------------------------------------------------------------------------
 */
typedef signed   char      int8;
typedef unsigned char      uint8;

typedef signed   short     int16;
typedef unsigned short     uint16;

typedef signed   long      int32;
typedef unsigned long      uint32;
typedef unsigned long long uint64; 

#define bool               _Bool

#include "user_tool.h"
#include "user_memory.h"
#include "user_list.h"
#include "user_deviceJoin.h"
#include "user_event.h"
#include "user_zdorsp.h"
#include "user_init.h"
#include "user_uart0dma.h"
#include "user_fifo.h"
#include "user_uartrtx.h"
#include "user_proc_rx.h"
#include "user_zcl.h"


#endif


