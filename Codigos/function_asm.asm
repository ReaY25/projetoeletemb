;asmFile.asm
; *****************************************************************************
			.cdecls C,LIST,"msp430.h"

			.global media_ADC
media_ADC:
			PUSH	R15
			MOV		R12,R15
			MOV		0(R15),R12
			ADD		2(R15),R12
			ADD		4(R15),R12
			ADD		6(R15),R12
			RRA		R12
			RRA		R12
			POP		R15
			ret
