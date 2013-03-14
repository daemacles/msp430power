#include <msp430.h>

int fn(unsigned int a) {
    return a + 5;
}

int main(void)
{
    // Stop watchdog timer
    WDTCTL = WDTPW + WDTHOLD;

    // Set frequency
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    // Set P1.0 to output direction
    P1DIR |= 0x01;                            
    P1OUT = 0x00;

    // Wait for the chip to spin up
    volatile unsigned int i = 1000;
    while(i--);
    
    P1OUT = 0x01;

    i = 1000;
    volatile unsigned int j = 0;
    while(i--) {
        j = fn(i);
    }
    
    P1OUT = 0x00;

    i = 5;
    while(i--);
    
    return 0;
}
