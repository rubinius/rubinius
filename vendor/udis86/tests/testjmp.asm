[bits 64]

	jnz near x
	jo near x
	jno word x
	jc near x
	jnc word x
	jae dword x
	jcxz x
	jecxz x
	jrcxz x
	jmp near x
	call x
	jmp word x
	jmp dword x
	jmp word [eax]	
x:	jmp qword [rax]
