#include <msp430.h>

void setup(void) {
    // Configuration stuff goes here
    // Timer A is driven from SMCLK = MCLK = 1MHz with a divider of 8
    //  --> Timer A is at ~ 125 kHz
    TACTL = TASSEL_2 | MC_1 | ID_3 | TACLR;

    // enable interrupt
    TACCTL0 |= CCIE;

    // At these rates, TACCR0 overflows every .1 seconds ~ 10Hz
    TACCR0 = 12500;

}

int main(void)
{
    // Stop watchdog timer
    WDTCTL = WDTPW + WDTHOLD;

    // Set frequency
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    setup();

    // Set P1.0 to output direction
    P1DIR |= 0x01;                            
    P1OUT = 0x00;

    // Wait for the chip to spin up
    volatile unsigned int i = 1000;
    while(i--);
    
    P1OUT = 0x01;

    //////////////////////////////////////////////////
    // TEST CODE GOES HERE

    // Clear the clock
    TACTL |= TACLR;

    // Enable interrupts
    _BIS_SR(GIE);
    
    // Enter LPM0
    LPM0;

    //////////////////////////////////////////////////
    
    P1OUT = 0x00;
    
    return 0;
}

__attribute__((interrupt(TIMER0_A0_VECTOR)))
void Timer_A(void){
    // Exit LPM0
    LPM0_EXIT;
}
