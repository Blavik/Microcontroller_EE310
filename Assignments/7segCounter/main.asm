//-----------------------------
// Title: 7 Segment Counter
//-----------------------------
// Purpose: To increment/decrement hex value and display on 7 segment display
// Dependencies: None
// Compiler: MPLAB X IDE v6.00
// Author: Bjorn Lavik
// OUTPUTS: RD6-RD0, 7 segment display segments
// INPUTS: Switch1, Switch2. Decrement and Increment Buttons
// Versions:
//  	V1.0: 3/25/2024 - First version, I focused on definitions and logic
//  	V1.1: 3/26/2024 - Second version, debugged inderect addressing and
//			 finished program.
//-----------------------------

#include "./myConfigFile.inc"
#include <xc.inc>

;----------------------------------
; INPUT AND CONSTANT DEFINITIONS
;----------------------------------

#define Switch1	    PORTB, 1 //Switch 1 Decrement Count
#define Switch2	    PORTB, 2 //Switch 2 Increment Count
#define	DispREG	    PORTD
	
SwitchREG   EQU	    0x20    //Switch 1 And 2 State Register
CountREG    EQU	    0x21    //Count Register
//DispREG	    EQU	    0x22    //Seven Segment Display Register
	    
DelayOuter  EQU	    0x02	    //Outer Delay Loop Count
DelayInner  EQU	    0x02    //Inner Delay Loop Count

DelayOuterREG	EQU 0x30    //Outer Delay Loop Register
DelayInnerREG	EQU 0x31    //Inner Delay Loop Register

//Registers for holding coded values
dREG0	    EQU	    0x00
dREG1	    EQU	    0x01
dREG2	    EQU	    0x02
dREG3	    EQU	    0x03
dREG4	    EQU	    0x04
dREG5	    EQU	    0x05
dREG6	    EQU	    0x06
dREG7	    EQU	    0x07
dREG8	    EQU	    0x08
dREG9	    EQU	    0x09
dREGA	    EQU	    0x0A
dREGB	    EQU	    0x0B
dREGC	    EQU	    0x0C
dREGD	    EQU	    0x0D
dREGE	    EQU	    0x0E
dREGF	    EQU	    0x0F
	    
Disp0	    EQU	    0b11000000
Disp1	    EQU	    0b11111001
Disp2	    EQU	    0b10100100
Disp3	    EQU	    0b10110000
Disp4	    EQU	    0b10011001
Disp5	    EQU	    0b10010010
Disp6	    EQU	    0b10000010
Disp7	    EQU	    0b11111000
Disp8	    EQU	    0b10000000
Disp9	    EQU	    0b10010000
DispA	    EQU	    0b10001000
DispB	    EQU	    0b10000011
DispC	    EQU	    0b11000110
DispD	    EQU	    0b10100001
DispE	    EQU	    0b10000110
DispF	    EQU	    0b10001110
		    
;--------------
; Main Program
;--------------
PSECT absdata,abs,ovrld        ; Do not change
   
	ORG	0x00		;Reset vector
	GOTO	initialization

	ORG	0x0020		;Begin assembly at 0020H

//Initialize Ports and coded didgits
initialization:
    CALL    setupPortD
    CALL    setupPortB
    CALL    initREG
    MOVLW   0x00
    MOVWF   CountREG, 0

main:
    
//Read switch 1 and set/clear SwitchREG, 0
switch1:
    BTFSC   Switch1
    BRA	    set1
    BRA	    clear1
set1:
    BSF	    SwitchREG,0
    BRA	    switch2
clear1:
    BCF	    SwitchREG,0
    BRA	    switch2
   
//Read switch 2 and set/clear SwitchREG, 1
switch2:
    BTFSC   Switch2
    BRA	    set2
    BRA	    clear2
set2:
    BSF	    SwitchREG,1
    BRA	    controlLogic
clear2:
    BCF	    SwitchREG,1
    BRA	    controlLogic
    
//Control Logic based on Switch readings
controlLogic:
    MOVLW   0x00
    XORWF   SwitchREG, 0
    BZ	    doNothing
    MOVLW   0x01
    XORWF   SwitchREG, 0
    BZ	    decrementDisplay
    MOVLW   0x02
    XORWF   SwitchREG, 0
    BZ	    incrementDisplay
    MOVLW   0x03
    XORWF   SwitchREG, 0
    BZ	    resetDisplay
    BRA	    delayLoop
    
doNothing:
    GOTO    displayControl
   
decrementDisplay:
    DECF    CountREG, 1
    GOTO    displayControl
    
incrementDisplay:
    INCF    CountREG, 1
    GOTO    displayControl
    
resetDisplay:
    MOVLW   0x00
    MOVWF   CountREG, 0
    GOTO    displayControl
    
displayControl:
    MOVLW   0x0F
    ANDWF   CountREG, 0, 0
    MOVWF   FSR0L
    MOVFF   INDF0,   DispREG
    BANKSEL	LATD ;Data Latch
    MOVFF   DispREG,	LATD
    

// Delay Subroutine
delayLoop:
    MOVLW   DelayOuter
    MOVWF   DelayOuterREG
    MOVLW   DelayInner
    MOVWF   DelayInnerREG
loop1:
    DECF    DelayInnerREG, 1
    NOP	    //A few NOPs to tune delay
    NOP
    NOP
    BNZ	    loop1
    MOVLW   DelayInner
    MOVWF   DelayInnerREG
    DECF    DelayOuterREG, 1
    BNZ	    loop1
    GOTO    main
    
    
// PortD and PortB Configuration
setupPortD:
    BANKSEL	PORTD ;
    CLRF	PORTD ;Init PORTD
    BANKSEL	LATD ;Data Latch
    CLRF	LATD ;
    BANKSEL	ANSELD ;
    CLRF	ANSELD ;digital I/O
    BANKSEL	TRISD ;
    MOVLW	0x00 ;Set RD7 as an input
    MOVWF	TRISD ;and set [RD6:RD0] as ouputs
    RETURN
    
setupPortB:
    BANKSEL	PORTB ;
    CLRF	PORTB ;Init PORTB
    BANKSEL	LATB ;Data Latch
    CLRF	LATB ;
    BANKSEL	ANSELB ;
    CLRF	ANSELB ;digital I/O
    BANKSEL	TRISB ;
    MOVLW	0xFF ;Set RB1 and RB2 as inputs
    MOVWF	TRISB
    RETURN    

//Move coded didgits into registers dREG0 - dREGF
initREG:
    MOVLW   Disp0
    MOVWF   dREG0, 0
    MOVLW   Disp1
    MOVWF   dREG1, 0
    MOVLW   Disp2
    MOVWF   dREG2, 0
    MOVLW   Disp3
    MOVWF   dREG3, 0
    MOVLW   Disp4
    MOVWF   dREG4, 0
    MOVLW   Disp5
    MOVWF   dREG5, 0
    MOVLW   Disp6
    MOVWF   dREG6, 0
    MOVLW   Disp7
    MOVWF   dREG7, 0
    MOVLW   Disp8
    MOVWF   dREG8, 0
    MOVLW   Disp9
    MOVWF   dREG9, 0
    MOVLW   DispA
    MOVWF   dREGA, 0
    MOVLW   DispB
    MOVWF   dREGB, 0
    MOVLW   DispC
    MOVWF   dREGC, 0
    MOVLW   DispD
    MOVWF   dREGD, 0
    MOVLW   DispE
    MOVWF   dREGE, 0
    MOVLW   DispF
    MOVWF   dREGF, 0
    RETURN
    
END
