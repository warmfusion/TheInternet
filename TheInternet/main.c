#include <msp430g2231.h>


//How many cycles of the clock before we trigger the interupt
// The VLO is around 12000/second, though environmental conditions change this
#define TRIGGER 12000
#define LED 0x01


void main(void)
{
	WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer

		/* Set the entire port P1 to output to reduce power consumption. */
		P1DIR = 0xff;
		P1OUT = LED;
		// the VLO must be enabled by setting bits 4 and 5 (LFXT1S) to 2 in the Basic Clock System Control Register 3 (BCSCTL3), as follows:
		BCSCTL3 |= LFXT1S_2; //Enable the build in VLO clock
		TACCR0 = TRIGGER;

		// We want to be notified when the timer resets, so we enable the register 0
		// interrupt by setting bit 4 (CCIE) of the Timer_A Capture/Compare Control Register 0 (TACCTL0).
		// This will cause a wakeup event each time the count-up timer reaches the value stored in TACCR0,
		// as well as the execution of the Timer_A interrupt handler if it is defined.
		TACCTL0 |= CCIE;

		// Once the timer trigger value is set and its interrupt enabled, we must configure it for count-up
		// mode and start the timer. These operations are both performed in the Timer_A Control Register, TACTL.
		// To select ACLK as our clock source, we set bits 8 and 9 (TASSEL) to 01, and to enable the timer
		//in count-up mode, we set bits 4 and 5 (MC) to 01 as well:
		TACTL = TASSEL_1 | MC_1;


		// Clear the timer and enable timer interrupt
		/* Enable interrupts */
		__enable_interrupt();


		__bis_SR_register(LPM3_bits); // LPM0 with interrupts enabled

		/* Unreached.  If this code were reached, the LED would stay
		 * continuously lit. */
		while (1)
			P1OUT = LED;
}


// Timer A0 interrupt service routine
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A (void)
{
	P1OUT ^= LED;
}
