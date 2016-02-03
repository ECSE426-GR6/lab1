	AREA text, CODE, READONLY
	export init_memory
	export process_measurement

	import in_ptr
	import out_ptr
	import kal_ptr
	import arr_length

init_memory
	;INPUTS
	;R0 - input ptr
	;R1 - output ptr
	;R2 - kalman ptr
	;R3 - length

	LDR R4, =in_ptr
	LDR R5, =out_ptr
	LDR R6, =kal_ptr
	LDR R7, =arr_length

	STR R0, [R4]
	STR R1, [R5]
	STR R2, [R6]
	STR R3, [R7]

	BX LR ;


process_measurement

	;S0 - input measurement
	;update params
	;add result to array
	;return result through S0

	BX LR ;
	end
