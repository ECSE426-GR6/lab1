	AREA text, CODE, READONLY
	export Kalmanfilter_asm
	export test_asm

test_asm
	mov R1, R0
	BX LR

Kalmanfilter_asm
	;INPUTS
	;R0 - input ptr
	;R1 - output ptr
	;R2 - kalman ptr
	;R3 - length
	;R4 - current index
	;R5 - temporary address register

	;S0 - current measurement
	;S1 - q
	;S2 - r
	;S3 - x
	;S4 - p
	;S5 - k
	;S6 - temporary register
	;S7 - nan check register

	MOV R4, #0 ;current index

	VLDR S1, [R2]		;q
	VLDR S2, [R2, #4]	;r
	VLDR S3, [R2, #8]	;x
	VLDR S4, [R2, #12]	;p
	VLDR S5, [R2, #16]	;k

next_measurement

	;load next measurement
	LSL R5, R4, #2		;calculate offset from index
	ADD R5, R0, R5		;add base address to offset
	VLDR S0, [R5] 		;S0 = input[R4]

	;p = p + q
	VADD.F32 S4, S4, S1

	;k = p / (p + r)
	VADD.F32 S6, S4, S2 ;S6 = p + r
	VDIV.F32 S5, S4, S6	;k = p / S6

	;x = x + k * (measurement - x)
	VSUB.F32 S6, S0, S3	;S6 = measurement - x
	VMUL.F32 S6, S5, S6 ;S6 = k * S6
	VADD.F32 S3, S3, S6 ;x = x + S6

	;p = (1 - k) * p
	VMOV.F32 S6, #1.0 		;store 1 in S6 to subtract from
	VSUB.F32 S6, S6, S5	;S6 = 1 - k
	VMUL.F32 S4, S6, S4		;p = S6 - p

	;store result in output array and increment index R4
	LSL R5, R4, #2		;calculate offset from index
	ADD R5, R1, R5		;add base address to offset
	VSTR.F32 S3, [R5]	;store x (S3) in output[R4]
	ADD R4, R4, #1		;R4++

	;loop condition: index i < length
	CMP R4, R3				;compare R4(index) and R3(length)
								;if R4 - R3 < 0 then N = 1
	BMI next_measurement	;loop if R4 (index) < R3 (length)
							;else return
	;store final state
	VSTR.F32 S1, [R2]		;q
	VSTR.F32 S2, [R2, #4]	;r
	VSTR.F32 S3, [R2, #8]	;x
	VSTR.F32 S4, [R2, #12]	;p
	VSTR.F32 S5, [R2, #16]	;k

	;error checking (check for overflows and nan exceptions)
	VMRS R0, FPSCR	;copy FPSCR to R0
	MOV R1, #0xF	;set R1 to possible exceptions (inexact exception ignored)
	AND R0, R0, R1	;if any exception occured, R0 != 0, else R0 == 0
	BX LR			;Return from subroutine
	END
