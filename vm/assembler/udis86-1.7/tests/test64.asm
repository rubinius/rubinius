[bits 64]
	mov rax, 0x102030405060708
	mov [eax+0x10], ax
	mov [eax+0x1000], ebx
	mov [ebp+eax+0x0], esi
	mov qword [ebp+edi+0x1000], 0x10
	mov byte [esp], 0x10
	mov [eax], rax
	mov [r12], eax
	mov [r13+r12+0x0], eax
	mov [r8*4+0x0], sil
	inc rax
	dec rax
	mov [rip+0x200], rax
	mov rax, 0x10000
	push rax
	push r12
	call word near [r8]
	call qword near [rax]
	call word far [r8]
	call dword far [rax]
	cbw 
	cwde 
	cdqe 
	cwd 
	cdq 
	cqo 
	cmovl rax, [rax]
	cmovge eax, [rax]
	clflush [r14]
	cmp [rax], rax
	cmp rbx, r12
	cmp r12d, r8d
	cmpsb 
	cmpsw 
	cmpsd 
	cmpsq 
	cmpxchg [eax], r14
	cmpxchg8b [ebx]
	inc r12d
	inc dword [rax]
	dec r11w
	hlt 
	imul rax, [eax+ebx*8+0x100000], 0x10
	idiv dword [r12]
	enter 0x100, 0x0
	enter 0x100, 0x1
	enter 0x100, 0x10
	in al, 0x10
	in ax, 0x10
	in eax, 0x10
	lfs eax, [eax]
	lgs eax, [ebx]
	lea eax, [rbx]
	lea r11, [eax]
	lodsb 
	lodsq 
	lodsd 
	push rax
	push r11
	xchg [eax], sil
	xchg [eax], ebx
	xchg [eax], bx
	xchg r8, rax
	xchg r9, rax
	xchg cx, ax
	movsd 
	movsq 
	mov al, r11b
	mov sil, al
	mov r11b, dil
	ret 0x10
	pop rax
	pop r11
	pop qword [eax]
	insd 
	outsd 
	mov [r14d], cs
	mov cs, [ebx+ecx]
	pushfq 
	scasq 
	lar rax, [eax]
	lsl ax, [rbx]
	movups xmm0, [r12d]
	movupd xmm0, [r12d]
	movsldup xmm0, [r12d]
	movups xmm0, xmm1
	movups xmm10, xmm12
	movups xmm0, xmm12
	movlps xmm0, [rax]
	movlps [rax], xmm0
	unpcklps xmm4, xmm5
	unpckhps xmm4, xmm5
	movhps xmm3, [eax]
	movhps [rax], xmm3
	movss xmm0, [eax]
	movss [eax], xmm0
	movlpd [eax], xmm0
	movlpd xmm0, [eax]
	unpcklpd xmm2, xmm4
	unpckhpd xmm3, [eax+ebx*8+0xffffffff]
	movhpd xmm3, [rax]
	movhpd [rax], xmm2
	movsd xmm3, xmm4
	movddup xmm3, [rax]
	mov dr0, rax
	mov rax, dr1
	movnti [eax], eax
	movnti [rax], rax
	movd dword [eax], xmm0
	movd qword [eax], xmm0
	movd xmm11, qword [eax]
	vmmcall 
	vmrun 
	clgi 
	stgi 
	mfence 
	lfence 
	sfence 
	clflush [rax]
	mov cr8, rax
	push qword [eax]
	push word [eax]
	add bh, bh
	add dil, dil
	add sil, bpl
	add al, sil
	add rax, r12
	add eax, r12d
	prefetcht0 [eax]
	prefetchnta [eax]
	prefetch [eax]
	add [r8], r9b
	mov [rax-0x1], eax
	inc rax
	jmp qword near [eax]
	jmp rax
	mov [0x10], rax
	mov rax, [0x10]
	mov rax, 0x102030405060708
	nop 
	xchg r8, rax
	push ax
	push rax
	push r11
	mov rax, [0x100]
	pmovmskb r12d, xmm14
	movdq2q mm0, xmm13
	psrlw xmm10, 0x10
	psraw xmm7, 0x1
	psllw xmm12, 0x23
	swapgs 
	fadd dword [rax+rbx]
	shl rsi, 1
