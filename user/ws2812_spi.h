void ws2812spi_push(char *pdata, unsigned short len);
void ICACHE_FLASH_ATTR ws2812spi_init(void);

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif /* MIN */

#ifndef MIN_MEMCPY
#define MIN_MEMCPY(a, b) os_memcpy(&(a), (b), MIN(sizeof (a), strlen(b)+1))
#endif /* MIN_MEMCPY */