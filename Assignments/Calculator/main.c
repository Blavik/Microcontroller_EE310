/*
 * ---------------------
 * Title: Calculator
 * ---------------------
 * Program Details:
 *  Add, Subtract, Multiply, or Divide 2, 1 digit, integers and display output
 * on 2 7-segment displays
 * Inputs: 4x4 Keypad matrix. RC2-RC5 and RB0-RB3
 * Outputs: RD0-RD7 and RA0
 * Date: 4 / 9/ 2024
 * File Dependencies / Libraries:<xc.h>, <stdio.h>, <stdlib.h>
 * Compiler: xc8, 2.46
 * Author: Bjorn Lavik
 * Versions:
 *      V1.0:   4/7/2024.   Struggled to get 7 segment display working
 *      V1.1    4/8/2024.   Got display function working, struggled to read any input
 *      V1.2    4/9/2024.   Finally got system working, implementation was a nightmare
 */

// PIC18F46K42 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1L
#pragma config FEXTOSC = LP     // External Oscillator Selection (LP (crystal oscillator) optimized for 32.768 kHz; PFM set to low power)
#pragma config RSTOSC = EXTOSC  // Reset Oscillator Selection (EXTOSC operating per FEXTOSC bits (device manufacturing default))

// CONFIG1H
#pragma config CLKOUTEN = OFF   // Clock out Enable bit (CLKOUT function is disabled)
#pragma config PR1WAY = ON      // PRLOCKED One-Way Set Enable bit (PRLOCK bit can be cleared and set only once)
#pragma config CSWEN = ON       // Clock Switch Enable bit (Writing to NOSC and NDIV is allowed)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor enabled)

// CONFIG2L
#pragma config MCLRE = EXTMCLR  // MCLR Enable bit (If LVP = 0, MCLR pin is MCLR; If LVP = 1, RE3 pin function is MCLR )
#pragma config PWRTS = PWRT_OFF // Power-up timer selection bits (PWRT is disabled)
#pragma config MVECEN = ON      // Multi-vector enable bit (Multi-vector enabled, Vector table used for interrupts)
#pragma config IVT1WAY = ON     // IVTLOCK bit One-way set enable bit (IVTLOCK bit can be cleared and set only once)
#pragma config LPBOREN = OFF    // Low Power BOR Enable bit (ULPBOR disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled , SBOREN bit is ignored)

// CONFIG2H
#pragma config BORV = VBOR_2P45 // Brown-out Reset Voltage Selection bits (Brown-out Reset Voltage (VBOR) set to 2.45V)
#pragma config ZCD = OFF        // ZCD Disable bit (ZCD disabled. ZCD can be enabled by setting the ZCDSEN bit of ZCDCON)
#pragma config PPS1WAY = ON     // PPSLOCK bit One-Way Set Enable bit (PPSLOCK bit can be cleared and set only once; PPS registers remain locked after one clear/set cycle)
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config DEBUG = OFF      // Debugger Enable bit (Background debugger disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Extended Instruction Set and Indexed Addressing Mode disabled)

// CONFIG3L
#pragma config WDTCPS = WDTCPS_31// WDT Period selection bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF       // WDT operating mode (WDT Disabled; SWDTEN is ignored)

// CONFIG3H
#pragma config WDTCWS = WDTCWS_7// WDT Window Select bits (window always open (100%); software control; keyed access not required)
#pragma config WDTCCS = SC      // WDT input clock selector (Software Control)

// CONFIG4L
#pragma config BBSIZE = BBSIZE_512// Boot Block Size selection bits (Boot Block size is 512 words)
#pragma config BBEN = OFF       // Boot Block enable bit (Boot block disabled)
#pragma config SAFEN = OFF      // Storage Area Flash enable bit (SAF disabled)
#pragma config WRTAPP = OFF     // Application Block write protection bit (Application Block not write protected)

// CONFIG4H
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block not write-protected)
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)
#pragma config WRTSAF = OFF     // SAF Write protection bit (SAF not Write Protected)
#pragma config LVP = ON         // Low Voltage Programming Enable bit (Low voltage programming enabled. MCLR/VPP pin function is MCLR. MCLRE configuration bit is ignored)

// CONFIG5L
#pragma config CP = OFF         // PFM and Data EEPROM Code Protection bit (PFM and Data EEPROM code protection disabled)

#include <xc.h> // must have this
//#include "../../../../../Program Files/Microchip/xc8/v2.40/pic/include/proc/pic18f46k42.h"
#include "C:\Program Files\Microchip\xc8\v2.46\pic\include\proc\pic18f46k42.h"
//#include "C:\Program Files\Microchip\xc8\v2.40\pic\include\proc\pic18f46k42"

#define _XTAL_FREQ 4000000                 // Fosc  frequency for _delay()  library
#define FCY    _XTAL_FREQ/4

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>

#define DISPLAY_LAT LATD 
#define DISPLAY_PIN  LATA

// Binary-coded representation for each digit
const unsigned char digitPatterns[10] = {
    0b00111111,
    0b00000110,
    0b01011011,
    0b01001111,
    0b01100110,
    0b01101101,
    0b01111101,
    0b00000111,
    0b01111111,
    0b01101111  
};


void initDisplay();
int readKeypad();
void displayDigit(int digit, int commonAnode, int isNegative);
void displayTwoDigitNumber(int number);


int main() {
    initDisplay();
    int firstNumber = 0;
    int secondNumber = 0;
    int operator = 0;
    int result = 0;
    int state = 0;
    int isNegative = 0;
    displayTwoDigitNumber(00);

    while (1) {
        int key = readKeypad();

        switch (state) {
            case 0: // Waiting for the first number
                displayTwoDigitNumber(result);
                if (key >= 1 && key <= 9) {
                    firstNumber = key;
                    state = 1;
                }
                break;

            case 1: // Waiting for the operator
                displayTwoDigitNumber(firstNumber);
                if (key >= 10 && key <= 13) {
                    operator = key;
                    displayTwoDigitNumber(operator);
                    state = 2;
                }
                break;

            case 2: // Waiting for the second number
                displayTwoDigitNumber(00);
                if (key >= 1 && key <= 9) {
                    secondNumber = key;
                    displayTwoDigitNumber(secondNumber);
                    state = 3;
                }
                break;

            case 3: // Waiting for the equal symbol
                displayTwoDigitNumber(secondNumber);
                if (key == 15) {
                    switch (operator) {
                        case 10: // Addition
                            result = firstNumber + secondNumber;
                            break;
                        case 11: // Subtraction
                            result = firstNumber - secondNumber;
                            break;
                        case 12: // Multiplication
                            result = firstNumber * secondNumber;
                            break;
                        case 13: // Division
                            if (secondNumber != 0) {
                                result = (int)(firstNumber / secondNumber);
                            }
                            break;
                    }
                    if (result < 0) {
                        isNegative = 1;
                    } else {
                        isNegative = 0;
                    }
                    displayTwoDigitNumber(result);
 
                    state = 0;
                }
                break;

            default:
                
                state = 0;
                break;
        }
    }

    return 0;
}


void initDisplay() {
    TRISD = 0x00;
    TRISA &= 0xFE;
}


void displayDigit(int digit, int commonAnode, int isNegative) {
    DISPLAY_PIN = commonAnode ? 0 : 1; 

    if (isNegative) {
        // Set the sign bit
        DISPLAY_LAT &= 0xFF;
    } else {
        // Clear the sign bit
        DISPLAY_LAT &= ~0x7F;
    }

    // Set segment pins based on the binary-coded representation of the digit
    if (commonAnode) {
        DISPLAY_LAT = digitPatterns[digit];
    } else {
        DISPLAY_LAT = ~digitPatterns[digit];
    }
}


void displayTwoDigitNumber(int number) {
    int digit1 = abs(number) / 10; // Tens digit
    int digit2 = abs(number) % 10; // Ones digit
    
    static int counter = 0;
    
    // Check if the number is negative
    int isNegative = (number < 0) ? 1 : 0;
    
    if (counter % 2 == 0) {
        displayDigit(digit1, 0, isNegative);
    } else {
        displayDigit(digit2, 1, isNegative);
    }
    
    // Increment the counter
    counter++;
    if (counter >= 2) {
        counter = 0;
    }

    __delay_ms(1);
}

int readKeypad() {
    // Configure PORTC as outputs
    TRISCbits.TRISC2 = 0;
    TRISCbits.TRISC3 = 0;
    TRISCbits.TRISC4 = 0;
    TRISCbits.TRISC5 = 0;
    
    // Configure PORTB as inputs
    TRISBbits.TRISB0 = 1;
    TRISBbits.TRISB1 = 1;
    TRISBbits.TRISB2 = 1;
    TRISBbits.TRISB3 = 1;
    
    ANSELC = 0x00;
    ANSELB = 0x00;

    //key mapping for the 4x4 matrix keypad
    int keyMap[4][4] = {
        {13, 12, 11, 10},
        {15, 9, 6, 3},
        {0, 8, 5, 2},
        {14, 7, 4, 1}
    };

    // Loop through rows
    for (int row = 0; row < 4; row++) {
        LATC = (LATC & 0b11000000) | (1 << (row + 2));
        __delay_ms(1);
        // Read the input pins
        int input = PORTB & 0x0F;
        // Loop through columns
        for (int col = 0; col < 4; col++) {
            if ((input & (1 << col))) {
                return keyMap[row][col];
            }
        }
        LATC &= ~(1 << (row + 2));
    }
    return -1;
}
