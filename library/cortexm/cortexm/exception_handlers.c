//
// This file is part of the µOS++ III distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include "cortexm/exception_handlers.h"
#include "startup/mal_startup.h"

// ----------------------------------------------------------------------------
// Default exception handlers. Override the ones here by defining your own
// handler routines in your application code.
// ----------------------------------------------------------------------------

void __attribute__ ((section(".after_vectors"),noreturn))
Reset_Handler(void) {
	mal_startup();
	while(1);
}

void __attribute__ ((section(".after_vectors"),weak))
NMI_Handler(void) {
	while (1) {
	}
}

// Hard Fault handler wrapper in assembly.
// It extracts the location of stack frame and passes it to handler
// in C as a pointer. We also pass the LR value as second
// parameter.
// (Based on Joseph Yiu's, The Definitive Guide to ARM Cortex-M0
// First Edition, Chap. 12.8, page 402).

void __attribute__ ((section(".after_vectors"),weak,naked))
HardFault_Handler(void) {
	asm volatile(
			" movs r0,#4      \n"
			" mov r1,lr       \n"
			" tst r0,r1       \n"
			" beq 1f          \n"
			" mrs r0,psp      \n"
			" b   2f          \n"
			"1:               \n"
			" mrs r0,msp      \n"
			"2:"
			" mov r1,lr       \n"
			" ldr r2,=HardFault_Handler_C \n"
			" bx r2"

			: /* Outputs */
			: /* Inputs */
			: /* Clobbers */
	);
}

void __attribute__ ((section(".after_vectors"),weak,used))
HardFault_Handler_C(ExceptionStackFrame* frame __attribute__((unused)),
		uint32_t lr __attribute__((unused))) {
	// There is no semihosting support for Cortex-M0, since on ARMv6-M
	// faults are fatal and it is not possible to return from the handler.

	while (1) {
	}
}

void __attribute__ ((section(".after_vectors"),weak))
SVC_Handler(void) {
	while (1) {
	}
}

void __attribute__ ((section(".after_vectors"),weak))
PendSV_Handler(void) {
	while (1) {
	}
}

void __attribute__ ((section(".after_vectors"),weak))
SysTick_Handler(void) {
	// DO NOT loop, just return.
	// Useful in case someone (like STM HAL) inadvertently enables SysTick.
	;
}

// ----------------------------------------------------------------------------
