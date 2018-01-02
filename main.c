/*
 *
 */

#include <stdint.h>

#define PORTx_PDR	0x0008C000
#define PORTx_PODR	0x0008C020

enum {
	PORT0 = 0,
	PORT1,
	PORT2,
	PORT3,
	PORT4,
	PORT5,
	PORT6,
	PORT7,
	PORT8,
	PORT9,
	PORTA,
	PORTB,
	PORTC,
	PORTD,
	PORTE,
	PORTF,
	PORTG,

	PORTJ = 0x12,
};

#if defined(RDK_RX62N) || defined(LCD_RX63N)
static void clock_setup(void)
{
	volatile uint8_t *PORT5_PDR  = (uint8_t *)(PORTx_PDR + PORT5);
	volatile uint32_t *SCKCR = (uint32_t *)0x00080020;
#ifdef LCD_RX63N
	volatile uint16_t *SCKCR3 = (uint16_t *)0x00080026;
#endif

	// 12 MHz XTAL
	*PORT5_PDR &= ~(1 << 3); // BCLK
#ifdef LCD_RX63N
	*SCKCR = (0 << 28) | (0 << 24) | (1 << 23) | (0 << 22) | (1 << 16) | (1 << 12) | (1 << 8);
#else
	*SCKCR =             (0 << 24) | (1 << 23) | (0 << 22) | (1 << 16) |             (1 << 8);
#endif
	*PORT5_PDR |= 1 << 3; // BCLK
	*SCKCR &= ~(1 << 23);

#ifdef LCD_RX63N
	*SCKCR3 = 0 << 8;
#endif
}
#endif

static void wait(void)
{
#ifdef LCD_RX63N
	const int max = 10000;
#else
	const int max = 10000000;
#endif
	int i;

	for (i = 0; i < max; i++) {
		asm volatile ("nop");
	}
}

#ifdef SAKURA
#define PORTA_LED1	(1 << 0)
#define PORTA_LED2	(1 << 1)
#define PORTA_LED3	(1 << 2)
#define PORTA_LED4	(1 << 6)
#define PORTA_LEDS	(PORTA_LED4 | PORTA_LED3 | PORTA_LED2 | PORTA_LED1)
#endif
#ifdef LCD_RX63N
#define PORT9_LEDRR	(1 << 0)
#define PORT9_LEDRG	(1 << 1)
#define PORT9_LEDLR	(1 << 2)
#define PORT9_LEDLG	(1 << 3)
#define PORT9_LEDS	(PORT9_LEDRR | PORT9_LEDRG | PORT9_LEDLR | PORT9_LEDLG)
extern void _rx_nonmaskable_exception_handler(void);
#endif

int main(void)
{
#ifdef SAKURA
	volatile uint8_t *PORTA_PDR  = (uint8_t *)(PORTx_PDR + PORTA);
	volatile uint8_t *PORTA_PODR = (uint8_t *)(PORTx_PODR + PORTA);
#if 0
	volatile uint16_t *SCKCR3 = (uint16_t *)0x00080026;
#endif
#endif
#ifdef RDK_RX62N
	volatile uint8_t *PORTD_PDR  = (uint8_t *)(PORTx_PDR + PORTD);
	volatile uint8_t *PORTD_PODR = (uint8_t *)(PORTx_PODR + PORTD);
	volatile uint8_t *PORTE_PDR  = (uint8_t *)(PORTx_PDR + PORTE);
	volatile uint8_t *PORTE_PODR = (uint8_t *)(PORTx_PODR + PORTE);
#endif
#ifdef LCD_RX63N
	volatile uint8_t *PORT9_PDR  = (uint8_t *)(PORTx_PDR + PORT9);
	volatile uint8_t *PORT9_PODR = (uint8_t *)(PORTx_PODR + PORT9);
#endif
	uint8_t val;

#if defined(RDK_RX62N) || defined(LCD_RX63N)
	clock_setup();
#endif

#ifdef SAKURA
	*PORTA_PDR = PORTA_LEDS;
#endif
#ifdef RDK_RX62N
	*PORTD_PDR = 0xff;
	*PORTE_PDR = 0xff;
#endif
#ifdef LCD_RX63N
	*PORT9_PDR = PORT9_LEDS;
#endif

	while (1) {
#ifdef SAKURA
#if 1
		val = *PORTA_PODR;
		val &= ~PORTA_LEDS;
		val |= PORTA_LED1;
		*PORTA_PODR = val;
		wait();
		val = *PORTA_PODR;
		val &= ~PORTA_LEDS;
		val |= PORTA_LED2;
		*PORTA_PODR = val;
		wait();
		val = *PORTA_PODR;
		val &= ~PORTA_LEDS;
		val |= PORTA_LED3;
		*PORTA_PODR = val;
		wait();
		val = *PORTA_PODR;
		val &= ~PORTA_LEDS;
		val |= PORTA_LED4;
		*PORTA_PODR = val;
		wait();
#else
		val = *PORTA_PODR;
		val &= ~PORTA_LEDS;
		val |= (*SCKCR3 >> 8) & 0x7;
		*PORTA_PODR = val;
		wait();
		val = *PORTA_PODR;
		val &= ~PORTA_LEDS;
		*PORTA_PODR = val;
		wait();
#endif
#endif
#ifdef RDK_RX62N
		val = *PORTD_PODR;
		val ^= 0xff;
		*PORTD_PODR = val;
		val = *PORTE_PODR;
		*PORTE_PODR = (val & 0xf0) | ((val & 0xf) ^ 0xf);
		wait();
#endif
#ifdef LCD_RX63N
		val = *PORT9_PODR;
		val |= PORT9_LEDLG | PORT9_LEDRG;
		*PORT9_PODR = val;
		wait();
		val &= ~(PORT9_LEDLG | PORT9_LEDRG);
		*PORT9_PODR = val;
		//*PORT9_PODR = (val & ~PORT9_LEDS) | (((val & PORT9_LEDS) ^ PORT9_LEDS) & PORT9_LEDS);
		wait();
#endif
	}
}

#ifdef LCD_RX63N
void __attribute__((interrupt)) _rx_nonmaskable_exception_handler(void)
{
	volatile uint8_t *PORT9_PDR  = (uint8_t *)(PORTx_PDR + PORT9);
	volatile uint8_t *PORT9_PODR = (uint8_t *)(PORTx_PODR + PORT9);
	uint8_t val;

	*PORT9_PDR = PORT9_LEDS;

	val = *PORT9_PODR;
	val |= PORT9_LEDRR | PORT9_LEDLR;
	*PORT9_PODR = val;

	// NMI must not return
	while (1) {}
}
#endif
