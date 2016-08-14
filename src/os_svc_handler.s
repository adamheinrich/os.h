.syntax unified
.cpu cortex-m0
.fpu softvfp

.thumb

.global SVC_Handler
.type SVC_Handler, %function
SVC_Handler:
	/* Exception frame saved by the NVIC hardware onto stack:
	+------+
	| xPSR |
	|  PC  |
	|  LR  |
	|  R12 |
	|  R3  |
	|  R2  |
	|  R1  |
	|  R0  | <- SP points here
	+------+

	Check bit 2 of the LR reg. to determine which stack has been used: */
	movs	r0, #4	/* 100b */
	mov	r1, lr
	tst	r0, r1
	beq	msp_used
psp_used:
	mrs	r1, psp
	b	extract_svc_number
msp_used:
	mrs 	r1, msp
extract_svc_number:
	/* The stacked PC value is located at SP+24 bytes. The PC points after
	the SVC instruction which has invoked the interrupt. Subtract two bytes
	to get the immediate value encoded in the instruction: */
	ldr	r0, [r1, #24]
	subs	r0, #2
	ldrb	r0, [r0, #0]
	/* Call os_svcall_handler(uint8_t svc_num, uint32_t *p_stack_frame)
	(svc_num has to be in r0, p_stack_frame = SP in r1): */
	ldr	r2, =os_svcall_handler
	bx	r2

.size SVC_Handler, .-SVC_Handler
