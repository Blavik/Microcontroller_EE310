/* 
 * File:   functions.h
 * Author: lavik
 *
 * Created on April 23, 2024, 12:25 PM
 */

#ifndef FUNCTIONS_H
#define	FUNCTIONS_H

#include "variables.h"

int adcRead(int input) {
    if(input == 0){
        ADPCH = 0x00; //RA0 is Active Analog channel
    }
    if(input == 1){
        ADPCH = 0x01; //RA1 is Active Analog channel
    }
    //DO: Set ADCON0 Go to start conversion
    ADCON0bits.GO = 1;    
    while (ADCON0bits.GO); //Wait for conversion done
    
    digital = (ADRESH*256) | (ADRESL);/*Combine 8-bit LSB and 2-bit MSB*/
    // DO: define voltage = Vref/4096 (note that voltage is float type
    voltage = digital * (Vref / 4096);
    __delay_ms(100); //Delay for Debouncing
    return (int)voltage;
    
}

void initADC(void)
{
    //------Setup the ADC
    //DO: using ADCON0 set right justify
    ADCON0bits.FM = 1;
    //DO: Using ADCON0 set ADCRC Clock
    ADCON0bits.CS = 1; //ADCRC Clock
    //DO: Set RA0 to input
    TRISA = 0xFF; //Set RA0 to input
    //DO: Set RA0 to analog
    ANSELAbits.ANSELA0 = 1; //Set RA0 to analog
    ANSELAbits.ANSELA1 = 1; //Set RA1 to analog
    //DO: set ADC CLOCK Selection register to zero
    ADCLK = 0xFF;
    //DO: Clear ADC Result registers
    ADRESH = 0x00;
    ADRESL = 0x00;
    //DO: set precharge select to 0 in register ADPERL & ADPERH
    ADPREL = 0x00;
    ADPREH = 0x00;
    //DO: Set acquisition LOW and HIGH bytes to zero
    ADACQL = 0x00;
    ADACQH = 0x00;
    //DO: Turn ADC On on register ADCON0   
    ADCON0bits.ON = 1;
}

void initDisplay() {
    TRISD = 0x00;
}

void displayDigit(int digit) {
    DISPLAY_LAT = ~digitPatterns[digit];
}

void initOutputs(){
    TRISB = 0x01;
    ANSELB = 0x00;
}

void enableMotor() {
    LATBbits.LATB2 = 1;
    __delay_ms(1000);
    LATBbits.LATB2 = 0;
}

void enableBuzzer() {
    LATBbits.LATB3 = 1;
    __delay_ms(1000);
    LATBbits.LATB3 = 0;
}

void INTERRUPT_Initialize (void)
{
    // Enable interrupt priority bit in INTCON0 (check INTCON0 register and find the bit)
    INTCON0bits.IPEN = 1;
    // Enable high priority interrupts using bits in INTCON0
    INTCON0bits.GIEH = 1;
    // Enable low priority interrupts using bits in INTCON0
    INTCON0bits.GIEL = 1;
    // Interrupt on rising edge of INT0 pin using bits in INTCON0
    INTCON0bits.INT0EDG = 1;
    // Set the interrup high priority (IP) for INT0 - INT0IP
    IPR1bits.INT0IP = 1;
    // Enable the interrup (IE) for INT0
    PIE1bits.INT0IE = 1;

    //Clear interrupt flag for INT06
    PIR1bits.INT0IF = 0;
  
    // Change IVTBASE by doign the following
    // Set IVTBASEU to 0x00 
    IVTBASEU = 0x00;
    // Set IVTBASEH to  0x40; 
    IVTBASEH = 0x40;
    // Set IVTBASEL to 0x08; 
    IVTBASEL = 0x08;
}

void delay(int time){
    for (int i=0; i<time; i++){
        __delay_ms(1);
    }
}

// Defining Interrupt ISR 
void __interrupt(irq(IRQ_INT0),base(0x4008)) INT0_ISR(void)
{
        // Check if interrupt flag for INT0 is set to 1 - (note INT0 is your input)
    if(PIR1bits.INT0IF == 1){
        for(char i=0; i<30; i++){
            PORTBbits.RB3 = 1;
            delay(i);
            PORTBbits.RB3 = 0;
            delay(i);
        }
        PIR1bits.INT0IF = 0;
    }
}

#endif	/* FUNCTIONS_H */

