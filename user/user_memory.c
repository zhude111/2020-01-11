#include "user_include.h"

#define OSALMEM_IN_USE             0x8000
#if (MAXMEMHEAP & OSALMEM_IN_USE)
#error MAXMEMHEAP is too big to manage!
#endif

#define OSALMEM_HDRSZ              sizeof(osalMemHdr_t)

#define OSALMEM_ROUND(X)       ((((X) + OSALMEM_HDRSZ - 1) / OSALMEM_HDRSZ) * OSALMEM_HDRSZ)

#if !defined OSALMEM_MIN_BLKSZ
#define OSALMEM_MIN_BLKSZ         (OSALMEM_ROUND((OSALMEM_HDRSZ * 2)))
#endif

#if !defined OSALMEM_SMALL_BLKSZ
#define OSALMEM_SMALL_BLKSZ       (OSALMEM_ROUND(16))
#endif

#if !defined OSALMEM_SMALL_BLKCNT
#define OSALMEM_SMALL_BLKCNT       8
#endif

#define OSALMEM_INIT              'X'

#define OSALMEM_LASTBLK_IDX      ((MAXMEMHEAP / OSALMEM_HDRSZ) - 1)


// Adjust accordingly to attempt to accomodate the vast majority of very high frequency operations.
#define OSALMEM_SMALLBLK_BUCKET  ((OSALMEM_SMALL_BLKSZ * OSALMEM_SMALL_BLKCNT))
#define OSALMEM_SMALLBLK_HDRCNT   (OSALMEM_SMALLBLK_BUCKET / OSALMEM_HDRSZ)
#define OSALMEM_BIGBLK_SZ         (MAXMEMHEAP - OSALMEM_SMALLBLK_BUCKET - OSALMEM_HDRSZ*2)
#define OSALMEM_BIGBLK_IDX        (OSALMEM_SMALLBLK_HDRCNT + 1)
osalMemHdr_t theHeap[MAXMEMHEAP / OSALMEM_HDRSZ];

osalMemHdr_t *pff=NULL; 
uint8 osalMemStat=0;          

void mem_init(void)
{
 //int  val=0;
 //int *pt=NULL; 
    osal_mem_init();
    osal_mem_kick();
//  emberAfCorePrintln("sizeof(uint32)=%d,sizeof(uint64)=%d,sizeof(Device_Join_T)=%d ",sizeof(uint32),sizeof(uint64),sizeof(Device_Join_T));
}

void osal_mem_init(void)
{

  osal_memset(theHeap, OSALMEM_INIT, MAXMEMHEAP); 
  theHeap[OSALMEM_LASTBLK_IDX].val = 0;
  pff =theHeap;
  pff->val = OSALMEM_SMALLBLK_BUCKET;  
  theHeap[OSALMEM_SMALLBLK_HDRCNT].val = (OSALMEM_HDRSZ | OSALMEM_IN_USE);
  theHeap[OSALMEM_BIGBLK_IDX].val = OSALMEM_BIGBLK_SZ;

  //emberAfCorePrintln("theHeap addr: 0x%04x ",(int)&theHeap[0]);
  #if 0
  emberAfCorePrintln("theHeap max addr: 0x%04x ",(int)&theHeap[OSALMEM_LASTBLK_IDX]);
  emberAfCorePrintln("count : %d ",&theHeap[OSALMEM_LASTBLK_IDX]-&theHeap[0]);
  emberAfCorePrintln("pff_paddr: 0x%04x ",pff);
  emberAfCorePrintln("pff->val: 0x%04x ",pff->val);
  emberAfCorePrintln("OSALMEM_SMALLBLK_HDRCNT: 0x%04x ",OSALMEM_SMALLBLK_HDRCNT);
  emberAfCorePrintln("OSALMEM_SMALLBLK_HDRCNT->val: 0x%04x ",theHeap[OSALMEM_SMALLBLK_HDRCNT].val);
  emberAfCorePrintln("OSALMEM_BIGBLK_IDX->val: 0x%04x ",theHeap[OSALMEM_BIGBLK_IDX].val);
#endif
}

int osal_mem_kick(void)
{
  osalMemHdr_t *tmp = osal_mem_alloc(1);
 if(tmp==NULL)  return 1;
     CORE_DECLARE_IRQ_STATE;
     CORE_ENTER_ATOMIC(); 
          pff = tmp - 1;      
          osal_mem_free(tmp);
          osalMemStat = 0x01; 
      CORE_EXIT_ATOMIC(); 
  return 0;
}

void *osal_mem_alloc( uint16 size )
{
    osalMemHdr_t *prev = NULL;
    osalMemHdr_t *hdr;
    uint8 coal = 0;
    
    if(size==0)  
    {
      return NULL;   
    }
    
    CORE_DECLARE_IRQ_STATE;
      size += OSALMEM_HDRSZ;
      const uint8 mod = size % sizeof( halDataAlign_t );
      if ( mod != 0 )
      {
        size += (sizeof( halDataAlign_t ) - mod);
      }
    
       CORE_ENTER_ATOMIC();
        if ((osalMemStat == 0) || (size <= OSALMEM_SMALL_BLKSZ))
          {
            hdr = pff;
          }
          else
          {
            hdr = (theHeap + OSALMEM_BIGBLK_IDX);
          }
        
          do
            {
              if(hdr->hdr.inUse)
              {
                    coal = 0;
              }
              else
              {
                if ( coal != 0 )
                {
                    prev->hdr.len += hdr->hdr.len;
                    if ( prev->hdr.len >= size )
                    {
                      hdr = prev;
                      break;
                    }
                }
                else
                {
                    if ( hdr->hdr.len >= size )
                    {
                      break;
                    }
                    coal = 1;
                    prev = hdr;
                }
              }
              hdr = (osalMemHdr_t *)((uint8 *)hdr + hdr->hdr.len);
              if ( hdr->val == 0 )
                {
                  hdr = NULL;
                  break;
                }
            }while (1);
          //=====================
            if ( hdr != NULL )
                    {
                      uint16 tmp = hdr->hdr.len - size;
                      if ( tmp >= OSALMEM_MIN_BLKSZ )
                      {
                        osalMemHdr_t *next = (osalMemHdr_t *)((uint8 *)hdr + size);
                        next->val = tmp;                     
                        hdr->val = (size | OSALMEM_IN_USE);  
                      }
                      else
                      {
                        hdr->hdr.inUse = TRUE;
                      }

                      if ((osalMemStat != 0) && (pff == hdr))
                      {
                        pff = (osalMemHdr_t *)((uint8 *)hdr + hdr->hdr.len);
                      }
                      hdr++;
                    }
           CORE_EXIT_ATOMIC();  
  return (void *)hdr;  
}


int osal_mem_free(void *ptr)
{
   osalMemHdr_t *hdr = (osalMemHdr_t *)ptr - 1;
   int stutu=0;
   CORE_DECLARE_IRQ_STATE;
  
        if(((uint8 *)ptr >= (uint8 *)theHeap) && ((uint8 *)ptr < (uint8 *)theHeap+MAXMEMHEAP))
        {
            if(hdr->hdr.inUse==FALSE) 
            {
              stutu= 1;
            }

            
                  CORE_ENTER_ATOMIC();
                      hdr->hdr.inUse = FALSE;
                      if (pff > hdr)
                      {
                        pff = hdr;
                      }   
                 CORE_EXIT_ATOMIC();
        }
        else
        {
          stutu= 1; 
        }
    return stutu;
}



















