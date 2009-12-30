[bits 32]
	mov [0x1000], bx
	mov [0x10], ax
	mov ax, [0x10]
	mov byte [bx+si], 0x10
	mov byte [bx+si+0x10], 0x10
	mov word [bp+0x0], 0x10
	mov word [bp+di+0x10], 0x10
	mov dword [si+0x10], 0x10
	mov word [di+0x10], 0x10
	mov dword [bx+0x10], 0x1000
	mov word [bx+0x1000], 0x1000
	mov dword [ebx+ecx*4], 0x100
	mov [eax+eax*2], eax
	mov [edx+esi*8], ebp
	mov dword [ecx*4+0x0], 0x100
	mov byte [cs:0x100000], 0x10
	mov word [eax+0x10], 0x10
	mov [eax+0x10], ax
	mov [eax+0x1000], ebx
	mov [ebp+eax+0x0], esi
	mov [ebp+edi+0x100000], esp
	mov byte [esp], 0x10
	add al, 0x10
	add eax, ebx
	push es
	pop es
	adc eax, 0x10000
	and [eax], al
	daa 
	inc ax
	inc edx
	push eax
	push ax
	pushad 
	bound eax, [eax]
	bound ax, [ecx]
	bsr ax, ax
	bsf eax, [bx+si]
	bswap eax
	bt [eax], ax
	btr ax, 0x10
	btc ebx, 0x10
	bts word [ebx], 0x10
	call ax
	call word near [bx+si]
	call eax
	call dword near [eax+ecx]
	call word 0x10:0x100
	call dword 0x10:0x10000
	call word far [eax]
	call dword far [bp+si]
	cbw 
	cwd 
	clc 
	cld 
	clflush [eax]
	cmp eax, ebx
	cmp ecx, [bx]
	cmpsb 
	cmpsw 
	cmpsd 
	cmpxchg [eax], ax
	cmpxchg8b [ebx]
	cpuid 
	das 
	inc eax
	inc word [ecx]
	dec byte [si]
	in al, 0x10
	in ax, 0x10
	in eax, 0x10
	insb 
	insw 
	insd 
	int 0x10
	into 
	lahf 
	lds ax, [eax]
	les eax, [ebx]
	lea ax, [eax]
	lea eax, [bx+si]
	leave 
	lodsb 
	lodsw 
	lodsd 
	test al, bl
	test [eax], bl
	test [eax], ebx
	test [eax], bx
	ret 
	ret 0x10
	aad 0x10
	aam 0x10
	salc 
	hlt 
	cmc 
	lock xchg ebx, eax
	repne mov eax, ebx
	rep mov eax, 0x10
	push cs
	outsd 
	outsw 
	mov ax, es
	mov bx, ds
	mov [eax], es
	mov [ebx+ecx], cs
	mov cs, [ebx+ecx]
	wait 
	pushfw 
	pushfd 
	lodsw 
	lodsd 
	retf 0x10
	int3 
	into 
	iretw 
	iretd 
	lar eax, [eax]
	lsl ax, [bx]
	syscall 
	clts 
	sysret 
	movups xmm0, xmm1
	mov dr0, eax
	wrmsr 
	rdmsr 
	rdtsc 
	rdtscp 
	rdpmc 
	sysenter 
	sysexit 
	cmovo eax, [eax]
	cmovno eax, [bx]
	cmovb eax, [eax]
	cmovae eax, [bx]
	cmovo eax, [eax]
	cmovz eax, ebx
	cmovnz eax, [eax]
	cmovbe eax, [bx]
	cmova eax, [bx]
	movmskps eax, xmm0
	movmskpd eax, xmm0
	sqrtps xmm1, xmm0
	rsqrtps xmm1, xmm0
	rcpps xmm1, xmm0
	andps xmm1, xmm0
	orps xmm1, xmm0
	xorps xmm1, xmm0
	andnps xmm1, xmm0
	sqrtss xmm1, xmm0
	rsqrtss xmm1, xmm0
	rcpss xmm1, xmm0
	sqrtpd xmm1, xmm0
	andpd xmm1, xmm0
	andnpd xmm1, xmm0
	orpd xmm1, xmm0
	xorpd xmm1, xmm0
	sqrtsd xmm1, xmm0
	punpcklbw mm0, [eax]
	punpcklwd mm0, mm1
	punpckldq mm0, mm1
	packsswb mm0, mm1
	packsswb mm0, mm1
	pcmpgtb mm0, mm1
	pcmpgtw mm0, mm1
	pcmpgtd mm0, mm1
	packuswb mm0, mm1
	punpcklbw xmm0, [eax]
	punpcklwd xmm0, xmm1
	punpckldq xmm0, xmm1
	packsswb xmm0, xmm1
	packsswb xmm0, xmm1
	pcmpgtb xmm0, xmm1
	pcmpgtw xmm0, xmm1
	pcmpgtd xmm0, xmm1
	packuswb xmm0, xmm1
	pshufw mm0, mm1, 0x10
	pcmpeqb mm0, mm1
	pcmpeqw mm0, mm1
	pcmpeqd mm0, mm1
	pcmpeqb xmm0, xmm1
	pcmpeqw xmm0, xmm1
	pcmpeqd xmm0, xmm1
	emms 
	pshufhw xmm0, xmm1, 0x10
	pshufd xmm0, xmm1, 0x10
	pshuflw xmm0, xmm1, 0x10
	seto [eax]
	setno [bx]
	setz [es:eax+ecx*2+0x100]
	push fs
	pop fs
	cpuid 
	bt [eax], eax
	shld eax, ebx, 0x10
	shld [eax], bx, cl
	cmpxchg [eax], eax
	lss eax, [eax]
	btr [eax], eax
	movnti [eax], eax
	psrlw mm0, 0x10
	fadd dword [eax]
	imul eax, [eax], 0xf6
	movd dword [eax], xmm0
	movzx eax, word [eax]
	push word [0x10]
	insw 
	insd 
	fnstsw ax
	fucomip st0, st1
	fcomip st0, st7
	fucomp st4
	fucom st5
	fstp st3
	fst st1
	ffree st0
	fdiv st7, st0
	fdivr st2, st0
	fsub st4, st0
	fsubr st6, st0
	fmul st0, st0
	fadd st5, st0
	ficom word [eax]
	fidivr word [eax]
	fimul word [ebx]
	fisub word [ecx]
	fld qword [bx+si]
	fisttp qword [edx+0x100]
	fnstsw word [eax]
	frstor [ebx]
	prefetch [bx+si]
	psrlq xmm0, 0x10
	psrldq xmm0, 0x10
	movsldup xmm0, [eax]
	add [0xffffffff], eax
	cvtsi2ss xmm1, dword [eax]
	pause 
	pop dword [eax]
	out 0x0, al
	lldt [0x100]
	lgdt [0x221]
	sldt [0x233]
	sgdt [0x443]
	lidt [eax+0x333]
	lldt ax
	ltr bx
	verr cx
	verw dx
	sldt ax
	str bx
	str eax
	and esp, 0xfc
	psrlw xmm1, 0x10
	psraw xmm7, 0x1
	psllw xmm2, 0x23
	fldenv [0x10]
	fldenv [0x123]
	fldcw word [0x100]
	fnstcw word [0x10]
	ficom word [eax+ebx+0x10]
	fstp tword [0x10]
	fadd qword [eax+ebx*2+0x1]
	frstor [0x100]
	fnstsw word [0x100]
	fiadd word [0x100]
	fild word [0x10]
	monitor 
	mwait 
	lfence 
	mfence 
	sfence 
	vmrun 
	vmmcall 
	vmload 
	vmsave 
	stgi 
	clgi 
	skinit 
	invlpga 
