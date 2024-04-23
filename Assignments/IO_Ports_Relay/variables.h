/* 
 * File:   variables.h
 * Author: lavik
 *
 * Created on April 23, 2024, 12:38 PM
 */

#ifndef VARIABLES_H
#define	VARIABLES_H

const int secretCode = 0x22; //This is the secret code!!!

int code1, code0;
int enteredDigit1 = 0, enteredDigit0 = 0;
int switch0Active = 0, switch1Active = 0;
int lastSwitchTime = 0;

#define DISPLAY_LAT  LATD
#define Vref 3.3 // voltage reference 
int digital; // holds the digital value 
float voltage; // hold the analog value (volt))

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

#endif	/* VARIABLES_H */

