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

    volatile unsigned int i = 1000;
    while(i--);
    
    P1OUT = 0x01;
    // i = 20;
    // while (i--)
    // P1OUT = 0x00;
    
    i = 10000;
    while(i--);
        
    // P1OUT = 0x01;
    // i = 20;
    // while (i--)
    P1OUT = 0x00;
    
    return 0;
}
