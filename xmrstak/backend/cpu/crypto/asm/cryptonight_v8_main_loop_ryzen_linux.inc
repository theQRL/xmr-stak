	mov	qword ptr [rsp+16], rbx
	mov	qword ptr [rsp+24], rbp
	mov	qword ptr [rsp+32], rsi
	push	rdi
	push	r12
	push	r13
	push	r14
	push	r15
	sub	rsp, 64

	stmxcsr dword ptr [rsp]
	mov dword ptr [rsp+4], 24448
	ldmxcsr dword ptr [rsp+4]

	mov	rax, qword ptr [rcx+48]
	mov	r9, rcx
	xor	rax, qword ptr [rcx+16]
	mov	ebp, 524288
	mov	r8, qword ptr [rcx+32]
	xor	r8, qword ptr [rcx]
	mov	r11, qword ptr [rcx+40]
	mov	r10, r8
	mov	rdx, qword ptr [rcx+56]
	movq	xmm3, rax
	xor	rdx, qword ptr [rcx+24]
	xor	r11, qword ptr [rcx+8]
	mov	rbx, qword ptr [rcx+224]
	mov	rax, qword ptr [r9+80]
	xor	rax, qword ptr [r9+64]
	movq	xmm0, rdx
	mov	rcx, qword ptr [rcx+88]
	xor	rcx, qword ptr [r9+72]
	mov	rdi, qword ptr [r9+104]
	and	r10d, 2097136
	movaps	xmmword ptr [rsp+48], xmm6
	movq	xmm4, rax
	movaps	xmmword ptr [rsp+32], xmm7
	movaps	xmmword ptr [rsp+16], xmm8
	xorps	xmm8, xmm8
	mov ax, 1023
	shl rax, 52
	movq xmm7, rax
	mov	r15, qword ptr [r9+96]
	punpcklqdq xmm3, xmm0
	movq	xmm0, rcx
	punpcklqdq xmm4, xmm0

	.align 64
main_loop_ryzen:
	movdqa	xmm5, xmmword ptr [r10+rbx]
	movq	xmm0, r11
	movq	xmm6, r8
	punpcklqdq xmm6, xmm0
	lea	rdx, qword ptr [r10+rbx]
	lea	r9, qword ptr [rdi+rdi]
	shl	rdi, 32

	mov	ecx, r10d
	mov	eax, r10d
	xor	ecx, 16
	xor	eax, 32
	xor	r10d, 48
	aesenc	xmm5, xmm6
	movdqa	xmm2, xmmword ptr [rcx+rbx]
	movdqa	xmm1, xmmword ptr [rax+rbx]
	movdqa	xmm0, xmmword ptr [r10+rbx]
	paddq	xmm2, xmm3
	paddq	xmm1, xmm6
	paddq	xmm0, xmm4
	movdqa	xmmword ptr [rcx+rbx], xmm0
	movdqa	xmmword ptr [rax+rbx], xmm2
	movdqa	xmmword ptr [r10+rbx], xmm1

	movaps	xmm1, xmm8
	mov	rsi, r15
	xor	rsi, rdi
	movq	r14, xmm5
	movdqa	xmm0, xmm5
	pxor	xmm0, xmm3
	mov	r10, r14
	and	r10d, 2097136
	movdqa	xmmword ptr [rdx], xmm0
	xor	rsi, qword ptr [r10+rbx]
	lea	r12, qword ptr [r10+rbx]
	mov	r13, qword ptr [r10+rbx+8]

	add	r9d, r14d
	or	r9d, -2147483647
	xor	edx, edx
	movdqa	xmm0, xmm5
	psrldq	xmm0, 8
	movq	rax, xmm0

	div	r9
	movq xmm0, rax
	movq xmm1, rdx
	punpckldq xmm0, xmm1
	movq r15, xmm0
	paddq xmm0, xmm5
	movdqa xmm2, xmm0
	psrlq xmm0, 12
	paddq	xmm0, xmm7
	sqrtsd	xmm1, xmm0
	movq	rdi, xmm1
	test	rdi, 524287
	je	sqrt_fixup_ryzen
	shr	rdi, 19

sqrt_fixup_ryzen_ret:
	mov	rax, rsi
	mul	r14
	movq xmm1, rax
	movq xmm0, rdx
	punpcklqdq xmm0, xmm1

	mov	r9d, r10d
	mov	ecx, r10d
	xor	r9d, 16
	xor	ecx, 32
	xor	r10d, 48
	movdqa	xmm1, xmmword ptr [rcx+rbx]
	xor rdx, [rcx+rbx]
	xor rax, [rcx+rbx+8]
	movdqa	xmm2, xmmword ptr [r9+rbx]
	pxor xmm2, xmm0
	paddq xmm4, xmmword ptr [r10+rbx]
	paddq	xmm2, xmm3
	paddq	xmm1, xmm6
	movdqa	xmmword ptr [r9+rbx], xmm4
	movdqa	xmmword ptr [rcx+rbx], xmm2
	movdqa	xmmword ptr [r10+rbx], xmm1

	movdqa	xmm4, xmm3
	add	r8, rdx
	add	r11, rax
	mov	qword ptr [r12], r8
	xor	r8, rsi
	mov	qword ptr [r12+8], r11
	mov	r10, r8
	xor	r11, r13
	and	r10d, 2097136
	movdqa	xmm3, xmm5
	dec	ebp
	jne	main_loop_ryzen

	ldmxcsr dword ptr [rsp]
	movaps	xmm6, xmmword ptr [rsp+48]
	lea	r11, qword ptr [rsp+64]
	mov	rbx, qword ptr [r11+56]
	mov	rbp, qword ptr [r11+64]
	mov	rsi, qword ptr [r11+72]
	movaps	xmm8, xmmword ptr [r11-48]
	movaps	xmm7, xmmword ptr [rsp+32]
	mov	rsp, r11
	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	rdi
	jmp cnv2_main_loop_ryzen_endp

sqrt_fixup_ryzen:
	movq r9, xmm2
	dec	rdi
	mov edx, -1022
	shl rdx, 32
	mov	rax, rdi
	shr	rdi, 19
	shr	rax, 20
	mov	rcx, rdi
	sub	rcx, rax
	lea	rcx, [rcx+rdx+1]
	add	rax, rdx
	imul	rcx, rax
	sub	rcx, r9
	adc	rdi, 0
	jmp	sqrt_fixup_ryzen_ret

cnv2_main_loop_ryzen_endp:
