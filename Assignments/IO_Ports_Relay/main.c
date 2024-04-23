/*
 * ---------------------
 * Title: IO Ports (Secret code)
 * ---------------------
 * Program Details:
 *  Uses 2 photo-resistors to input a secret code that either turns a motor or beeps
 * a buzzer depending on the input. The system also includes an emergency interrupt
 * and a 7-segment display to help the user see their inputs.
 * Inputs: RB0 Emergency Switch, RA0-RA1 Photo-resistor analog in.
 * Outputs: RD0-RD6 7-segment display, RB1 status LED, RB2, Relay control, RB3 Active Buzzer
 * Date: 4 / 23 / 2024
 * File Dependencies / Libraries:<xc.h>, <stdio.h>, <stdlib.h>
 * Compiler: xc8, 2.46
 * Author: Bjorn Lavik
 * Versions:
 *      V1.0:   4/22/2024.   Tested each system on breadboard and wrote 1rst version of code
 *      V1.1    4/23/2024.   finalized code and split main.c into 3 header files
 */

//#include "C:\Program Files\Microchip\xc8\v2.46\pic\include\proc\pic18f46k42.h"

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "functions.h"
#include "variables.h"

int main() {
    initDisplay();
    initADC();
    initOutputs();
    displayDigit(0);
    INTERRUPT_Initialize();
    
    // Get switch 1 input
    code1 = (secretCode & 0xF0) >> 4;
    // Get switch 0 input
    code0 = secretCode & 0x0F;
    
    // Read switch 0
    while (1) {
        // Wait until switch 0 input is triggered
        while (adcRead(0) == 0);
        
        // Read switch 0 input
        while (adcRead(0) == 1) {
            enteredDigit0++;
            LATBbits.LATB1 = 1; // Turn on RB1 LED
            displayDigit(enteredDigit0); // Display enteredDigit0 on the display
            __delay_ms(100); 
            LATBbits.LATB1 = 0; // Turn off RB1 LED
            __delay_ms(900);
        }

        __delay_ms(1000);
        
        // Break if 0
        if (adcRead(0) == 0) {
            break;
        }
    }
    
    // Read switch 1
    while (1) {
        // Wait until switch 1 input is triggered
        while (adcRead(1) == 0){
            displayDigit(enteredDigit1);
        }
        
        // Read switch 1 input
        while (adcRead(1) == 1) {
            enteredDigit1++;
            LATBbits.LATB1 = 1; // Turn on RB1 LED
            displayDigit(enteredDigit1); // Display enteredDigit1 on the display
            __delay_ms(100);
            LATBbits.LATB1 = 0; // Turn off RB1 LED
            __delay_ms(900);
        }

        __delay_ms(1000);
        
        // Break if 0
        if (adcRead(1) == 0) {
            break;
        }
    }
    
    // Compare entered digits with secret code
    if (enteredDigit0 == code0 && enteredDigit1 == code1) {
        // Correct code
        enableMotor();
    } else {
        // Incorrect code
        enableBuzzer();
    }
    
    return 0;
}
