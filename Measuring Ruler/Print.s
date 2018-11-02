; Print.s
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 
x 		EQU 4	
    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix

    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB

  

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutDec
	 ;SUB sp, #8		;make space for local var
	;MOV r11, sp
	;add sp, sp, #8
	MOV r3, #0
	MOV r1, #10		;store local var
	;STR r1, [r11,#x]
ODloop1
	mov r1,#10
	MOV r2, r0
	;ldr r1, [r11,#x]
	udiv r0, r0, r1
	MUL r1, r1, r0
	subs r1, r2, r1  ;gets remainder (digit) and pushes into stack
	push {r1,r4}
	ADD r3, #1
	CMP r0, #0
	BNE ODloop1
ODloop2	POP {r0,r4}
	ADD r0, #0x30
	PUSH {LR, r3}
	BL ST7735_OutChar 	; output character
	POP {LR,r3}
	SUBS r3, #1
	BNE ODloop2

      BX  LR
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000 "
;       R0=3,    then output "0.003 "
;       R0=89,   then output "0.089 "
;       R0=123,  then output "0.123 "
;       R0=9999, then output "9.999 "
;       R0>9999, then output "*.*** "
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutFix
	MOV r1, #9999
	CMP r0, r1
	BHI OFover
	MOV r1, #1000
	MOV r2, r0
	UDIV r0,r0, r1
	MUL r1,r1,r0
	SUBS r2, r2, r1
	ADD r0, #0x30
	PUSH {LR,r2}
	
	BL ST7735_OutChar
	MOV r0, #0x2e
	BL ST7735_OutChar
	POP {LR,r2}
	MOV r1, #100
	UDIV r0,r2,r1
	MUL r1,r1,r0
	SUBS r2,r2,r1
	ADD r0, #0x30
	PUSH {LR, r2}
	
	BL ST7735_OutChar
	POP {LR,r2}
	MOV r1, #10
	UDIV r0,r2,r1
	MUL r1,r1,r0
	SUBS r2,r2,r1
	ADD r0, #0x30
	
	PUSH {LR, r2}
	BL ST7735_OutChar
	POP {LR,r2}
	MOV r0,r2
	ADD r0, #0x30
	
	PUSH {LR, r2}
	BL ST7735_OutChar
	POP {LR,r2}
     BX   LR
	 
OFover	MOV r0, #0x2a
	PUSH {LR, r4}
	BL ST7735_OutChar
	POP {LR,r4}
	MOV r0, #0x2e
	PUSH {LR, r4}
	BL ST7735_OutChar
	POP {LR,r4}
	MOV r0, #0x2a
	PUSH {LR, r4}
	BL ST7735_OutChar
	POP {LR,r4}
	MOV r0, #0x2a
	PUSH {LR, r4}
	BL ST7735_OutChar
	POP {LR,r4}
	MOV r0, #0x2a
	PUSH {LR, r4}
	BL ST7735_OutChar
	POP {LR,r4}
	BX LR
     ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file
