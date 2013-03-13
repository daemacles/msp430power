#include <msp430.h>

// DOES NOT USE INTERNAL REFERENCE VOLTAGE...which adds about 200uA of drain

void setup(void) {
    // Configuration stuff goes here
    ADC10CTL0 = SREF_2 | ADC10SHT_0;
    ADC10CTL1 = INCH_1;         // in channel = A1
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

    // turn on the ADC10
    ADC10CTL0 |= ENC | ADC10ON;
    volatile unsigned int value;
    
    // spin
    i = 2000;
    while (i--) {
    // turn on the ADC10
        if (!(ADC10CTL1 & ADC10BUSY)) {
            value = ADC10MEM;
            ADC10CTL0 |= ADC10SC;
        }
    };

    // turn off the ADC10
    ADC10CTL0 = 0;
    
    //////////////////////////////////////////////////
    
    P1OUT = 0x00;
    
    return 0;
}
