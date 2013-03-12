#include <msp430.h>

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

    // TEST CODE GOES HERE
    volatile int a = 0;
    volatile int b = 1;
    i = 1000;
    while(i--) {
        // 10!
        a += b;
        a += b;
        a += b;
        a += b;
        a += b;
        a += b;
        a += b;
        a += b;
        a += b;
        a += b;
    }
    
    P1OUT = 0x00;
    
    return 0;
}
