#ifndef OSAL_TOOL_H
#define OSAL_TOOL_H

#ifndef CONST
  #define CONST const
#endif

#ifndef GENERIC
  #define GENERIC
#endif

#ifndef false
  #define false 0
#endif

#ifndef true
  #define true 1
#endif

#ifndef BV
#define BV(n)      (1 << (n))
#endif

#ifndef BF
#define BF(x,b,s)  (((x) & (b)) >> (s))
#endif

#ifndef MIN
#define MIN(n,m)   (((n) < (m)) ? (n) : (m))
#endif

#ifndef MAX
#define MAX(n,m)   (((n) < (m)) ? (m) : (n))
#endif

#ifndef ABS
#define ABS(n)     (((n) < 0) ? -(n) : (n))
#endif

#define BUILD_UINT16(loByte, hiByte) \
          ((uint16)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))

#define BUILD_UINT32(Byte0, Byte1, Byte2, Byte3) \
          ((uint32)((uint32)((Byte0) & 0x00FF) \
          + ((uint32)((Byte1) & 0x00FF) << 8) \
          + ((uint32)((Byte2) & 0x00FF) << 16) \
          + ((uint32)((Byte3) & 0x00FF) << 24)))
            
/* takes a byte out of a uint32 : var - uint32,  ByteNum - byte to take out (0 - 3) */
#define BREAK_UINT32( var, ByteNum ) \
          (uint8)((uint32)(((var) >>((ByteNum) * 8)) & 0x00FF))  
            
#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)

#define BUILD_UINT8(hiByte, loByte) \
          ((uint8)(((loByte) & 0x0F) + (((hiByte) & 0x0F) << 4)))

#define HI_UINT8(a) (((a) >> 4) & 0x0F)
#define LO_UINT8(a) ((a) & 0x0F)

// Write the 32bit value of 'val' in little endian format to the buffer pointed 
// to by pBuf, and increment pBuf by 4
#define UINT32_TO_BUF_LITTLE_ENDIAN(pBuf,val) \
	do { \
		*(pBuf)++ = ((((uint32)(val)) >>  0) & 0xFF); \
		*(pBuf)++ = ((((uint32)(val)) >>  8) & 0xFF); \
		*(pBuf)++ = ((((uint32)(val)) >> 16) & 0xFF); \
		*(pBuf)++ = ((((uint32)(val)) >> 24) & 0xFF); \
	} while (0)

// Return the 32bit little-endian formatted value pointed to by pBuf, and increment pBuf by 4
#define BUF_TO_UINT32_LITTLE_ENDIAN(pBuf) (((pBuf) += 4), BUILD_UINT32((pBuf)[-4], (pBuf)[-3], (pBuf)[-2], (pBuf)[-1]))

#ifndef CHECK_BIT
#define CHECK_BIT(DISCS, IDX) ((DISCS) & (1<<(IDX)))
#endif          
#ifndef GET_BIT
#define GET_BIT(DISCS, IDX)  (((DISCS)[((IDX) / 8)] & BV((IDX) % 8)) ? TRUE : FALSE)
#endif
#ifndef SET_BIT
#define SET_BIT(DISCS, IDX)  (((DISCS)[((IDX) / 8)] |= BV((IDX) % 8)))
#endif
#ifndef CLR_BIT
#define CLR_BIT(DISCS, IDX)  (((DISCS)[((IDX) / 8)] &= (BV((IDX) % 8) ^ 0xFF)))
#endif
 
#define st(x)      do { x } while (__LINE__ == -1)
          
extern bool IntMasterEnable(void);
extern bool IntMasterDisable(void);

//__ATTRIBUTES unsigned int __iar_builtin_get_CPSR( void );
//__ATTRIBUTES void         __iar_builtin_set_CPSR( unsigned int );

          
#define HAL_ENTER_CRITICAL_SECTION(x)  \
  do { (x) = !IntMasterDisable(); } while (0)          

#define HAL_EXIT_CRITICAL_SECTION(x) \
  do { if (x) { (void) IntMasterEnable(); } } while (0)
          
int osal_strlen( char *pString );
void *osal_memset( void *dest, uint8 value, int len );
void *osal_memcpy( void *dst, const void GENERIC *src, unsigned int len );
uint8 osal_memcmp( const void GENERIC *src1, const void GENERIC *src2, unsigned int len );
void *osal_memdup( const void GENERIC *src, unsigned int len );
void *osal_revmemcpy( void *dst, const void GENERIC *src, unsigned int len );
void *osal_memdup( const void GENERIC *src, unsigned int len );
uint16 osal_build_uint16( uint8 *swapped );
uint32 osal_build_uint32( uint8 *swapped, uint8 len );
void _itoa(uint16 num, uint8 *buf, uint8 radix);
unsigned char * _ltoa(unsigned long l, unsigned char *buf, unsigned char radix);
#endif 
