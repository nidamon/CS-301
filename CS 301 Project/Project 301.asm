; Project 301.asm
; Nathan Damon
; 10/3/2020
; This file holds all of the assembly code for Project 301

section .text
bits 64

;input argument is in rcx
;  output value is in rax

; =======================================================================================================
global YearlengthSet

YearlengthSet:
	movss [Yearlength], xmm0 ; set the year length
	ret

; ================================================
global SeasonOfYear

SeasonOfYear:
	addss xmm0, [TimeofYear]
	movss [TimeofYear], xmm0 ; increase the time based on elapsed time
	ucomiss xmm0, [Yearlength] ; is it the next year?
	jb .is_spring
	movss xmm0, [float_0] 
	movss [TimeofYear], xmm0; reset the year
	mov rax, 1 ; 1 = spring
	ret ; special case of new year, it will be spring
	
	.is_spring:
		movss xmm1, [Yearlength]
		mulss xmm1, [OneForth]
		ucomiss xmm0, xmm1
		jae .is_summer
		mov rax, 1 ; 1 = spring
		ret
	
	.is_summer:
		movss xmm1, [Yearlength]
		mulss xmm1, [OneHalf]
		ucomiss xmm0, xmm1
		jae .is_fall
		mov rax, 2 ; 2 = summer
		ret
		
	.is_fall:
		movss xmm1, [Yearlength]
		mulss xmm1, [ThreeQuarters]
		ucomiss xmm0, xmm1
		jae .is_winter
		mov rax, 3 ; 3 = fall
		ret
		
	.is_winter:
		mov rax, 4 ;4 = winter
		ret

; =======================================================================================================
global wants_to_eat

wants_to_eat:
	movss xmm1, [float_variable1]
	movss xmm2, [float_variable2]
	movss xmm0, xmm1
	divss xmm0, xmm2     ; gets % fullness

	ucomiss xmm0, [percent100]
	jb lessthan100percent
	mov rax, -1
	ret

	lessthan100percent:
	ucomiss xmm0, [percent70]     ; if above 70% don't care for food
	jb lessthan70percent
	mov rax, 0
	ret

	lessthan70percent:  
	ucomiss xmm0, [percent45]        ; if between 70% and 45% eat when near food
	jb lessthan45percent
	mov rax, 1
	ret		
					
	lessthan45percent:  ; if below 45% eat
	mov rax, 2
	ret	


; =======================================================================================================            .labels!!!!!!!!!!! YAY !!!!!!!!!!!!!!!
; VARIABLE SETTING FUNCTIONS
; =======================================================================================================


; INTS ; ======================================================================================================= ; INTS ;

global set_all_function_use_variables_to_0
set_all_function_use_variables_to_0:
	mov rax, 0
	mov [int_variable1], rax
	mov [int_variable2], rax
	mov [int_variable3], rax
	mov [int_variable4], rax
	movss xmm0, [float_0]
	movss [float_variable1], xmm0
	movss [float_variable2], xmm0
	movss [float_variable3], xmm0
	movss [float_variable4], xmm0
	ret


global set_int_variable1
set_int_variable1:
	mov [int_variable1], rcx
	mov rax, rcx
	ret


global set_int_variable2
set_int_variable2:
	mov [int_variable2], rcx
	mov rax, rcx
	ret


global set_int_variable3
set_int_variable3:
	mov [int_variable3], rcx
	mov rax, rcx
	ret


global set_int_variable4
set_int_variable4:
	mov [int_variable4], rcx
	mov rax, rcx
	ret


; FLOATS ; ======================================================================================================= ; FLOATS ;

;global float_create
;float_create:


global set_float_variable1
set_float_variable1:
	cvtsi2ss xmm0, rcx
	movss [float_variable1], xmm0
	ret


global set_float_variable2
set_float_variable2:
	cvtsi2ss xmm0, rcx
	movss [float_variable2], xmm0
	ret


global set_float_variable3
set_float_variable3:
	cvtsi2ss xmm0, rcx
	movss [float_variable3], xmm0
	ret

global set_float_variable4
set_float_variable4:
	cvtsi2ss xmm0, rcx
	movss [float_variable4], xmm0
	ret


; DATA ; ======================================================================================================= ; DATA ;


section .data       ; Storing all variables and related
int_variable1: dd 0
int_variable2: dd 0
int_variable3: dd 0
int_variable4: dd 0

float_variable1: dd 0.0
float_variable2: dd 0.0
float_variable3: dd 0.0
float_variable4: dd 0.0
float_0: dd 0.0
float_1: dd 1.0     ; Didn't know how to get float input
float_10: dd 10.0      ; so i take rcx int and create a float version
float_100: dd 100.0
float_1000: dd 1000.0

;      For use with wants_to_eat
percent100: dd 1.0
percent70: dd 0.7
percent45: dd 0.45	

;     For use with SeasonOfYear
OneForth: dd 0.25
OneHalf: dd 0.5
ThreeQuarters: dd 0.75
Yearlength: dd 60.0 ; in Seconds 
TimeofYear: dd 0.0  ; Changed based on elapsed time