; Project 301.asm
; Nathan Damon
; 10/3/2020
; This file holds all of the assembly code for Project 301

section .text
bits 64

; input argument is in (rcx/rax, rdx)
; input for float is in xmm0
; output value is in rax/xmm0

; =======================================================================================================
Global YearlengthSet

YearlengthSet:
	movss [Yearlength], xmm0 ; set the year length
	ret

; ================================================
Global SeasonOfYear

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
Global wants_to_eat

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

; =======================================================================================================
Global BoolGridModify

BoolGridModify:
	mov rsi, 0x8000000000000000 ; put a 1 at the far right  //side note: placing one 1, places a 1, 2, 4, or 8 in the byte
	jmp .loopbegin

	.loopstart
		sub rcx, 1
		shr rsi, 1  ; how far to the right does the 1 move (x-value)
	.loopbegin
		cmp rcx, 0
		jg .loopstart
	
	mov rdi, rdx  ; specify the y value
	mov rax, QWORD [BoolGrid + 8*rdi]

	xor rax, rsi
	mov QWORD [BoolGrid + 8*rdi], rax
	ret

; ================================================
Global BoolGridGet

BoolGridGet:
	mov rsi, 0x8000000000000000 ; put a 1 at the far right  //side note: placing one 1, use 1, 2, 4, 8
	jmp .loopbegin

	.loopstart
		sub rcx, 1
		shr rsi, 1  ; how far to the right does the 1 move (x-value)
	.loopbegin
		cmp rcx, 0
		jg .loopstart


	mov rdi, rdx  ; specify the y value
	mov rax, QWORD [BoolGrid + 8*rdi]

	and rax, rsi
	cmp rax, 0
	jne .collision
	mov rax, 0  ; no collision with another 1
	ret

	.collision:
		mov rax, 1  ; collided with a 1
		ret

; =======================================================================================================
Global DimensionsSet

DimensionsSet:  ;  Sets the height and width as well as some position checking constants for the path finding code
	mov QWORD [Width], rcx
	mov QWORD [Height], rdx

	mov rax, 0
	sub rax, rcx
	mov QWORD [PositionCheckNorth], rax
	mov QWORD [PositionCheckSouth], rcx

	mov rax, 0
	sub rax, rcx
	sub rax, 1
	mov QWORD [PositionCheckNorthWest], rax

	add rax, 2
	mov QWORD [PositionCheckNorthEast], rax


	mov rax, 0
	add rax, rcx
	sub rax, 1
	mov QWORD [PositionCheckSouthWest], rax

	add rax, 2
	mov QWORD [PositionCheckSouthEast], rax
	ret

; ================================================
Global CharSet

CharSet:
	mov BYTE [Char], cl
	movsx rax, cl
	ret

; ================================================
Global CharGridModify

CharGridModify:
	imul rdx, [Width] ; y value times the width
	add rdx, rcx ; add the position in the row

	mov rax, [Width]
	imul rax, [Height]

	cmp rdx, rax
	jl .continue
	mov rax, rdx
	ret

	.continue:
		mov cl, BYTE [Char]

		mov BYTE [CharGrid + rdx], cl ; move the preset char into the grid
		mov rax, rcx
		ret

; ================================================
Global CharGridGet

CharGridGet:
	imul rdx, [Width] ; y value times the width
	add rdx, rcx ; add the position in the row
	
	mov rax, [Width]
	imul rax, [Height]

	cmp rdx, rax
	jl .continue
	mov rax, rdx
	ret

	.continue:
		mov cl, BYTE [CharGrid + rdx]
		mov rax, rcx
		ret

; =======================================================================================================
Global CharOverlayGridModify

CharOverlayGridModify:
	imul rdx, [Width] ; y value times the width
	add rdx, rcx ; add the position in the row

	mov rax, [Width]
	imul rax, [Height]

	cmp rdx, rax
	jl .continue
	mov rax, rdx
	ret

	.continue:
		mov cl, BYTE [Char]

		mov BYTE [CharOverlayGrid + rdx], cl ; move the preset char into the grid
		mov rax, rcx
		ret

; ================================================
Global CharOverlayGridGet

CharOverlayGridGet:
	imul rdx, [Width] ; y value times the width
	add rdx, rcx ; add the position in the row
	
	mov rax, [Width]
	imul rax, [Height]

	cmp rdx, rax
	jl .continue
	mov rax, rdx
	ret

	.continue:
		mov cl, BYTE [CharOverlayGrid + rdx]
		mov rax, rcx
		ret

; =======================================================================================================	
Global IntegralGridResetFrom ; from (rcx(x), rdx(y)) to (width, height)

IntegralGridResetFrom: ; Resets the integral image starting from the given coordinates

	mov QWORD [IntegralGridResetFromXPosition], rcx ; Setting the left starting border

	; rdx is inputed y
	mov r8, rdx ; Keeps track of the array element index
	imul r8, [Width]
	add r8, rcx

	mov rsi, 0x8000000000000000  ; put a 1 at the far right

	push rcx
		mov rcx, QWORD [IntegralGridResetFromXPosition]
		ror rsi, cl
	pop rcx

	.loops:
	jmp .beginloop1

	.loop1start:
		add rdx, 1 ; Increment by 1 (y position)
		add r8, QWORD [IntegralGridResetFromXPosition]
		add r8, 1
	.beginloop1:
		mov rcx, rsi  ; Start at the ResetFromXPosition on the new line
		mov r11, QWORD [IntegralGridResetFromXPosition]
		jmp .beginloop2

		.loop2start:
			ror rcx, 1 ; Increment by 1 (x position)
			add r8, 1 ; increment index by 1
			add r11, 1
		.beginloop2:
			mov rax, QWORD [BoolGrid + 8*rdx] ; Get the row
			and rax, rcx



			mov r9, 0
			cmp rax, 0 ; if (rax > 0) -> there is a bit that came through the AND and thus solid
			je .nocollision

			add r9, 1 ; In the case of collision add 1
			;mov rax, r8
			;ret

			.nocollision:    ; Add to the integral image
				cmp r8, 0
				je .Setit  ;  The first element will not have any thing to add to itself				
				
				cmp rdx, 0
				je .onlyleftvalue  ;  We do not have a row above to take a sum from

				mov r10, r8
				sub r10, [Width]
				add r9w, WORD [ShortIntegralGrid + 2*r10] ; Add the sum of values above and to the left

				.onlyleftvalue:
					cmp r11, 0
					je .Setit  ;  The first element in the row will not have any thing to add to itself from the left

					mov r10, r8
					sub r10, 1
					add r9w, WORD [ShortIntegralGrid + 2*r10] ; Add the sum of values to the left and above

				cmp rdx, 0
				je .Setit  ;  We do not have a row above to take a sum from
				cmp r11, 0
				je .Setit  ;  The first element in the row will not have any thing to add to itself from the left

				mov r10, r8
				sub r10, [Width]
				sub r10, 1
				cmp r10, 0
				jl .bad1

				sub r9w, WORD [ShortIntegralGrid + 2*r10] ; subtract the sum of values that were counted twice

				.Setit:
					cmp r8, 0
					jl .bad
					cmp r8, 4095 ; 64*64 limit
					jg .bad

					mov WORD [ShortIntegralGrid + 2*r8], r9w ; Set the cell to the new calculated sum

					cmp r11, 63 ; rcx is at the end of the row -> 0x0000000000000001
					jne .loop2start
				
					cmp rdx, [Height]  ; Check if this was the last row
					jl .loop1start

					mov rax, -1
					ret

					.bad:
						mov rax, -33333
						ret	
					.bad1:
						mov rax, -55555
						ret	
; ===============================================================
Global IntegralGridGet

IntegralGridGet:
	imul rdx, [Width] ; y value times the width
	add rdx, rcx ; add the position in the row
	
	mov rax, [Width]
	imul rax, [Height]

	cmp rdx, rax
	jl .continue
	mov rax, rdx
	ret

	.continue:
		mov cx, WORD [ShortIntegralGrid + 2*rdx]
		mov rax, rcx
		ret

; ===============================================================
Global IntegralGridAreaSumGet  ; Gets the sum of the solid objects in the bool grid
	;  Will not be able to check sum of top row and/or left column

IntegralGridAreaSumGet:  ;  (x1, y1) -> (rcx, rdx) --- (x2, y2) -> (r8, r9)
	
	mov rdi, r8
	sub rdi, rcx ; Get the width difference
	mov rsi, r9
	sub rsi, rdx ; Get the height difference

	mov r10, 0 ; Determines how the sum is calculated with the corners

	cmp rdi, 0
	jl .negwidth
	jmp .poswidth

	.negwidth:
		cmp rsi, 0
		jl .negheightNW  ; NW -> negative width
		jmp .posheightNW

		.negheightNW:
			; ecx and edx unchanged
			sub r8d, 1
			sub r9d, 1
			mov r10, 1
			jmp .xleftckeckingloop

		.posheightNW:
			; ecx and r9d unchanged
			sub r8d, 1
			sub edx, 1
			mov r10, 2
			jmp .xleftckeckingloop

	.poswidth:
		cmp rsi, 0
		jl .negheightPW ; PW -> positive width
		jmp .posheightPW
		
		.negheightPW:
			; r8d and edx unchanged
			sub ecx, 1
			sub r9d, 1
			mov r10, 3
			jmp .xleftckeckingloop

		.posheightPW:
			; r8d and r9d unchanged
			sub ecx, 1
			sub edx, 1
			mov r10, 4
			jmp .xleftckeckingloop


	.xleftckeckingloop: ; Moves the start coord to onto the map if off the left side
		cmp ecx, -1
		jg .xrightcheckingloop
		add ecx, 1
		jmp .xleftckeckingloop

	.xrightcheckingloop: ; Moves the start coord to onto the map if off the right side
		cmp ecx, [Width]
		jl .ytopcheckingloop
		sub ecx, 1
		jmp .xrightcheckingloop

	.ytopcheckingloop: ; Moves the start coord to onto the map if off the top
		cmp edx, -1
		jg .ybottomcheckingloop
		add edx, 1
		jmp .ytopcheckingloop

	.ybottomcheckingloop: ; Moves the start coord to onto the map if off the bottom
		cmp edx, [Height]
		jl .retrievesum     ; Was .widthleftcheckingloop  but found that the width checking was not needed and was ruining results
		sub edx, 1
		jmp .ybottomcheckingloop

	.retrievesum:
		mov rdi, 0
		cmp r10, 2
		jg .r10is34
		jmp .r10is12

		.r10is12:
			cmp r10, 2
			jl .r10is1
			jmp .r10is2

			.r10is1:
				mov r11, rdx
				imul r11, [Width]
				add r11, rcx
				mov di, WORD [ShortIntegralGrid + 2*r11] ; Bottom right corner

				mov r11, r9
				imul r11, [Width]
				add r11, r8
				add di, WORD [ShortIntegralGrid + 2*r11] ; Top left corner
				mov rax, rdi

				mov r11, rdx
				imul r11, [Width]
				add r11, r8
				mov di, WORD [ShortIntegralGrid + 2*r11] ; Bottom left corner

				mov r11, r9
				imul r11, [Width]
				add r11, rcx
				add di, WORD [ShortIntegralGrid + 2*r11] ; Top right corner
				sub rax, rdi
				ret

			.r10is2:
				mov r11, r9
				imul r11, [Width]
				add r11, rcx
				mov di, WORD [ShortIntegralGrid + 2*r11] ; Bottom right corner

				mov r11, rdx
				imul r11, [Width]
				add r11, r8
				add di, WORD [ShortIntegralGrid + 2*r11] ; Top left corner
				mov rax, rdi

				mov r11, r9
				imul r11, [Width]
				add r11, r8
				mov di, WORD [ShortIntegralGrid + 2*r11] ; Bottom left corner

				mov r11, rdx
				imul r11, [Width]
				add r11, rcx
				add di, WORD [ShortIntegralGrid + 2*r11] ; Top right corner
				sub rax, rdi
				ret


		.r10is34:
			cmp r10, 4
			jl .r10is3
			jmp .r10is4

			.r10is3:
				mov r11, rdx
				imul r11, [Width]
				add r11, r8
				mov di, WORD [ShortIntegralGrid + 2*r11] ; Bottom right corner

				mov r11, r9
				imul r11, [Width]
				add r11, rcx
				add di, WORD [ShortIntegralGrid + 2*r11] ; Top left corner
				mov rax, rdi

				mov r11, rdx
				imul r11, [Width]
				add r11, rcx
				mov di, WORD [ShortIntegralGrid + 2*r11] ; Bottom left corner

				mov r11, r9
				imul r11, [Width]
				add r11, r8
				add di, WORD [ShortIntegralGrid + 2*r11] ; Top right corner
				sub rax, rdi
				ret

			.r10is4:
				mov r11, r9
				imul r11, [Width]
				add r11, r8
				mov di, WORD [ShortIntegralGrid + 2*r11] ; Bottom right corner

				mov r11, rdx
				imul r11, [Width]
				add r11, rcx
				add di, WORD [ShortIntegralGrid + 2*r11] ; Top left corner
				mov rax, rdi

				mov r11, r9
				imul r11, [Width]
				add r11, rcx
				mov di, WORD [ShortIntegralGrid + 2*r11] ; Bottom left corner

				mov r11, rdx
				imul r11, [Width]
				add r11, r8
				add di, WORD [ShortIntegralGrid + 2*r11] ; Top right corner
				sub rax, rdi
				ret

; =======================================================================================================
Global NeighborCheck  ;  Important Note - This does not check for map bounderies, the map is expected to have a solid border

NeighborCheck:  ; rcx(position), rdx(direction), r8(target position), r9(iteration count)
	mov r10, rcx ; put position into r10

	cmp rdx, 1 ; Check if rdx is greater than 1
	jg .nextcmp23

	cmp rdx, 0  ; Check if rdx is 0
	jg .is1 
	add r10, QWORD [PositionCheckNorth] ; position + direction
	jmp .resume

	.is1:   ; rdx is 1
		add r10, QWORD [PositionCheckWest] ; position + direction
	    jmp .resume

	.nextcmp23:
		cmp rdx, 3  ; Check if rdx is 3
		jl .is2
		add r10, QWORD [PositionCheckEast] ; position + direction
	    jmp .resume

		.is2:   ; rdx is 2
			add r10, QWORD [PositionCheckSouth] ; position + direction
			jmp .resume

	.resume:
		mov eax, DWORD [PathIntGrid + 4*r10] ; Position within the integer grid
		cmp eax, -1  ; check the path grid for if already checked
		je .ContinueCheck

		mov rax, 0  ;  If fails check return value of 0
		ret

		.ContinueCheck:
			  ;  Split the array position into x and y components

			mov rdx, 0
			mov rax, r10
			cqo
			mov rcx, [Width]
			idiv rcx

			; Checks the bool grid for solid or not

			mov rcx, rdx ; x-value    
			mov rdx, rax ; y-value
			imul rdx, 8

			mov rsi, 0x8000000000000000
			shr rsi, cl ; shift left by x units

			mov rax, [BoolGrid + rdx] ; pointer to (grid + x + y)
			and rax, rsi ; Check specific bit
	
			cmp rax, 0 ; if (rax > 0) -> there is a bit that came through the AND and thus solid
			je .nocollision
			mov rax, 0  ;  If fails check place a one accordingly
			ret

			.nocollision:    ; Add the tile to the list of tiles to check
				mov DWORD [PathIntGrid + 4*r10], r9d

				cmp r10, r8
				je .FoundFinish

				mov rax, 1 ;  If passes checks return value of 1
				ret

				.FoundFinish:
					mov rax, 2 ;  If position equals target position return value of 2
					ret

; ===============================================================
Global NeighborRetrieve

NeighborRetrieve:  ; rcx(position), rdx(direction)

		;  Find which direction it is via search algorithm (using an array was giving me touble)
	cmp rdx, 3 ; Check if rdx is greater than 3
	jg .nextcmp4567

	cmp rdx, 1 ; Check if rdx is greater than 1
	jg .nextcmp23

	cmp rdx, 0  ; Check if rdx is 0
	jg .is1 
	add ecx, DWORD [PositionCheckNorth]
	jmp .resume

	.is1:   ; rdx is 1
		add ecx, DWORD [PositionCheckWest]
	    jmp .resume

	.nextcmp23:
		cmp rdx, 3  ; Check if rdx is 3
		jl .is2
		add ecx, DWORD [PositionCheckEast]
	    jmp .resume

		.is2:   ; rdx is 2
			add ecx, DWORD [PositionCheckSouth]
			jmp .resume

	.nextcmp4567:
		cmp rdx, 5 ; Check if rdx is greater than 5
		jg .nextcmp67

		cmp rdx, 4  ; Check if rdx is 4
		jg .is5 
		add ecx, DWORD [PositionCheckNorthWest]
		jmp .resume

		.is5:   ; rdx is 5
			add ecx, DWORD [PositionCheckNorthEast]
			jmp .resume

		.nextcmp67:
			cmp rdx, 7  ; Check if rdx is 7
			jl .is6
			add ecx, DWORD [PositionCheckSouthWest]
			jmp .resume

			.is6:   ; rdx is 6
				add ecx, DWORD [PositionCheckSouthEast]
				jmp .resume

	.resume:
	mov eax, ecx
	ret

; ===============================================================
Global NeighborCheckPath  ;  Important Note - This does not check for map bounderies, the map is expected to have a solid border

NeighborCheckPath: ; ecx(current pos)  ;  Finds the next cell to move to

	mov DWORD [CurrentLowestCheckVal], 1000 ;  Reset at the beginning of each call

	mov r8, rcx ; save rcx in r8
	mov DWORD [CurrentCheckPos], ecx        ; Set value of current cell for comparision later

	mov r9, -1 ; The loop ahead starts by adding 1 and we want to start at zero
	mov r10, 0x00000000 ; keeps track of which tiles are still good
	.CheckLoopBegins:
		cmp r9, 7 
		jl .ContinueCmp

		mov eax, DWORD [CurrentCheckPos] ; There was an error
		ret

		.ContinueCmp:
			add r9, 1
			mov rcx, r8

			cmp r9, 0
			je .North
			cmp r9, 1
			je .West
			cmp r9, 2
			je .East
			cmp r9, 3
			je .South

			mov rax, 0x11001000
			and rax, r10
			cmp rax, 0
			je .NorthWest

			mov rax, 0x10100100
			and rax, r10
			cmp rax, 0
			je .NorthEast

			mov rax, 0x01010010
			and rax, r10
			cmp rax, 0
			je .SouthWest

			mov rax, 0x00110001
			and rax, r10
			cmp rax, 0
			je .SouthEast

			mov eax, DWORD [CurrentCheckPos] ; return once all surounding tiles have been checked
			ret 


			.North: ; North check is first
				mov r11, 0x10000000
				add rcx, QWORD [PositionCheckNorth]
				jmp .BeginChecks
			
			.West:
				mov r11, 0x01000000
				add rcx, QWORD [PositionCheckWest]
				jmp .BeginChecks

			.East:
				mov r11, 0x00100000
				add rcx, QWORD [PositionCheckEast]
				jmp .BeginChecks

			.South:
				mov r11, 0x00010000
				add rcx, QWORD [PositionCheckSouth]
				jmp .BeginChecks


			.NorthWest:
				mov r11, 0x00001000
				add rcx, QWORD [PositionCheckNorthWest]
				jmp .BeginChecks


			.NorthEast:
				mov r11, 0x00000100
				add rcx, QWORD [PositionCheckNorthEast]
				jmp .BeginChecks

			.SouthWest:
				mov r11, 0x00000010
				add rcx, QWORD [PositionCheckSouthWest]
				jmp .BeginChecks

			.SouthEast:
				mov r11, 0x00000001
				add rcx, QWORD [PositionCheckSouthEast]
				jmp .BeginChecks



			.BeginChecks:
				mov edi, ecx

				mov eax, DWORD [PathIntGrid + 4*ecx]
				cmp eax, -1  ; check the path grid for if unchecked -> means solid or don't care for it
				jne .ContinueCheck

				or r10, r11  ;  If fails check place, a one accordingly
				jmp .CheckLoopBegins

				.ContinueCheck:
					cmp eax, DWORD [CurrentLowestCheckVal]  ; Compare to find next best cell to move to
					jl .SetNewClosest
					jmp .CheckLoopBegins

				.SetNewClosest:
					mov DWORD [CurrentCheckPos], edi ; Set new closest cell
					mov DWORD [CurrentLowestCheckVal], eax ; Set the new lowest wave value 
					jmp .CheckLoopBegins

; ===============================================================
Global PathIntGridReset

PathIntGridReset:
	mov rax, [Width]
	imul rax, [Height]

	mov rcx, 0
	jmp .loopbegin

	.loopstart:
		add rcx, 1
	.loopbegin:
		cmp DWORD [PathIntGrid + 4*rcx], -1
		je .loopstart
		mov DWORD [PathIntGrid + 4*rcx], -1

		cmp rcx, rax
		jl .loopstart
			mov rax, rcx
			ret

; ===============================================================
Global PathIntGridGet

PathIntGridGet:
	imul rdx, [Width] ; y value times the width
	add rdx, rcx ; add the position in the row
	
	mov rax, [Width]
	imul rax, [Height]

	cmp edx, -1
	je .retnegative
	cmp rdx, rax
	jl .continue
		mov rax, rdx
		ret

	.retnegative:
		mov rax, -1
		ret

	.continue:
		mov ecx, DWORD [PathIntGrid + 4*rdx]
		mov eax, ecx
		ret

; ===============================================================
Global SetStartinPathGrid

SetStartinPathGrid:
	mov DWORD [PathIntGrid + 4*rcx], 0
	mov eax, 0
	ret

; ======================================================================================================= 
Global GetWidth ; Returns the width

GetWidth:
	mov rax, QWORD [Width]
	ret

; ===============================================================
Global GetHeight ; Returns the width

GetHeight:
	mov rax, QWORD [Height]
	ret
; ======================================================================================================= 

; =======================================================================================================            .labels!!!!!!!!!!! YAY !!!!!!!!!!!!!!!
; VARIABLE SETTING FUNCTIONS
; =======================================================================================================


; NOTE: Possibly need to remove these setting functions
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

; Need to get rid of this code later and adjust functions that use it
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


;     For use with Grid functions

BoolGrid:	; A grid of 64x64 bits 
times 64   dq 0x0000000000000000

CharGrid:	; A grid of 64x8 64bit longs with 8 chars in each
times 64*8 dq 0x0000000000000000

CharOverlayGrid:	; A grid of 64x8 64bit longs with 8 chars in each
times 64*8 dq 0x0101010101010101


;     For use with the path finding code
PathStart: dq 0
PathFinish: dq 0

PositionCheckNorth: dq 0  ; North
PositionCheckWest:	dq -1 ; West
PositionCheckEast:	dq 1  ; East
PositionCheckSouth:	dq 0  ; South

PositionCheckNorthWest:	dq 0 ; NorthWest
PositionCheckNorthEast:	dq 0 ; NorthEast
PositionCheckSouthWest:	dq 0 ; SouthWest
PositionCheckSouthEast:	dq 0 ; SouthEast

PathIntGrid:
times 64*64 dd -1

CurrentCheckPos: dd 0
CurrentLowestCheckVal: dd 1000

;     For use with the integralgrid code
ShortIntegralGrid: 
times 64*64 dw 0

IntegralGridResetFromXPosition: dq 0



;     For use with everything more or less
Char: db 0
Global Width
Width:	; Giving width and height an aditional piece of data as I had some issues with functionality previously
	dq 0
	dq 0

Global Height
Height:  
	dq 0
	dq 0