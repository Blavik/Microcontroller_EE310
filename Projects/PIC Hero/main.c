/*
 * ---------------------
 * Title: PIC Hero
 * ---------------------
 * Program Details:
 *  My final project for EE 310 is a 2 note Rhythm game that uses an LCD, 2 Buttons,
 * 2 LEDs, and one Interrupt as control. The program includes start menu, game, and 
 * a score system that is shown after the song is over.
 * Inputs: RB0, RA3, RA4. External Interrupt and Buttons
 * Outputs: RD0-RD7 LCD Control, RB1 RB2 LCD Control, 
 *      RA2, Passive Buzzer, RA0, Green LED, RA1, Red LED
 * Date: 5 / 15 / 2024
 * File Dependencies / Libraries:<xc.h>, <stdio.h>, <stdlib.h>, <string.h>
 * Compiler: xc8, 2.46
 * Author: Bjorn Lavik
 * Versions:
 *      V1.0:   5/12/2024.      Got all GPIO Functioning independently
 *      V1.1    5/15/2024.      Finished Project and Code
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
//#include "C:\Program Files\Microchip\xc8\v2.40\pic\include\proc\pic18f46k42"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define _XTAL_FREQ 4000000                 // Fosc  frequency for _delay()  library
#define FCY    _XTAL_FREQ/4

#define RS LATB1                   /* PORTD 0 pin is used for Register Select */
#define EN LATB2                   /* PORTD 1 pin is used for Enable */
#define ldata LATD                 /* PORTB is used for transmitting data to LCD */

#define Green LATA0
#define Red LATA1
#define Piezo1 LATA2

#define Button1 PORTAbits.RA3
#define Button2 PORTAbits.RA4

#define LCD_Port TRISD              
#define LCD_Control TRISB

char Game = 0;

char Note1Set = 0;
char Note2Set = 0;

char Note1Hit = 0;
char Note2Hit = 0;

float Score = 0;
char ScoreStr[5];

// Define the note arrays for each row
char notes_row1[64] = { '#', '#', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                        '#', '#', ' ', ' ', '#', '#', '#', '#', ' ', '#', '#', ' ', '#', '#', ' ', '#',
                        '#', ' ', '#', '#', ' ', ' ', ' ', ' ', '#', '#', ' ', '#', ' ', '#', ' ', ' ',
                        '#', ' ', ' ', '#', '#', ' ', ' ', '#', '#', ' ', '#', ' ', ' ', '#', ' ', '#'};

char notes_row2[64] = { ' ', '#', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
                        '#', '#', '#', '#', ' ', '#', ' ', ' ', '#', '#', ' ', '#', '#', '#', ' ', '#',
                        ' ', '#', '#', ' ', '#', '#', ' ', '#', '#', ' ', '#', ' ', '#', ' ', '#', ' ',
                        '#', ' ', '#', '#', '#', ' ', ' ', ' ', ' ', '#', ' ', '#', ' ', '#', ' ', '#'};

void LCD_Init(void);
void LCD_Command(char );
void LCD_Char(char x);
void LCD_Clear(void);
void LCD_String(const char *);
void LCD_String_xy(char ,char ,const char*);
void INTERRUPT_Initialize(void);
void Note1(void);
void Note2(void);

// Defining Interrupt ISR 
void __interrupt(irq(IRQ_INT0),base(0x4008)) INT0_ISR(void)
{
        // Check if interrupt flag for INT0 is set to 1 - (note INT0 is your input)
    if(PIR1bits.INT0IF == 1){
        Game = 1;
        for(char i=0; i<25; i++){
            Piezo1 = 1;
            __delay_ms(3);
            Piezo1 = 0;
            __delay_ms(3);
        }
        for(char i=0; i<25; i++){
            Piezo1 = 1;
            __delay_ms(2);
            Piezo1 = 0;
            __delay_ms(2);
        }
        for(char i=0; i<25; i++){
            Piezo1 = 1;
            __delay_ms(1);
            Piezo1 = 0;
            __delay_ms(1);
        }
        
        PIR1bits.INT0IF = 0;
    }
}

/*****************************Main Program*******************************/

void main(void)
{
    LCD_Init();
    INTERRUPT_Initialize();
    LCD_Clear();
    Red = 0;
    Green = 0;
    Game = 0;
    while(Game == 0){
        LCD_String_xy(1,0,"PIC HERO! Fl ip");
        LCD_String_xy(2,0,"Switch to St art");
    }
    
    LCD_Clear();
    while(Game){
    
        for (int i = 63; i >= 0; i--) {
        // Display notes for row 1
            Note1Hit = 0;
            Note2Hit = 0;
            Green = 0;
            for (int j = 0; j < 16; j++) {
                LCD_String_xy(1, j, notes_row1[(i + j) % 64] == '#' ? "#" : " ");
                LCD_String_xy(2, j, notes_row2[(i + j) % 64] == '#' ? "#" : " ");
                
                if(j==12){
                    if(notes_row1[(i + j) % 64] == '#'){
                        Note1Set = 1;
                    } else{
                        Note1Set = 0;
                    }
                    if(notes_row2[(i + j) % 64] == '#'){
                        Note2Set = 1;
                    } else{
                        Note2Set = 0;
                    }
                }
            }
            // Delay for a short duration
            for (int j = 0; j<=3; j++){
                if((Button1 == 1)&& Note1Set){
                    Note1();
                    Red = 0;
                    Green = 1;
                    Note1Hit = 1;
                    Score = Score + 1;
                } else{
                __delay_ms(20);
                }
                if((Button2 == 1)&& Note2Set){
                    Note2();
                    Red = 0;
                    Green = 1;
                    Note2Hit = 1;
                    Score = Score + 1;
                } else{
                __delay_ms(20);
                }
            }
            if((Note1Set == 0 && Button1 == 0) || (Note2Set == 0 && Button2 == 0)){
                Red = 0;
            }
            if((Note1Set && (Note1Hit == 0)) || (Note2Set && (Note2Hit == 0))
                || (Note1Set == 0) && Button1 || (Note2Set == 0) && Button2){
                    Red = 1;
                    if (Score > 1){
                        Score = Score - 1;
                    }
            }
            
            
        }
        Game = 0;
        
    }
    sprintf(ScoreStr,"%.1f",Score);
        while(Game == 0){
            LCD_String_xy(1,0,"Song Over, Y our");
            LCD_String_xy(2,0,"Score was");
            LCD_String_xy(2,10,ScoreStr);
    }
}
             

/****************************Functions********************************/
void LCD_Init()
{
    TRISA = 0b00011000;
    ANSELA = 0x00;
    __delay_ms(20);           /* 20ms,16x2 LCD Power on delay */
    LCD_Port = 0x00;       /* Set PORTB as output PORT for LCD data(D0-D7) pins */
    LCD_Control = 0x01;    /* Set PORTD as output PORT LCD Control(RS,EN) Pins */
    LCD_Command(0x01);     /* clear display screen */
    LCD_Command(0x38);     /* uses 2 line and initialize 5*7 matrix of LCD */
    LCD_Command(0x0c);     /* display on cursor off */
    LCD_Command(0x06);     /* increment cursor (shift cursor to right) */
}

void LCD_Clear()
{
        LCD_Command(0x01); /* clear display screen */
}

void LCD_Command(char cmd )
{
    ldata= cmd;            /* Send data to PORT as a command for LCD */   
    RS = 0;                /* Command Register is selected */
    EN = 1;                /* High-to-Low pulse on Enable pin to latch data */ 
    NOP();
    EN = 0;
    __delay_ms(3); 
}

void LCD_Char(char dat)
{
    ldata= dat;            /* Send data to LCD */  
    RS = 1;                /* Data Register is selected */
    EN=1;                  /* High-to-Low pulse on Enable pin to latch data */   
    NOP();
    EN=0;
    __delay_ms(1);
}


void LCD_String(const char *msg)
{
    while((*msg)!=0)
    {       
      LCD_Char(*msg);
      msg++;    
        }
}

void LCD_String_xy(char row,char pos,const char *msg)
{
    char location=0;
    if(row<=1)
    {
        location=(0x80) | ((pos) & 0x0f); /*Print message on 1st row and desired location*/
        LCD_Command(location);
    }
    else
    {
        location=(0xC0) | ((pos) & 0x0f); /*Print message on 2nd row and desired location*/
        LCD_Command(location);    
    }  
    LCD_String(msg);

}

void INTERRUPT_Initialize (void)
{
    ANSELB = 0x00;
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

void Note1(void){
    for(char i=0; i<=10; i++){
            Piezo1 = 1;
            __delay_ms(1);
            Piezo1 = 0;
            __delay_ms(1);
    }
}

void Note2(void){
    for(char i=0; i<=5; i++){
        Piezo1 = 1;
        __delay_ms(2);
        Piezo1 = 0;
        __delay_ms(2);
    }
}
