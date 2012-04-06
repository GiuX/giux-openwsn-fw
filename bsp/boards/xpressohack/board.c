/**
\brief LPC17XX-specific definition of the "board" bsp module.

\author Xavi Vilajosana <xvilajosana@eecs.berkeley.edu>, February 2012.
 */

#include "LPC17xx.h"
#include "board.h"
#include "board_info.h"
#include "leds.h"
#include "uart.h"
#include "spi.h"
#include "radio.h"
#include "bsp_timer.h"
#include "clkpwr.h"
#include "debugpins.h"
#include "radiotimer.h"


//=========================== variables =======================================

//=========================== prototypes ======================================

extern void EINT3_IRQHandler(void);

//=========================== public ==========================================

void board_init() {

	//===== radio pins
	// OpenMote SLP_TR [P1.22]
#ifdef OPENMOTE
	LPC_PINCON->PINSEL3      &= ~(0x3<<12);    // GPIO mode
	LPC_GPIO1->FIODIR        |=  1<<22;       // set as output
	LPC_GPIO1->FIOCLR        |=  1<<22;       // pull low
#endif
	//LPCXpresso is [P2.8]
#ifdef LPCXPRESSO1769
	LPC_PINCON->PINSEL4      &= ~(0x3<<16);    // GPIO mode
	LPC_GPIO2->FIODIR        |=  1<<8;       // set as output
	LPC_GPIO2->FIOCLR        |=  1<<8;       // pull low
#endif

	// [P0.17] RSTn
	LPC_PINCON->PINSEL1      &= ~(0x3<<2);     // GPIO mode
	LPC_GPIO0->FIODIR        |=  1<<17;       // set as output
	// [P0.22] ISR
	LPC_PINCON->PINSEL1      &= ~(0x3<<12);    // GPIO mode
	LPC_GPIO0->FIODIR        &= ~(1<<22);       // set as input
	LPC_GPIOINT->IO0IntClr   |=  1<<22;       // clear possible pending interrupt
	LPC_GPIOINT->IO0IntEnR   |=  1<<22;       // enable interrupt, rising edge

	// enable interrupts
	NVIC_EnableIRQ(EINT3_IRQn);              // GPIOs -- check that..

	debugpins_init();
	leds_init();
	uart_init();

	spi_init();
	//   i2c_init();
	bsp_timer_init();
	radio_init();
	radiotimer_init();

}

void board_sleep() {
	CLKPWR_Sleep();
}

//=========================== private =========================================

//=========================== interrupt handlers ==============================

// GPIOs
// note: all GPIO interrupts, both port 0 and 2, trigger this same vector
void EINT3_IRQHandler(void) {
	if ((LPC_GPIOINT->IO0IntStatR) & (1<<22)) {
		LPC_GPIOINT->IO0IntClr = (1<<22);
		//capture timer
		radio_isr();
	}
}
