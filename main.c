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

static void wait(void)
{
	int i;

	for (i = 0; i < 10000000; i++) {
		asm volatile ("nop");
	}
}

#define PORTA_LED1	(1 << 0)
#define PORTA_LED2	(1 << 1)
#define PORTA_LED3	(1 << 2)
#define PORTA_LED4	(1 << 6)
#define PORTA_LEDS	(PORTA_LED4 | PORTA_LED3 | PORTA_LED2 | PORTA_LED1)

int main(void)
{
	volatile uint8_t *PORTA_PDR  = (uint8_t *)(PORTx_PDR + PORTA);
	volatile uint8_t *PORTA_PODR = (uint8_t *)(PORTx_PODR + PORTA);
	uint8_t val;

	*PORTA_PDR = PORTA_LEDS;

	while (1) {
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
	}
}
