#ifndef OSAL_MEMORY_H
#define OSAL_MEMORY_H

#define MAXMEMHEAP   (18*1024)     //18K

#define HAL_ASSERT(expr)                      
//st( if (!( expr )) halAssertHandler(); )
#define VOID (void)

typedef uint32             halDataAlign_t;
typedef bool               halIntState_t;

typedef struct {
  unsigned len : 15;
  unsigned inUse : 1;
} osalMemHdrHdr_t;

typedef union {
  halDataAlign_t alignDummy;
  uint16 val;
  osalMemHdrHdr_t hdr;
} osalMemHdr_t;

void mem_init(void);
void osal_mem_init(void);
int osal_mem_kick(void);
void *osal_mem_alloc( uint16 size );
int osal_mem_free(void *ptr);

#endif /* #ifndef OSAL_MEMORY_H */
