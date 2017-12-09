#include <c_types.h>
#include <osapi.h>
#include <c_types.h>
#include <mem.h>
#include <ets_sys.h>
#include <gpio.h>
#include <os_type.h>
#include <user_interface.h>
#include <espconn.h>
#include <driver/spi.h>
#include <driver/spi_register.h>
#include "ws2812_spi.h"

#define LEDS 120

#define ZERO 0xe0
#define ONE  0xf8

static uint32_t zero_one24[2] = { ZERO << 24, ONE << 24 };
static uint32_t zero_one16[2] = { ZERO << 16, ONE << 16 };
static uint32_t zero_one8[2] = { ZERO << 8, ONE << 8 };
static uint32_t zero_one[2] = { ZERO, ONE };

/* G7..G0 R7..R0 B7..B0 */

void ws2812spi_push(char *pdata, unsigned short len) {
	uint32_t rgb[6];
	unsigned i, n = MIN(3*LEDS, len);

	for (i=0; i<n; i+=3) {
		rgb[0] = zero_one24[!!(pdata[i+1] & 0x80)] |
			zero_one16[!!(pdata[i+1] & 0x40)] |
			zero_one8[!!(pdata[i+1] & 0x20)] |
			zero_one[!!(pdata[i+1] & 0x10)];

		rgb[1] = zero_one24[!!(pdata[i+1] & 0x08)] |
			zero_one16[!!(pdata[i+1] & 0x04)] |
			zero_one8[!!(pdata[i+1] & 0x02)] |
			zero_one[!!(pdata[i+1] & 0x01)];

		rgb[2] = zero_one24[!!(pdata[i] & 0x80)] |
			zero_one16[!!(pdata[i] & 0x40)] |
			zero_one8[!!(pdata[i] & 0x20)] |
			zero_one[!!(pdata[i] & 0x10)];

		rgb[3] = zero_one24[!!(pdata[i] & 0x08)] |
			zero_one16[!!(pdata[i] & 0x04)] |
			zero_one8[!!(pdata[i] & 0x02)] |
			zero_one[!!(pdata[i] & 0x01)];

		rgb[4] = zero_one24[!!(pdata[i+2] & 0x80)] |
			zero_one16[!!(pdata[i+2] & 0x40)] |
			zero_one8[!!(pdata[i+2] & 0x20)] |
			zero_one[!!(pdata[i+2] & 0x10)];

		rgb[5] = zero_one24[!!(pdata[i+2] & 0x08)] |
			zero_one16[!!(pdata[i+2] & 0x04)] |
			zero_one8[!!(pdata[i+2] & 0x02)] |
			zero_one[!!(pdata[i+2] & 0x01)];

		while (spi_busy(HSPI));

		WRITE_PERI_REG(SPI_W0(HSPI), rgb[0]);
		WRITE_PERI_REG(SPI_W1(HSPI), rgb[1]);
		WRITE_PERI_REG(SPI_W2(HSPI), rgb[2]);
		WRITE_PERI_REG(SPI_W3(HSPI), rgb[3]);
		WRITE_PERI_REG(SPI_W4(HSPI), rgb[4]);
		WRITE_PERI_REG(SPI_W5(HSPI), rgb[5]);

		SET_PERI_REG_MASK(SPI_CMD(HSPI), SPI_USR);
	}
}

void ICACHE_FLASH_ATTR ws2812spi_init(void) {

	spi_init_gpio(HSPI, SPI_CLK_USE_DIV);

	/* 80 MHz / 12 / 1 = 6.666 MHz */
	spi_clock(HSPI, 12, 1);

	spi_tx_byte_order(HSPI, SPI_BYTE_ORDER_HIGH_TO_LOW);
	spi_rx_byte_order(HSPI, SPI_BYTE_ORDER_HIGH_TO_LOW);

	CLEAR_PERI_REG_MASK(SPI_USER(HSPI),
		SPI_CS_SETUP | SPI_CS_HOLD | SPI_FLASH_MODE);

	CLEAR_PERI_REG_MASK(SPI_USER(HSPI),
		SPI_USR_MISO|SPI_USR_COMMAND|SPI_USR_ADDR|SPI_USR_DUMMY);

	SET_PERI_REG_MASK(SPI_USER(HSPI), SPI_USR_MOSI);

	WRITE_PERI_REG(SPI_USER1(HSPI),
		/* address bits */
		((0-1) & SPI_USR_ADDR_BITLEN) << SPI_USR_ADDR_BITLEN_S |
		/* MOSI bits */
		((192-1) & SPI_USR_MOSI_BITLEN) << SPI_USR_MOSI_BITLEN_S |
		/* MISO bits */
		((0-1) & SPI_USR_MISO_BITLEN) << SPI_USR_MISO_BITLEN_S |
		/* dummy bits */
		((0-1) & SPI_USR_DUMMY_CYCLELEN) << SPI_USR_DUMMY_CYCLELEN_S);
}