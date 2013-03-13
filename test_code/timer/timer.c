#include <msp430.h>

void setup(void) {
    // Configuration stuff goes here
    TACTL = TASSEL_2 | MC_1 | ID_3 | TACLR;
    TACCR0 = 16000;
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

    i = 2000;
    while (--i) {

    }
    
    //////////////////////////////////////////////////
    
    P1OUT = 0x00;
    
    return 0;
}
