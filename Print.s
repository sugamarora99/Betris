; Print.s
; Student names: Sugam Arora
; Last modification date: change this to the last modification date or look very silly
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 

    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix

	PRESERVE8
    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB

  

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
dig EQU 0

LCD_OutDec
	CMP R0, #10
	BLO endcase
	
inductive
	MOV R2, #10
	UDIV R3, R0, R2
	MUL R2, R3, R2
	SUB R3, R0, R2 ; R3 = R0 % 10
	SUB SP, SP, #4 ; allocate space
	STR R3, [SP, #dig] ; put digit on the stack
	MOV R2, #10
	UDIV R0, R0, R2 ; N = N/10
	PUSH{R0, LR}
	BL LCD_OutDec
	POP{R0, LR}
	
	LDR R0, [SP, #dig] ; R0 = digit on top of stack
	ADD R0, R0, #0X30
	PUSH {R0, LR}
	BL ST7735_OutChar
	POP {R0, LR}
	ADD SP, SP, #4 ; deallocate space
	BX LR
	
	

endcase
	ADD R0, R0, #0X30
	PUSH{R0, LR}
	BL ST7735_OutChar
	POP{R0, LR}
	BX  LR
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.01, range 0.00 to 9.99
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.00 "
;       R0=3,    then output "0.03 "
;       R0=89,   then output "0.89 "
;       R0=123,  then output "1.23 "
;       R0=999,  then output "9.99 "
;       R0>999,  then output "*.** "
; Invariables: This function must not permanently modify registers R4 to R11
num EQU 4
LCD_OutFix
	CMP R0, #1000
	BHS printStars
	
	SUB SP, SP, #8
	STR R0, [SP, #num]
	MOV R1, #100
	UDIV R0, R0, R1
	ADD R0, R0, #0X30
	
	PUSH {R0, LR}
	BL ST7735_OutChar ; first digit
	POP{R0, LR}
	
	LDR R0, [SP, #num]
	MOV R2, #100
	UDIV R3, R0, R2
	MUL R2, R3, R2
	SUB R0, R0, R2 ; modulus
	STR R0, [SP, #num]
	
	
	MOV R0, #0X2E
	PUSH{R0, LR}
	BL ST7735_OutChar
	POP{R0, LR} ; decimal point
	
	LDR R0, [SP, #num]
	MOV R1, #10
	UDIV R0, R0, R1
	ADD R0, R0, #0X30
	
	PUSH{R0, LR}
	BL ST7735_OutChar ; second digit
	POP{R0, LR}
	
	LDR R0, [SP, #num]
	MOV R2, #10
	UDIV R3, R0, R2
	MUL R2, R3, R2
	SUB R0, R0, R2 ; moduluS, last digit
	ADD R0, R0, #0X30
	
	PUSH{R0, LR}
	BL ST7735_OutChar
	POP{R0, LR}
	B done
	
printStars
	MOV R0, #0X2A
	PUSH{R0, LR}
	BL ST7735_OutChar
	POP{R0, LR} ; *
	
	MOV R0, #0X2E
	PUSH{R0, LR}
	BL ST7735_OutChar
	POP{R0, LR} ; decimal point
	
	MOV R0, #0X2A
	PUSH{R0, LR}
	BL ST7735_OutChar
	POP{R0, LR} ; *
	
	PUSH{R0, LR}
	BL ST7735_OutChar
	POP{R0, LR} ; *
	
done	
	ADD SP, SP, #8
    BX   LR
 
    
     
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file
