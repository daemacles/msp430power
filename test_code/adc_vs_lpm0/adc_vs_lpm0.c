#include <msp430.h>

// This alternates between taking ADC samples and Low Power Mode 3.

void setup(void) {
    // Configuration stuff goes here

    // Set up VLO
    BCSCTL3 |= LFXT1S_2;
    
    // Timer A is driven from ACLK.
    //  --> Timer A is at ~ 125 kHz
    TACTL = TASSEL_1 | MC_1 | ID_3 | TACLR;

    // enable interrupt
    TACCTL0 |= CCIE;

    TACCR0 = 30;

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

    // Clear the clock
    TACTL |= TACLR;

    // Enable interrupts
    _BIS_SR(GIE);

    volatile unsigned int value;
    volatile int j;
    
    i = 5;
    while (i--) {
        j = 400;
        // turn on the ADC10
        ADC10CTL0 |= ENC | ADC10ON;
        while (j--) {
            if (!(ADC10CTL1 & ADC10BUSY)) {
                value = ADC10MEM;
                ADC10CTL0 |= ADC10SC;
            }
        }
        ADC10CTL0 &= ~(ENC);
        ADC10CTL0 &= ~(ADC10ON);
        
        // Enter LPM0
        LPM0;
    }
    
    //////////////////////////////////////////////////
    
    P1OUT = 0x00;
    
    return 0;
}

__attribute__((interrupt(TIMER0_A0_VECTOR)))
void Timer_A(void){
    // Exit LPM0
    LPM0_EXIT;
}
