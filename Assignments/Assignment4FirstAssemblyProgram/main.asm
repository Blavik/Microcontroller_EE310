//-----------------------------
// Title: Temp System
//-----------------------------
// Purpose: To control heater and cooler based on measured temp and desired reference temp
// Dependencies: None
// Compiler: MPLAB X IDE v6.00
// Author: Bjorn Lavik
// OUTPUTS: RD1 (Heater), RD2 (Cooler)
// INPUTS: measuredTempInput, refTempInput
// Versions:
//  	V1.0: 3/4/2024 - First version, I focused on definitions
//  	V1.1: 3/5/2024 - Second version, debugged all syntax errors and finished program
//	V1.2: 3/6/2024 - Third version, cleaned up code and finished comments
//-----------------------------

#include "./myConfigFile.inc"
#include <xc.inc>

;----------------------------------
; REGISTER AND CONSTANT DEFINITIONS
;----------------------------------
measuredTempInput   EQU     0 ; Measured temperature input
refTempInput        EQU     15 ; Reference temperature input

measuredTempREG     EQU     0x20 ; Register for measured temperature
refTempREG          EQU     0x21 ; Register for reference temperature
contREG		    EQU     0x22 ; Control register
portTEMP	    EQU	    0x23 ; Temp reg for debugging PORTD
	 
numREG		    EQU	    0x30 ; Numerator for dec conversion
quoREG		    EQU	    0x31 ; Quotient for dec conversion
measuredTempDecU    EQU     0x70 ; Decimal equivalent of measured temperature
measuredTempDecH    EQU     0x71
measuredTempDecL    EQU     0x72
refTempDecU         EQU     0x60 ; Decimal equivalent of reference temperature
refTempDecH         EQU     0x61
refTempDecL         EQU     0x62
denom		    EQU	    0x0A ; Denominator for dec conversion
measuredTempComREG  EQU	    0x32

		    
;--------------
; Main Program
;--------------
PSECT absdata,abs,ovrld        ; Do not change
   
	ORG	0x00		;Reset vector
	GOTO	START

	ORG	0x0020		;Begin assembly at 0020H
START:	

    MOVLW   0x00;
    MOVWF   TRISD, 0;	set PORTD 1 and 2 bits to Output
    
;---------------------------------
; Compare measuredTemp and refTemp
;---------------------------------
    
    ; Load ref and measured temps into registers
    MOVLW   measuredTempInput
    MOVWF   measuredTempREG,	0
    MOVLW   refTempInput
    MOVWF   refTempREG,   0
    
    ; Compare measuredTemp and refTemp, branch based on difference
    MOVFF    refTempREG, WREG	; W = refTemp
    SUBWF   measuredTempREG, 0, 0   ; W = measuredTemp - refTemp
    BZ	temp_equal  ; measuredTemp = refTemp
    BN	temp_neg    ; measuredTemp < refTemp
    BRA	temp_pos    ; measuredTemp > refTemp
    
;-----------------------
; measuredTemp = refTemp
;-----------------------
    
temp_equal:		; No heating or cooling
    MOVLW   0x00
    MOVWF   contREG, 0  ; contREG = 0
    MOVWF   portTEMP, 0
    MOVFF   portTEMP, PORTD ; PORTD = 0x00
    GOTO    decimal_convert

;-----------------------
; measuredTemp < refTemp
;-----------------------

temp_neg:		; Yes heating no cooling
    MOVLW   0x01
    MOVWF   contREG, 0	; contREG = 1
    RLNCF   contREG, 0, 0
    MOVWF   portTEMP, 0
    MOVFF   portTEMP, PORTD ; PORTD = 0x02
    GOTO    decimal_convert
    
;-----------------------
; measuredTemp > refTemp
;-----------------------
    
temp_pos:		; No heating yes cooling
    MOVLW   0x2
    MOVWF   contREG, 0	; contREG = 2
    RLNCF   contREG, 0, 0
    MOVWF   portTEMP, 0
    MOVFF   portTEMP, PORTD ; PORTD = 0x04
    GOTO    decimal_convert
    
;----------------------------------
; refTemp Hex to Decimal Conversion
;----------------------------------
    
decimal_convert:
    ;REF TEMP
    MOVFF   refTempREG, numREG	; numREG = refTemp
    MOVLW   denom
    CLRF    quoREG, 0
    BRA	    rd1
rd1:
    INCF    quoREG, 1, 0
    SUBWF   numREG, 1, 0
    BC	    rd1
    ADDWF   numREG
    DECF    quoREG, 1, 0
    MOVFF   numREG, refTempDecL
    MOVFF   quoREG, numREG
    CLRF    quoREG, 0
    BRA	    rd2
rd2:
    INCF    quoREG, 1, 0
    SUBWF   numREG, 1, 0
    BC	    rd2
    ADDWF   numREG
    DECF    quoREG, 1, 0
    MOVFF   numREG, refTempDecH
    
;---------------------------------------
; measuredTemp Hex to Decimal Conversion
;---------------------------------------
    
    ;check if measured temp is negative
    MOVLW   0x00
    ADDWF   measuredTempREG
    BNN	    nonsign ; If measuered is non-signed continue as normal
    MOVFF   measuredTempREG, measuredTempComREG ; Else complement measuredTemp
    NEGF    measuredTempComREG, 0
    MOVFF   measuredTempComREG, numREG	; numREG = 1's complement of measuredTemp
    BRA	    mconv
    
    
nonsign:
    MOVFF   measuredTempREG, numREG	; numREG = measuredTemp
    BRA	    mconv
    
mconv:
    MOVLW   denom
    CLRF    quoREG, 0
    BRA	    md1
md1:
    INCF    quoREG, 1, 0
    SUBWF   numREG, 1, 0
    BC	    md1
    ADDWF   numREG
    DECF    quoREG, 1, 0
    MOVFF   numREG, measuredTempDecL
    MOVFF   quoREG, numREG
    CLRF    quoREG, 0
    BRA	    md2
md2:
    INCF    quoREG, 1, 0
    SUBWF   numREG, 1, 0
    BC	    md2
    ADDWF   numREG
    DECF    quoREG, 1, 0
    MOVFF   numREG, measuredTempDecH
 
;---------------
; End of program
;---------------

END
