/**
\brief LPC17XX-specific definition of the "board" bsp module.

\author Xavi Vilajosana <xvilajosana@eecs.berkeley.edu>, February 2012.
 */

#include "board.h"
#include "board_info.h"
#include "leds.h"
#include "led.h"
#include "bsp_timer.h"
#include "smc.h"
#include "mcg.h"


/* Actual system clock frequency */
int mcg_clk_hz;
int mcg_clk_khz;
int core_clk_khz;
int core_clk_mhz;
int periph_clk_khz;
//=========================== variables =======================================

//=========================== prototypes ======================================


void cb();
void sysinit();
//=========================== public ==========================================

uint16_t count;
uint16_t time;
extern int mote_main(void);

int main(void) {
	return mote_main();
}

void board_init() {
	uint8_t mcgmode=0;
	//enable all port clocks.
	SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK
			| SIM_SCGC5_PORTB_MASK
			| SIM_SCGC5_PORTC_MASK
			| SIM_SCGC5_PORTD_MASK
			| SIM_SCGC5_PORTE_MASK );

	sysinit();//enables pll

	/*Enable ALLS operation mode. This is a write once register*/  
	SMC_PMPROT |= SMC_PMPROT_ALLS_MASK;

	mcgmode= what_mcg_mode();

	llwu_init();//low leakage unit init

	debugpins_init();
	leds_init();
	bsp_timer_init();
	uart_init();
	radiotimer_init();
	spi_init();
	leds_all_off();
}


void sysinit(){
	/* Ramp up the system clock */
	/* Set the system dividers */
	/* NOTE: The PLL init will not configure the system clock dividers,
	 * so they must be configured appropriately before calling the PLL
	 * init function to ensure that clocks remain in valid ranges.
	 */  
	SIM_CLKDIV1 = ( 0
			| SIM_CLKDIV1_OUTDIV1(0)
			| SIM_CLKDIV1_OUTDIV2(1)
			| SIM_CLKDIV1_OUTDIV3(1)
			| SIM_CLKDIV1_OUTDIV4(2) );

	/*Reading this bit indicates whether certain peripherals and the I/O pads are in a latched state as a result of
		        having been in a VLLS mode. Writing one to this bit when it is set releases the I/O pads and certain
		        peripherals to their normal run mode state.*/

	if (PMC_REGSC &  PMC_REGSC_ACKISO_MASK)
		PMC_REGSC |= PMC_REGSC_ACKISO_MASK; //write to release hold on I/O 

	/* Initialize PLL */ 
	/* PLL will be the source for MCG CLKOUT so the core, system, and flash clocks are derived from it */ 
	mcg_clk_hz = pll_init(CLK0_FREQ_HZ,  /* CLKIN0 frequency */
			LOW_POWER,     /* Set the oscillator for low power mode */
			CLK0_TYPE,     /* Crystal or canned oscillator clock input */
			PLL0_PRDIV,    /* PLL predivider value */
			PLL0_VDIV,     /* PLL multiplier */
			MCGOUT);       /* Use the output from this PLL as the MCGOUT */

	/* Check the value returned from pll_init() to make sure there wasn't an error */
	if (mcg_clk_hz < 0x100)
		while(1);

	/*
	 * Use the value obtained from the pll_init function to define variables
	 * for the core clock in kHz and also the peripheral clock. These
	 * variables can be used by other functions that need awareness of the
	 * system frequency.
	 */
	mcg_clk_khz = mcg_clk_hz / 1000;
	core_clk_khz = mcg_clk_khz / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK) >> 28)+ 1);
	periph_clk_khz = mcg_clk_khz / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV2_MASK) >> 24)+ 1);
}


void board_sleep() {
	uint8_t op_mode;
	//enter_wait();
	//    PORTA_PCR2 |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;//JTAG_TDO    
	enter_wait();
	op_mode = what_mcg_mode();
	if(op_mode==PBE)
	{
		mcg_clk_hz = pbe_pee(CLK0_FREQ_HZ);
	}
	//enter_wait();
}

//=========================== private =========================================

//=========================== interrupt handlers ==============================

