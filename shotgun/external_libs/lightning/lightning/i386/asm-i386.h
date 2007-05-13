/******************************** -*- C -*- ****************************
 *
 *	Run-time assembler for the i386
 *
 ***********************************************************************/


/***********************************************************************
 *
 * Copyright 1999, 2000, 2001, 2002 Ian Piumarta
 * Copyright 2003 Gwenole Beauchesne
 * Copyright 2006 Free Software Foundation
 *
 * This file is part of GNU lightning.
 *
 * GNU lightning is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1, or (at your option)
 * any later version.
 * 
 * GNU lightning is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with GNU lightning; see the file COPYING.LESSER; if not, write to the
 * Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 ***********************************************************************/




#ifndef __lightning_asm_i386_h
#define __lightning_asm_i386_h

/*	OPCODE	+ i		= immediate operand
 *		+ r		= register operand
 *		+ m		= memory operand (disp,base,index,scale)
 *		+ sr/sm		= a star preceding a register or memory
 */

typedef _uc		jit_insn;

#ifndef LIGHTNING_DEBUG
#define _b00		0
#define _b01		1
#define _b10		2
#define _b11		3

#define _b000		0
#define _b001		1
#define _b010		2
#define _b011		3
#define _b100		4
#define _b101		5
#define _b110		6
#define _b111		7

/*** REGISTERS ***/	/* [size,,number] */

#define _NOREG		0

#define _AL		0x10
#define _CL		0x11
#define _DL		0x12
#define _BL		0x13

#define _AX		0x30
#define _CX		0x31
#define _DX		0x32
#define _BX		0x33
#define _SP		0x34
#define _BP		0x35
#define _SI		0x36
#define _DI		0x37

#define _EAX		0x40
#define _ECX		0x41
#define _EDX		0x42
#define _EBX		0x43
#define _ESP		0x44
#define _EBP		0x45
#define _ESI		0x46
#define _EDI		0x47

#define _MM0		0x60
#define _MM1		0x61
#define _MM2		0x62
#define _MM3		0x63
#define _MM4		0x64
#define _MM5		0x65
#define _MM6		0x66
#define _MM7		0x67

#define _XMM0		0x70
#define _XMM1		0x71
#define _XMM2		0x72
#define _XMM3		0x73
#define _XMM4		0x74
#define _XMM5		0x75
#define _XMM6		0x76
#define _XMM7		0x77

#define _ST0		0
#define _ST1		1
#define _ST2		2
#define _ST3		3
#define _ST4		4
#define _ST5		5
#define _ST6		6
#define _ST7		7

#define _r0P(R)		((int)(R) == (int)_NOREG)
#define _rIP(R)		((int)(R) == (int)_RIP)

#define _rC(R)		((R) & 0xf0)
#define _rR(R)		((R) & 0x0f)
#define _rN(R)		((R) & 0x07)
#define _rXP(R)		((R) > 0 && _rR(R) > 7)

#if !defined(_ASM_SAFETY)
#define _r1(R)		_rN(R)
#define _r2(R)		_rN(R)
#define _r4(R)		_rN(R)
#define _r8(R)		_rN(R)
#define _rM(R)		_rN(R)
#define _rX(R)		_rN(R)
#else
#define _r2(R)		( (_rC(R) == 0x30)			? _rN(R) : JITFAIL("16-bit register required"))
#define _r4(R)		( (_rC(R) == 0x40)			? _rN(R) : JITFAIL("32-bit register required"))
#define _rM(R)		( (_rC(R) == 0x60)			? _rN(R) : JITFAIL("MMX register required"))
#define _rX(R)		( (_rC(R) == 0x70)			? _rN(R) : JITFAIL("SSE register required"))
#endif

#define _rbpP(R)	(_rR(R) == _rR(_EBP))
#define _rspP(R)	(_rR(R) == _rR(_ESP))
#define _rbp13P(R)	(_rN(R) == _rN(_EBP))
#define _rsp12P(R)	(_rN(R) == _rN(_ESP))

/*** ASSEMBLER ***/

#define _OFF4(D)        (_jit_UL(D) - _jit_UL(_jit.x.pc))
#define _CKD8(D)        _ck_d(8, ((_uc) _OFF4(D)) )

#define _D8(D)          (_jit_B(0), ((*(_PUC(_jit.x.pc)-1))= _CKD8(D)))
#define _D32(D)         (_jit_I(0), ((*(_PUI(_jit.x.pc)-1))= _OFF4(D)))

#ifndef _ASM_SAFETY
# define _M(M)		(M)
# define _r(R)		(R)
# define _m(M)		(M)
# define _s(S)		(S)
# define _i(I)		(I)
# define _b(B)		(B)
# define _noESP(I,OK)	(OK)
#else
# define _M(M)		(((M)>3) ? JITFAIL("internal error: mod = " #M) : (M))
# define _r(R)		(((R)>7) ? JITFAIL("internal error: reg = " #R) : (R))
# define _m(M)		(((M)>7) ? JITFAIL("internal error: r/m = " #M) : (M))
# define _s(S)		(((S)>3) ? JITFAIL("internal error: memory scale = " #S) : (S))
# define _i(I)		(((I)>7) ? JITFAIL("internal error: memory index = " #I) : (I))
# define _b(B)		(((B)>7) ? JITFAIL("internal error: memory base = "  #B) : (B))
# define _noESP(I,OK)	(((I)==_ESP) ? JITFAIL("illegal index register: %esp") : (OK))
#endif

#define _Mrm(Md,R,M)	_jit_B((_M(Md)<<6)|(_r(R)<<3)|_m(M))
#define _SIB(Sc,I, B)	_jit_B((_s(Sc)<<6)|(_i(I)<<3)|_b(B))

#define _SCL(S)		((((S)==1) ? _b00 : \
			 (((S)==2) ? _b01 : \
			 (((S)==4) ? _b10 : \
			 (((S)==8) ? _b11 : JITFAIL("illegal scale: " #S))))))

/* memory subformats - urgh! */

/* _r_D() is RIP addressing mode if X86_TARGET_64BIT, use _r_DSIB() instead */
#define _r_D(	R, D	  )	(_Mrm(_b00,_rN(R),_b101 )		             ,_jit_I((long)(D)))
#define _r_DSIB(R, D      )	(_Mrm(_b00,_rN(R),_b100 ),_SIB(_SCL(1),_b100 ,_b101 ),_jit_I((long)(D)))
#define _r_0B(	R,   B    )	(_Mrm(_b00,_rN(R),_rA(B))			                   )
#define _r_0BIS(R,   B,I,S)	(_Mrm(_b00,_rN(R),_b100 ),_SIB(_SCL(S),_rA(I),_rA(B))              )
#define _r_1B(	R, D,B    )	(_Mrm(_b01,_rN(R),_rA(B))		             ,_jit_B((long)(D)))
#define _r_1BIS(R, D,B,I,S)	(_Mrm(_b01,_rN(R),_b100 ),_SIB(_SCL(S),_rA(I),_rA(B)),_jit_B((long)(D)))
#define _r_4B(	R, D,B    )	(_Mrm(_b10,_rN(R),_rA(B))		             ,_jit_I((long)(D)))
#define _r_4IS( R, D,I,S)	(_Mrm(_b00,_rN(R),_b100 ),_SIB(_SCL(S),_rA(I),_b101 ),_jit_I((long)(D)))
#define _r_4BIS(R, D,B,I,S)	(_Mrm(_b10,_rN(R),_b100 ),_SIB(_SCL(S),_rA(I),_rA(B)),_jit_I((long)(D)))

#define _r_DB(  R, D,B    )	((_s0P(D) && (!_rbp13P(B)) ? _r_0B  (R,  B    ) : (_s8P(D) ? _r_1B(  R,D,B    ) : _r_4B(  R,D,B    ))))
#define _r_DBIS(R, D,B,I,S)	((_s0P(D) && (!_rbp13P(B)) ? _r_0BIS(R,  B,I,S) : (_s8P(D) ? _r_1BIS(R,D,B,I,S) : _r_4BIS(R,D,B,I,S))))



/* --- Instruction formats ------------------------------------------------- */

/*	 _format						     Opcd	  ModR/M dN(rB,rI,Sc)	  imm... */

#define	 _d16()					   (		  _jit_B(0x66	)				  )
#define	  _O(	     OP				)  (		  _jit_B(  OP	)				  )
#define	  _Or(	     OP,R			)  (		  _jit_B( (OP)|_r(R))				  )
#define	 _OO(	     OP				)  ( _jit_B((OP)>>8), _jit_B( (OP)	)				  )
#define	 _OOr(	     OP,R			)  ( _jit_B((OP)>>8), _jit_B( (OP)|_r(R))				  )
#define	  _Os(	     OP,B			)  (	_s8P(B) ? _jit_B(((OP)|_b10)) : _jit_B(OP)			  )
#define	    _sW(			     W	)  (				       _s8P(W) ? _jit_B(W):_jit_W(W)	  )
#define	    _sL(			     L	)  (				       _s8P(L) ? _jit_B(L):_jit_I(L)	  )
#define	    _sWO(			     W	)  (				       _s8P(W) ?    1 :   2	  )
#define	    _sLO(			     L	)  (				       _s8P(L) ?    1 :   4	  )
#define	  _O_B(	     OP			    ,B	)  (	    _O	    (  OP  )			      ,_jit_B(B)	  )
#define	  _O_W(	     OP			    ,W	)  (	    _O	    (  OP  )			      ,_jit_W(W)	  )
#define	  _O_L(	     OP			    ,L	)  (	    _O	    (  OP  )			      ,_jit_I(L)	  )
#define	  _O_D8(     OP			    ,D	)  (	    _O	    (  OP  )			     ,_D8(D)	  )
#define	  _O_D32(     OP		    ,D	)  (	    _O	    (  OP  )			     ,_D32(D)	  )
#define	 _OO_D32(     OP		    ,D	)  (	   _OO	    (  OP  )			     ,_D32(D)	  )
#define	  _Os_sW(    OP			    ,W	)  (	    _Os	    (  OP,W)			     ,_sW(W)	  )
#define	  _Os_sL(    OP			    ,L	)  (	    _Os	    (  OP,L)			     ,_sL(L)	  )
#define	  _O_W_B(    OP			    ,W,B)  (	    _O	    (  OP  )			      ,_jit_W(W),_jit_B(B))
#define	  _Or_B(     OP,R		    ,B	)  (	    _Or	    (  OP,R)			      ,_jit_B(B)	  )
#define	  _Or_W(     OP,R		    ,W	)  (	    _Or	    (  OP,R)			      ,_jit_W(W)	  )
#define	  _Or_L(     OP,R		    ,L	)  (	    _Or	    (  OP,R)			      ,_jit_I(L)	  )
#define	  _Or_Q(     OP,R		    ,Q	)  (	    _Or	    (  OP,R)			      ,_jit_L(Q)	  )
#define	  _O_Mrm(    OP	 ,MO,R,M		)  (	    _O	    (  OP  ),_Mrm(MO,R,M	    )		  )
#define	 _OO_Mrm(    OP	 ,MO,R,M		)  (	   _OO	    (  OP  ),_Mrm(MO,R,M	    )		  )
#define	  _O_Mrm_B(  OP	 ,MO,R,M	    ,B	)  (	    _O	    (  OP  ),_Mrm(MO,R,M	    ) ,_jit_B(B)	  )
#define	  _O_Mrm_W(  OP	 ,MO,R,M	    ,W	)  (	    _O	    (  OP  ),_Mrm(MO,R,M	    ) ,_jit_W(W)	  )
#define	  _O_Mrm_L(  OP	 ,MO,R,M	    ,L	)  (	    _O	    (  OP  ),_Mrm(MO,R,M	    ) ,_jit_I(L)	  )
#define	 _OO_Mrm_B(  OP	 ,MO,R,M	    ,B	)  (	   _OO	    (  OP  ),_Mrm(MO,R,M	    ) ,_jit_B(B)	  )
#define	  _Os_Mrm_sW(OP	 ,MO,R,M	    ,W	)  (	    _Os	    (  OP,W),_Mrm(MO,R,M	    ),_sW(W)	  )
#define	  _Os_Mrm_sL(OP	 ,MO,R,M	    ,L	)  (	    _Os	    (  OP,L),_Mrm(MO,R,M	    ),_sL(L)	  )
#define	  _O_r_X(    OP	    ,R	,MD,MB,MI,MS	)  (	    _O	    (  OP  ),_r_X(   R	,MD,MB,MI,MS,0)		  )
#define	 _OO_r_X(    OP	    ,R	,MD,MB,MI,MS	)  (	   _OO	    (  OP  ),_r_X(   R	,MD,MB,MI,MS,0)		  )
#define	  _O_r_X_B(  OP	    ,R	,MD,MB,MI,MS,B	)  (	    _O	    (  OP  ),_r_X(   R	,MD,MB,MI,MS,1) ,_jit_B(B)	  )
#define	  _O_r_X_W(  OP	    ,R	,MD,MB,MI,MS,W	)  (	    _O	    (  OP  ),_r_X(   R	,MD,MB,MI,MS,2) ,_jit_W(W)	  )
#define	  _O_r_X_L(  OP	    ,R	,MD,MB,MI,MS,L	)  (	    _O	    (  OP  ),_r_X(   R	,MD,MB,MI,MS,4) ,_jit_I(L)	  )
#define	 _OO_r_X_B(  OP	    ,R	,MD,MB,MI,MS,B	)  (	   _OO	    (  OP  ),_r_X(   R	,MD,MB,MI,MS,1) ,_jit_B(B)	  )
#define	  _Os_r_X_sW(OP	    ,R	,MD,MB,MI,MS,W	)  (	    _Os	    (  OP,W),_r_X(   R	,MD,MB,MI,MS,_sWO(W)),_sW(W))
#define	  _Os_r_X_sL(OP	    ,R	,MD,MB,MI,MS,L	)  (	    _Os	    (  OP,L),_r_X(   R	,MD,MB,MI,MS,_sLO(L)),_sL(L))
#define	  _O_X_B(    OP		,MD,MB,MI,MS,B	)  (	    _O_r_X_B(  OP	    ,0	,MD,MB,MI,MS	 ,B)	  )
#define	  _O_X_W(    OP		,MD,MB,MI,MS,W	)  (	    _O_r_X_W(  OP	    ,0	,MD,MB,MI,MS	 ,W)	  )
#define	  _O_X_L(    OP		,MD,MB,MI,MS,L	)  (	    _O_r_X_L(  OP	    ,0	,MD,MB,MI,MS	 ,L)	  )


/* --- REX prefixes -------------------------------------------------------- */

#define _VOID()			((void)0)
#define _BIT(X)			(!!(X))
#define _d64(W,R,X,B)		(_jit_B(0x40|(W)<<3|(R)<<2|(X)<<1|(B)))

#define __REXwrxb(L,W,R,X,B)	((W|R|X|B) || (L) ? _d64(W,R,X,B) : _VOID())
#define __REXwrx_(L,W,R,X,MR)	(__REXwrxb(L,W,R,X,_BIT(_rIP(MR)?0:_rXP(MR))))
#define __REXw_x_(L,W,R,X,MR)	(__REXwrx_(L,W,_BIT(_rXP(R)),X,MR))
#define __REX_reg(RR)		(__REXwrxb(0,0,0,00,_BIT(_rXP(RR))))
#define __REX_mem(MB,MI)	(__REXwrxb(0,0,0,_BIT(_rXP(MI)),_BIT(_rXP(MB))))

/* FIXME: can't mix new (SPL,BPL,SIL,DIL) with (AH,BH,CH,DH) */
#define _REXBrr(RR,MR)		_m64(__REXw_x_(_r1e8lP(RR)||_r1e8lP(MR),0,RR,0,MR))
#define _REXBmr(MB,MI,RD)	_m64(__REXw_x_(_r1e8lP(RD)||_r1e8lP(MB),0,RD,_BIT(_rXP(MI)),MB))
#define _REXBrm(RS,MB,MI)	_REXBmr(MB,MI,RS)

#define _REXBLrr(RR,MR)		_m64(__REXw_x_(_r1e8lP(MR),0,RR,0,MR))
#define _REXLrr(RR,MR)		_m64(__REXw_x_(0,0,RR,0,MR))
#define _REXLmr(MB,MI,RD)	_m64(__REXw_x_(0,0,RD,_BIT(_rXP(MI)),MB))
#define _REXLrm(RS,MB,MI)	_REXLmr(MB,MI,RS)
#define _REXLr(RR)		_m64(__REX_reg(RR))
#define _REXLm(MB,MI)		_m64(__REX_mem(MB,MI))

#define _REXQrr(RR,MR)		_m64only(__REXw_x_(0,1,RR,0,MR))
#define _REXQmr(MB,MI,RD)	_m64only(__REXw_x_(0,1,RD,_BIT(_rXP(MI)),MB))
#define _REXQrm(RS,MB,MI)	_REXQmr(MB,MI,RS)
#define _REXQr(RR)		_m64only(__REX_reg(RR))
#define _REXQm(MB,MI)		_m64only(__REX_mem(MB,MI))


/* ========================================================================= */
/* --- Fully-qualified intrinsic instructions ------------------------------ */
/* ========================================================================= */

/*	OPCODE	+ i	= immediate operand
 *		+ r	= register operand
 *		+ m	= memory operand (disp,base,index,scale)
 *		+ sr/sm	= a star preceding a register or memory
 *		+ 0	= top of stack register (for FPU instructions)
 *
 *	NOTE in x86-64 mode: a memory operand with only a valid
 *	displacement value will lead to the expect absolute mode. If
 *	RIP addressing is necessary, X86_RIP shall be used as the base
 *	register argument.
 */

/* --- ALU instructions ---------------------------------------------------- */

enum {
  X86_ADD = 0,
  X86_OR  = 1,
  X86_ADC = 2,
  X86_SBB = 3,
  X86_AND = 4,
  X86_SUB = 5,
  X86_XOR = 6,
  X86_CMP = 7,
};

/*									_format		Opcd		,Mod ,r	    ,m		,mem=dsp+sib	,imm... */

#define _ALUBrr(OP,RS, RD)		(_REXBrr(RS, RD),		_O_Mrm		(((OP) << 3)	,_b11,_r1(RS),_r1(RD)				))
#define _ALUBmr(OP, MD, MB, MI, MS, RD)	(_REXBmr(MB, MI, RD),		_O_r_X		(((OP) << 3) + 2,_r1(RD)		,MD,MB,MI,MS		))
#define _ALUBrm(OP, RS, MD, MB, MI, MS)	(_REXBrm(RS, MB, MI),		_O_r_X		(((OP) << 3)	,    ,_r1(RS)		,MD,MB,MI,MS		))
#define _ALUBir(OP, IM, RD)		((RD) == _AL ? \
					(_REXBrr(0, RD),		_O_B		(((OP) << 3) + 4					,_su8(IM))) : \
					(_REXBrr(0, RD),		_O_Mrm_B	(0x80		,_b11,OP     ,_r1(RD)			,_su8(IM))) )
#define _ALUBim(OP, IM, MD, MB, MI, MS)	(_REXBrm(0, MB, MI),		_O_r_X_B	(0x80		     ,OP		,MD,MB,MI,MS	,_su8(IM)))

#define _ALUWrr(OP, RS, RD)		(_d16(), _REXLrr(RS, RD),	_O_Mrm		(((OP) << 3) + 1,_b11,_r2(RS),_r2(RD)				))
#define _ALUWmr(OP, MD, MB, MI, MS, RD)	(_d16(), _REXLmr(MB, MI, RD),	_O_r_X		(((OP) << 3) + 3     ,_r2(RD)		,MD,MB,MI,MS		))
#define _ALUWrm(OP, RS, MD, MB, MI, MS)	(_d16(), _REXLrm(RS, MB, MI),	_O_r_X		(((OP) << 3) + 1     ,_r2(RS)		,MD,MB,MI,MS		))
#define _ALUWir(OP, IM, RD)		((RD) == _AX ? \
					(_d16(), _REXLrr(0, RD),	_O_W		(((OP) << 3) + 5					,_su16(IM))) : \
					(_d16(), _REXLrr(0, RD),	_Os_Mrm_sW	(0x81		,_b11,OP     ,_r2(RD)			,_su16(IM))) )
#define _ALUWim(OP, IM, MD, MB, MI, MS)	(_d16(), _REXLrm(0, MB, MI),	_Os_r_X_sW	(0x81		     ,OP		,MD,MB,MI,MS	,_su16(IM)))

#define _ALULrr(OP, RS, RD)		(_REXLrr(RS, RD),		_O_Mrm		(((OP) << 3) + 1,_b11,_r4(RS),_r4(RD)				))
#define _ALULmr(OP, MD, MB, MI, MS, RD)	(_REXLmr(MB, MI, RD),		_O_r_X		(((OP) << 3) + 3     ,_r4(RD)		,MD,MB,MI,MS		))
#define _ALULrm(OP, RS, MD, MB, MI, MS)	(_REXLrm(RS, MB, MI),		_O_r_X		(((OP) << 3) + 1     ,_r4(RS)		,MD,MB,MI,MS		))
#define _ALULir(OP, IM, RD)		(!_s8P(IM) && (RD) == _EAX ? \
					(_REXLrr(0, RD),		_O_L		(((OP) << 3) + 5					,IM	)) : \
					(_REXLrr(0, RD),		_Os_Mrm_sL	(0x81		,_b11,OP     ,_r4(RD)			,IM	)) )
#define _ALULim(OP, IM, MD, MB, MI, MS)	(_REXLrm(0, MB, MI),		_Os_r_X_sL	(0x81		     ,OP		,MD,MB,MI,MS	,IM	))

#define _ALUQrr(OP, RS, RD)		(_REXQrr(RS, RD),		_O_Mrm		(((OP) << 3) + 1,_b11,_r8(RS),_r8(RD)				))
#define _ALUQmr(OP, MD, MB, MI, MS, RD)	(_REXQmr(MB, MI, RD),		_O_r_X		(((OP) << 3) + 3     ,_r8(RD)		,MD,MB,MI,MS		))
#define _ALUQrm(OP, RS, MD, MB, MI, MS)	(_REXQrm(RS, MB, MI),		_O_r_X		(((OP) << 3) + 1     ,_r8(RS)		,MD,MB,MI,MS		))
#define _ALUQir(OP, IM, RD)		(!_s8P(IM) && (RD) == _RAX ? \
					(_REXQrr(0, RD),		_O_L		(((OP) << 3) + 5					,IM	)) : \
					(_REXQrr(0, RD),		_Os_Mrm_sL	(0x81		,_b11,OP     ,_r8(RD)			,IM	)) )
#define _ALUQim(OP, IM, MD, MB, MI, MS)	(_REXQrm(0, MB, MI),		_Os_r_X_sL	(0x81		     ,OP		,MD,MB,MI,MS	,IM	))

#define ADCBrr(RS, RD)			_ALUBrr(X86_ADC, RS, RD)
#define ADCBmr(MD, MB, MI, MS, RD)	_ALUBmr(X86_ADC, MD, MB, MI, MS, RD)
#define ADCBrm(RS, MD, MB, MI, MS)	_ALUBrm(X86_ADC, RS, MD, MB, MI, MS)
#define ADCBir(IM, RD)			_ALUBir(X86_ADC, IM, RD)
#define ADCBim(IM, MD, MB, MI, MS)	_ALUBim(X86_ADC, IM, MD, MB, MI, MS)

#define ADCWrr(RS, RD)			_ALUWrr(X86_ADC, RS, RD)
#define ADCWmr(MD, MB, MI, MS, RD)	_ALUWmr(X86_ADC, MD, MB, MI, MS, RD)
#define ADCWrm(RS, MD, MB, MI, MS)	_ALUWrm(X86_ADC, RS, MD, MB, MI, MS)
#define ADCWir(IM, RD)			_ALUWir(X86_ADC, IM, RD)
#define ADCWim(IM, MD, MB, MI, MS)	_ALUWim(X86_ADC, IM, MD, MB, MI, MS)

#define ADCLrr(RS, RD)			_ALULrr(X86_ADC, RS, RD)
#define ADCLmr(MD, MB, MI, MS, RD)	_ALULmr(X86_ADC, MD, MB, MI, MS, RD)
#define ADCLrm(RS, MD, MB, MI, MS)	_ALULrm(X86_ADC, RS, MD, MB, MI, MS)
#define ADCLir(IM, RD)			_ALULir(X86_ADC, IM, RD)
#define ADCLim(IM, MD, MB, MI, MS)	_ALULim(X86_ADC, IM, MD, MB, MI, MS)


#define ADDBrr(RS, RD)			_ALUBrr(X86_ADD, RS, RD)
#define ADDBmr(MD, MB, MI, MS, RD)	_ALUBmr(X86_ADD, MD, MB, MI, MS, RD)
#define ADDBrm(RS, MD, MB, MI, MS)	_ALUBrm(X86_ADD, RS, MD, MB, MI, MS)
#define ADDBir(IM, RD)			_ALUBir(X86_ADD, IM, RD)
#define ADDBim(IM, MD, MB, MI, MS)	_ALUBim(X86_ADD, IM, MD, MB, MI, MS)

#define ADDWrr(RS, RD)			_ALUWrr(X86_ADD, RS, RD)
#define ADDWmr(MD, MB, MI, MS, RD)	_ALUWmr(X86_ADD, MD, MB, MI, MS, RD)
#define ADDWrm(RS, MD, MB, MI, MS)	_ALUWrm(X86_ADD, RS, MD, MB, MI, MS)
#define ADDWir(IM, RD)			_ALUWir(X86_ADD, IM, RD)
#define ADDWim(IM, MD, MB, MI, MS)	_ALUWim(X86_ADD, IM, MD, MB, MI, MS)

#define ADDLrr(RS, RD)			_ALULrr(X86_ADD, RS, RD)
#define ADDLmr(MD, MB, MI, MS, RD)	_ALULmr(X86_ADD, MD, MB, MI, MS, RD)
#define ADDLrm(RS, MD, MB, MI, MS)	_ALULrm(X86_ADD, RS, MD, MB, MI, MS)
#define ADDLir(IM, RD)			_ALULir(X86_ADD, IM, RD)
#define ADDLim(IM, MD, MB, MI, MS)	_ALULim(X86_ADD, IM, MD, MB, MI, MS)


#define ANDBrr(RS, RD)			_ALUBrr(X86_AND, RS, RD)
#define ANDBmr(MD, MB, MI, MS, RD)	_ALUBmr(X86_AND, MD, MB, MI, MS, RD)
#define ANDBrm(RS, MD, MB, MI, MS)	_ALUBrm(X86_AND, RS, MD, MB, MI, MS)
#define ANDBir(IM, RD)			_ALUBir(X86_AND, IM, RD)
#define ANDBim(IM, MD, MB, MI, MS)	_ALUBim(X86_AND, IM, MD, MB, MI, MS)

#define ANDWrr(RS, RD)			_ALUWrr(X86_AND, RS, RD)
#define ANDWmr(MD, MB, MI, MS, RD)	_ALUWmr(X86_AND, MD, MB, MI, MS, RD)
#define ANDWrm(RS, MD, MB, MI, MS)	_ALUWrm(X86_AND, RS, MD, MB, MI, MS)
#define ANDWir(IM, RD)			_ALUWir(X86_AND, IM, RD)
#define ANDWim(IM, MD, MB, MI, MS)	_ALUWim(X86_AND, IM, MD, MB, MI, MS)

#define ANDLrr(RS, RD)			_ALULrr(X86_AND, RS, RD)
#define ANDLmr(MD, MB, MI, MS, RD)	_ALULmr(X86_AND, MD, MB, MI, MS, RD)
#define ANDLrm(RS, MD, MB, MI, MS)	_ALULrm(X86_AND, RS, MD, MB, MI, MS)
#define ANDLir(IM, RD)			_ALULir(X86_AND, IM, RD)
#define ANDLim(IM, MD, MB, MI, MS)	_ALULim(X86_AND, IM, MD, MB, MI, MS)


#define CMPBrr(RS, RD)			_ALUBrr(X86_CMP, RS, RD)
#define CMPBmr(MD, MB, MI, MS, RD)	_ALUBmr(X86_CMP, MD, MB, MI, MS, RD)
#define CMPBrm(RS, MD, MB, MI, MS)	_ALUBrm(X86_CMP, RS, MD, MB, MI, MS)
#define CMPBir(IM, RD)			_ALUBir(X86_CMP, IM, RD)
#define CMPBim(IM, MD, MB, MI, MS)	_ALUBim(X86_CMP, IM, MD, MB, MI, MS)

#define CMPWrr(RS, RD)			_ALUWrr(X86_CMP, RS, RD)
#define CMPWmr(MD, MB, MI, MS, RD)	_ALUWmr(X86_CMP, MD, MB, MI, MS, RD)
#define CMPWrm(RS, MD, MB, MI, MS)	_ALUWrm(X86_CMP, RS, MD, MB, MI, MS)
#define CMPWir(IM, RD)			_ALUWir(X86_CMP, IM, RD)
#define CMPWim(IM, MD, MB, MI, MS)	_ALUWim(X86_CMP, IM, MD, MB, MI, MS)

#define CMPLrr(RS, RD)			_ALULrr(X86_CMP, RS, RD)
#define CMPLmr(MD, MB, MI, MS, RD)	_ALULmr(X86_CMP, MD, MB, MI, MS, RD)
#define CMPLrm(RS, MD, MB, MI, MS)	_ALULrm(X86_CMP, RS, MD, MB, MI, MS)
#define CMPLir(IM, RD)			_ALULir(X86_CMP, IM, RD)
#define CMPLim(IM, MD, MB, MI, MS)	_ALULim(X86_CMP, IM, MD, MB, MI, MS)


#define ORBrr(RS, RD)			_ALUBrr(X86_OR, RS, RD)
#define ORBmr(MD, MB, MI, MS, RD)	_ALUBmr(X86_OR, MD, MB, MI, MS, RD)
#define ORBrm(RS, MD, MB, MI, MS)	_ALUBrm(X86_OR, RS, MD, MB, MI, MS)
#define ORBir(IM, RD)			_ALUBir(X86_OR, IM, RD)
#define ORBim(IM, MD, MB, MI, MS)	_ALUBim(X86_OR, IM, MD, MB, MI, MS)

#define ORWrr(RS, RD)			_ALUWrr(X86_OR, RS, RD)
#define ORWmr(MD, MB, MI, MS, RD)	_ALUWmr(X86_OR, MD, MB, MI, MS, RD)
#define ORWrm(RS, MD, MB, MI, MS)	_ALUWrm(X86_OR, RS, MD, MB, MI, MS)
#define ORWir(IM, RD)			_ALUWir(X86_OR, IM, RD)
#define ORWim(IM, MD, MB, MI, MS)	_ALUWim(X86_OR, IM, MD, MB, MI, MS)

#define ORLrr(RS, RD)			_ALULrr(X86_OR, RS, RD)
#define ORLmr(MD, MB, MI, MS, RD)	_ALULmr(X86_OR, MD, MB, MI, MS, RD)
#define ORLrm(RS, MD, MB, MI, MS)	_ALULrm(X86_OR, RS, MD, MB, MI, MS)
#define ORLir(IM, RD)			_ALULir(X86_OR, IM, RD)
#define ORLim(IM, MD, MB, MI, MS)	_ALULim(X86_OR, IM, MD, MB, MI, MS)


#define SBBBrr(RS, RD)			_ALUBrr(X86_SBB, RS, RD)
#define SBBBmr(MD, MB, MI, MS, RD)	_ALUBmr(X86_SBB, MD, MB, MI, MS, RD)
#define SBBBrm(RS, MD, MB, MI, MS)	_ALUBrm(X86_SBB, RS, MD, MB, MI, MS)
#define SBBBir(IM, RD)			_ALUBir(X86_SBB, IM, RD)
#define SBBBim(IM, MD, MB, MI, MS)	_ALUBim(X86_SBB, IM, MD, MB, MI, MS)

#define SBBWrr(RS, RD)			_ALUWrr(X86_SBB, RS, RD)
#define SBBWmr(MD, MB, MI, MS, RD)	_ALUWmr(X86_SBB, MD, MB, MI, MS, RD)
#define SBBWrm(RS, MD, MB, MI, MS)	_ALUWrm(X86_SBB, RS, MD, MB, MI, MS)
#define SBBWir(IM, RD)			_ALUWir(X86_SBB, IM, RD)
#define SBBWim(IM, MD, MB, MI, MS)	_ALUWim(X86_SBB, IM, MD, MB, MI, MS)

#define SBBLrr(RS, RD)			_ALULrr(X86_SBB, RS, RD)
#define SBBLmr(MD, MB, MI, MS, RD)	_ALULmr(X86_SBB, MD, MB, MI, MS, RD)
#define SBBLrm(RS, MD, MB, MI, MS)	_ALULrm(X86_SBB, RS, MD, MB, MI, MS)
#define SBBLir(IM, RD)			_ALULir(X86_SBB, IM, RD)
#define SBBLim(IM, MD, MB, MI, MS)	_ALULim(X86_SBB, IM, MD, MB, MI, MS)


#define SUBBrr(RS, RD)			_ALUBrr(X86_SUB, RS, RD)
#define SUBBmr(MD, MB, MI, MS, RD)	_ALUBmr(X86_SUB, MD, MB, MI, MS, RD)
#define SUBBrm(RS, MD, MB, MI, MS)	_ALUBrm(X86_SUB, RS, MD, MB, MI, MS)
#define SUBBir(IM, RD)			_ALUBir(X86_SUB, IM, RD)
#define SUBBim(IM, MD, MB, MI, MS)	_ALUBim(X86_SUB, IM, MD, MB, MI, MS)

#define SUBWrr(RS, RD)			_ALUWrr(X86_SUB, RS, RD)
#define SUBWmr(MD, MB, MI, MS, RD)	_ALUWmr(X86_SUB, MD, MB, MI, MS, RD)
#define SUBWrm(RS, MD, MB, MI, MS)	_ALUWrm(X86_SUB, RS, MD, MB, MI, MS)
#define SUBWir(IM, RD)			_ALUWir(X86_SUB, IM, RD)
#define SUBWim(IM, MD, MB, MI, MS)	_ALUWim(X86_SUB, IM, MD, MB, MI, MS)

#define SUBLrr(RS, RD)			_ALULrr(X86_SUB, RS, RD)
#define SUBLmr(MD, MB, MI, MS, RD)	_ALULmr(X86_SUB, MD, MB, MI, MS, RD)
#define SUBLrm(RS, MD, MB, MI, MS)	_ALULrm(X86_SUB, RS, MD, MB, MI, MS)
#define SUBLir(IM, RD)			_ALULir(X86_SUB, IM, RD)
#define SUBLim(IM, MD, MB, MI, MS)	_ALULim(X86_SUB, IM, MD, MB, MI, MS)


#define XORBrr(RS, RD)			_ALUBrr(X86_XOR, RS, RD)
#define XORBmr(MD, MB, MI, MS, RD)	_ALUBmr(X86_XOR, MD, MB, MI, MS, RD)
#define XORBrm(RS, MD, MB, MI, MS)	_ALUBrm(X86_XOR, RS, MD, MB, MI, MS)
#define XORBir(IM, RD)			_ALUBir(X86_XOR, IM, RD)
#define XORBim(IM, MD, MB, MI, MS)	_ALUBim(X86_XOR, IM, MD, MB, MI, MS)

#define XORWrr(RS, RD)			_ALUWrr(X86_XOR, RS, RD)
#define XORWmr(MD, MB, MI, MS, RD)	_ALUWmr(X86_XOR, MD, MB, MI, MS, RD)
#define XORWrm(RS, MD, MB, MI, MS)	_ALUWrm(X86_XOR, RS, MD, MB, MI, MS)
#define XORWir(IM, RD)			_ALUWir(X86_XOR, IM, RD)
#define XORWim(IM, MD, MB, MI, MS)	_ALUWim(X86_XOR, IM, MD, MB, MI, MS)

#define XORLrr(RS, RD)			_ALULrr(X86_XOR, RS, RD)
#define XORLmr(MD, MB, MI, MS, RD)	_ALULmr(X86_XOR, MD, MB, MI, MS, RD)
#define XORLrm(RS, MD, MB, MI, MS)	_ALULrm(X86_XOR, RS, MD, MB, MI, MS)
#define XORLir(IM, RD)			_ALULir(X86_XOR, IM, RD)
#define XORLim(IM, MD, MB, MI, MS)	_ALULim(X86_XOR, IM, MD, MB, MI, MS)



/* --- Shift/Rotate instructions ------------------------------------------- */

enum {
  X86_ROL = 0,
  X86_ROR = 1,
  X86_RCL = 2,
  X86_RCR = 3,
  X86_SHL = 4,
  X86_SHR = 5,
  X86_SAR = 7,
};

/*									_format		Opcd		,Mod ,r	    ,m		,mem=dsp+sib	,imm... */

#define _ROTSHIBir(OP,IM,RD)		((IM) == 1 ? \
					(_REXBrr(0, RD),		_O_Mrm		(0xd0		,_b11,OP,_r1(RD)				)) : \
					(_REXBrr(0, RD),		_O_Mrm_B	(0xc0		,_b11,OP,_r1(RD)			,_u8(IM))) )
#define _ROTSHIBim(OP,IM,MD,MB,MI,MS)	((IM) == 1 ? \
					(_REXBrm(0, MB, MI),		_O_r_X		(0xd0		     ,OP		,MD,MB,MI,MS		)) : \
					(_REXBrm(0, MB, MI),		_O_r_X_B	(0xc0		     ,OP		,MD,MB,MI,MS	,_u8(IM))) )
#define _ROTSHIBrr(OP,RS,RD)		(((RS) == _CL) ? \
					(_REXBrr(RS, RD),		_O_Mrm		(0xd2		,_b11,OP,_r1(RD)				)) : \
									JITFAIL("source register must be CL"					)  )
#define _ROTSHIBrm(OP,RS,MD,MB,MI,MS)	(((RS) == _CL) ? \
					(_REXBrm(RS, MB, MI),		_O_r_X		(0xd2		     ,OP		,MD,MB,MI,MS		)) : \
									JITFAIL("source register must be CL"					)  )

#define _ROTSHIWir(OP,IM,RD)		((IM) == 1 ? \
					(_d16(), _REXLrr(0, RD),	_O_Mrm		(0xd1		,_b11,OP,_r2(RD)				)) : \
					(_d16(), _REXLrr(0, RD),	_O_Mrm_B	(0xc1		,_b11,OP,_r2(RD)			,_u8(IM))) )
#define _ROTSHIWim(OP,IM,MD,MB,MI,MS)	((IM) == 1 ? \
					(_d16(), _REXLrm(0, MB, MI),	_O_r_X		(0xd1		     ,OP		,MD,MB,MI,MS		)) : \
					(_d16(), _REXLrm(0, MB, MI),	_O_r_X_B	(0xc1		     ,OP		,MD,MB,MI,MS	,_u8(IM))) )
#define _ROTSHIWrr(OP,RS,RD)		(((RS) == _CL) ? \
					(_d16(), _REXLrr(RS, RD),	_O_Mrm		(0xd3		,_b11,OP,_r2(RD)				)) : \
									JITFAIL("source register must be CL"					)  )
#define _ROTSHIWrm(OP,RS,MD,MB,MI,MS)	(((RS) == _CL) ? \
					(_d16(), _REXLrm(RS, MB, MI),	_O_r_X		(0xd3		     ,OP		,MD,MB,MI,MS		)) : \
									JITFAIL("source register must be CL"					)  )

#define _ROTSHILir(OP,IM,RD)		((IM) == 1 ? \
					(_REXLrr(0, RD),		_O_Mrm		(0xd1		,_b11,OP,_r4(RD)				)) : \
					(_REXLrr(0, RD),		_O_Mrm_B	(0xc1		,_b11,OP,_r4(RD)			,_u8(IM))) )
#define _ROTSHILim(OP,IM,MD,MB,MI,MS)	((IM) == 1 ? \
					(_REXLrm(0, MB, MI),		_O_r_X		(0xd1		     ,OP		,MD,MB,MI,MS		)) : \
					(_REXLrm(0, MB, MI),		_O_r_X_B	(0xc1		     ,OP		,MD,MB,MI,MS	,_u8(IM))) )
#define _ROTSHILrr(OP,RS,RD)		(((RS) == _CL) ? \
					(_REXLrr(RS, RD),		_O_Mrm		(0xd3		,_b11,OP,_r4(RD)				)) : \
									JITFAIL("source register must be CL"					)  )
#define _ROTSHILrm(OP,RS,MD,MB,MI,MS)	(((RS) == _CL) ? \
					(_REXLrm(RS, MB, MI),		_O_r_X		(0xd3		     ,OP		,MD,MB,MI,MS		)) : \
									JITFAIL("source register must be CL"					)  )

#define _ROTSHIQir(OP,IM,RD)		((IM) == 1 ? \
					(_REXQrr(0, RD),		_O_Mrm		(0xd1		,_b11,OP,_r8(RD)				)) : \
					(_REXQrr(0, RD),		_O_Mrm_B	(0xc1		,_b11,OP,_r8(RD)			,_u8(IM))) )
#define _ROTSHIQim(OP,IM,MD,MB,MI,MS)	((IM) == 1 ? \
					(_REXQrm(0, MB, MI),		_O_r_X		(0xd1		     ,OP		,MD,MB,MI,MS		)) : \
					(_REXQrm(0, MB, MI),		_O_r_X_B	(0xc1		     ,OP		,MD,MB,MI,MS	,_u8(IM))) )
#define _ROTSHIQrr(OP,RS,RD)		(((RS) == _CL) ? \
					(_REXQrr(RS, RD),		_O_Mrm		(0xd3		,_b11,OP,_r8(RD)				)) : \
									JITFAIL("source register must be CL"					)  )
#define _ROTSHIQrm(OP,RS,MD,MB,MI,MS)	(((RS) == _CL) ? \
					(_REXQrm(RS, MB, MI),		_O_r_X		(0xd3		     ,OP		,MD,MB,MI,MS		)) : \
									JITFAIL("source register must be CL"					)  )

#define ROLBir(IM, RD)			_ROTSHIBir(X86_ROL, IM, RD)
#define ROLBim(IM, MD, MB, MI, MS)	_ROTSHIBim(X86_ROL, IM, MD, MB, MI, MS)
#define ROLBrr(RS, RD)			_ROTSHIBrr(X86_ROL, RS, RD)
#define ROLBrm(RS, MD, MB, MI, MS)	_ROTSHIBrm(X86_ROL, RS, MD, MB, MI, MS)

#define ROLWir(IM, RD)			_ROTSHIWir(X86_ROL, IM, RD)
#define ROLWim(IM, MD, MB, MI, MS)	_ROTSHIWim(X86_ROL, IM, MD, MB, MI, MS)
#define ROLWrr(RS, RD)			_ROTSHIWrr(X86_ROL, RS, RD)
#define ROLWrm(RS, MD, MB, MI, MS)	_ROTSHIWrm(X86_ROL, RS, MD, MB, MI, MS)

#define ROLLir(IM, RD)			_ROTSHILir(X86_ROL, IM, RD)
#define ROLLim(IM, MD, MB, MI, MS)	_ROTSHILim(X86_ROL, IM, MD, MB, MI, MS)
#define ROLLrr(RS, RD)			_ROTSHILrr(X86_ROL, RS, RD)
#define ROLLrm(RS, MD, MB, MI, MS)	_ROTSHILrm(X86_ROL, RS, MD, MB, MI, MS)


#define RORBir(IM, RD)			_ROTSHIBir(X86_ROR, IM, RD)
#define RORBim(IM, MD, MB, MI, MS)	_ROTSHIBim(X86_ROR, IM, MD, MB, MI, MS)
#define RORBrr(RS, RD)			_ROTSHIBrr(X86_ROR, RS, RD)
#define RORBrm(RS, MD, MB, MI, MS)	_ROTSHIBrm(X86_ROR, RS, MD, MB, MI, MS)

#define RORWir(IM, RD)			_ROTSHIWir(X86_ROR, IM, RD)
#define RORWim(IM, MD, MB, MI, MS)	_ROTSHIWim(X86_ROR, IM, MD, MB, MI, MS)
#define RORWrr(RS, RD)			_ROTSHIWrr(X86_ROR, RS, RD)
#define RORWrm(RS, MD, MB, MI, MS)	_ROTSHIWrm(X86_ROR, RS, MD, MB, MI, MS)

#define RORLir(IM, RD)			_ROTSHILir(X86_ROR, IM, RD)
#define RORLim(IM, MD, MB, MI, MS)	_ROTSHILim(X86_ROR, IM, MD, MB, MI, MS)
#define RORLrr(RS, RD)			_ROTSHILrr(X86_ROR, RS, RD)
#define RORLrm(RS, MD, MB, MI, MS)	_ROTSHILrm(X86_ROR, RS, MD, MB, MI, MS)


#define RCLBir(IM, RD)			_ROTSHIBir(X86_RCL, IM, RD)
#define RCLBim(IM, MD, MB, MI, MS)	_ROTSHIBim(X86_RCL, IM, MD, MB, MI, MS)
#define RCLBrr(RS, RD)			_ROTSHIBrr(X86_RCL, RS, RD)
#define RCLBrm(RS, MD, MB, MI, MS)	_ROTSHIBrm(X86_RCL, RS, MD, MB, MI, MS)

#define RCLWir(IM, RD)			_ROTSHIWir(X86_RCL, IM, RD)
#define RCLWim(IM, MD, MB, MI, MS)	_ROTSHIWim(X86_RCL, IM, MD, MB, MI, MS)
#define RCLWrr(RS, RD)			_ROTSHIWrr(X86_RCL, RS, RD)
#define RCLWrm(RS, MD, MB, MI, MS)	_ROTSHIWrm(X86_RCL, RS, MD, MB, MI, MS)

#define RCLLir(IM, RD)			_ROTSHILir(X86_RCL, IM, RD)
#define RCLLim(IM, MD, MB, MI, MS)	_ROTSHILim(X86_RCL, IM, MD, MB, MI, MS)
#define RCLLrr(RS, RD)			_ROTSHILrr(X86_RCL, RS, RD)
#define RCLLrm(RS, MD, MB, MI, MS)	_ROTSHILrm(X86_RCL, RS, MD, MB, MI, MS)


#define RCRBir(IM, RD)			_ROTSHIBir(X86_RCR, IM, RD)
#define RCRBim(IM, MD, MB, MI, MS)	_ROTSHIBim(X86_RCR, IM, MD, MB, MI, MS)
#define RCRBrr(RS, RD)			_ROTSHIBrr(X86_RCR, RS, RD)
#define RCRBrm(RS, MD, MB, MI, MS)	_ROTSHIBrm(X86_RCR, RS, MD, MB, MI, MS)

#define RCRWir(IM, RD)			_ROTSHIWir(X86_RCR, IM, RD)
#define RCRWim(IM, MD, MB, MI, MS)	_ROTSHIWim(X86_RCR, IM, MD, MB, MI, MS)
#define RCRWrr(RS, RD)			_ROTSHIWrr(X86_RCR, RS, RD)
#define RCRWrm(RS, MD, MB, MI, MS)	_ROTSHIWrm(X86_RCR, RS, MD, MB, MI, MS)

#define RCRLir(IM, RD)			_ROTSHILir(X86_RCR, IM, RD)
#define RCRLim(IM, MD, MB, MI, MS)	_ROTSHILim(X86_RCR, IM, MD, MB, MI, MS)
#define RCRLrr(RS, RD)			_ROTSHILrr(X86_RCR, RS, RD)
#define RCRLrm(RS, MD, MB, MI, MS)	_ROTSHILrm(X86_RCR, RS, MD, MB, MI, MS)


#define SHLBir(IM, RD)			_ROTSHIBir(X86_SHL, IM, RD)
#define SHLBim(IM, MD, MB, MI, MS)	_ROTSHIBim(X86_SHL, IM, MD, MB, MI, MS)
#define SHLBrr(RS, RD)			_ROTSHIBrr(X86_SHL, RS, RD)
#define SHLBrm(RS, MD, MB, MI, MS)	_ROTSHIBrm(X86_SHL, RS, MD, MB, MI, MS)

#define SHLWir(IM, RD)			_ROTSHIWir(X86_SHL, IM, RD)
#define SHLWim(IM, MD, MB, MI, MS)	_ROTSHIWim(X86_SHL, IM, MD, MB, MI, MS)
#define SHLWrr(RS, RD)			_ROTSHIWrr(X86_SHL, RS, RD)
#define SHLWrm(RS, MD, MB, MI, MS)	_ROTSHIWrm(X86_SHL, RS, MD, MB, MI, MS)

#define SHLLir(IM, RD)			_ROTSHILir(X86_SHL, IM, RD)
#define SHLLim(IM, MD, MB, MI, MS)	_ROTSHILim(X86_SHL, IM, MD, MB, MI, MS)
#define SHLLrr(RS, RD)			_ROTSHILrr(X86_SHL, RS, RD)
#define SHLLrm(RS, MD, MB, MI, MS)	_ROTSHILrm(X86_SHL, RS, MD, MB, MI, MS)


#define SHRBir(IM, RD)			_ROTSHIBir(X86_SHR, IM, RD)
#define SHRBim(IM, MD, MB, MI, MS)	_ROTSHIBim(X86_SHR, IM, MD, MB, MI, MS)
#define SHRBrr(RS, RD)			_ROTSHIBrr(X86_SHR, RS, RD)
#define SHRBrm(RS, MD, MB, MI, MS)	_ROTSHIBrm(X86_SHR, RS, MD, MB, MI, MS)

#define SHRWir(IM, RD)			_ROTSHIWir(X86_SHR, IM, RD)
#define SHRWim(IM, MD, MB, MI, MS)	_ROTSHIWim(X86_SHR, IM, MD, MB, MI, MS)
#define SHRWrr(RS, RD)			_ROTSHIWrr(X86_SHR, RS, RD)
#define SHRWrm(RS, MD, MB, MI, MS)	_ROTSHIWrm(X86_SHR, RS, MD, MB, MI, MS)

#define SHRLir(IM, RD)			_ROTSHILir(X86_SHR, IM, RD)
#define SHRLim(IM, MD, MB, MI, MS)	_ROTSHILim(X86_SHR, IM, MD, MB, MI, MS)
#define SHRLrr(RS, RD)			_ROTSHILrr(X86_SHR, RS, RD)
#define SHRLrm(RS, MD, MB, MI, MS)	_ROTSHILrm(X86_SHR, RS, MD, MB, MI, MS)


#define SALBir				SHLBir
#define SALBim				SHLBim
#define SALBrr				SHLBrr
#define SALBrm				SHLBrm

#define SALWir				SHLWir
#define SALWim				SHLWim
#define SALWrr				SHLWrr
#define SALWrm				SHLWrm

#define SALLir				SHLLir
#define SALLim				SHLLim
#define SALLrr				SHLLrr
#define SALLrm				SHLLrm


#define SARBir(IM, RD)			_ROTSHIBir(X86_SAR, IM, RD)
#define SARBim(IM, MD, MB, MI, MS)	_ROTSHIBim(X86_SAR, IM, MD, MB, MI, MS)
#define SARBrr(RS, RD)			_ROTSHIBrr(X86_SAR, RS, RD)
#define SARBrm(RS, MD, MB, MI, MS)	_ROTSHIBrm(X86_SAR, RS, MD, MB, MI, MS)

#define SARWir(IM, RD)			_ROTSHIWir(X86_SAR, IM, RD)
#define SARWim(IM, MD, MB, MI, MS)	_ROTSHIWim(X86_SAR, IM, MD, MB, MI, MS)
#define SARWrr(RS, RD)			_ROTSHIWrr(X86_SAR, RS, RD)
#define SARWrm(RS, MD, MB, MI, MS)	_ROTSHIWrm(X86_SAR, RS, MD, MB, MI, MS)

#define SARLir(IM, RD)			_ROTSHILir(X86_SAR, IM, RD)
#define SARLim(IM, MD, MB, MI, MS)	_ROTSHILim(X86_SAR, IM, MD, MB, MI, MS)
#define SARLrr(RS, RD)			_ROTSHILrr(X86_SAR, RS, RD)
#define SARLrm(RS, MD, MB, MI, MS)	_ROTSHILrm(X86_SAR, RS, MD, MB, MI, MS)



/* --- Bit test instructions ----------------------------------------------- */

enum {
  X86_BT  = 4,
  X86_BTS = 5,
  X86_BTR = 6,
  X86_BTC = 7,
};

/*									_format		Opcd		 ,Mod ,r      ,m	,mem=dsp+sib	,imm... */

#define _BTWir(OP, IM, RD)		(_d16(), _REXLrr(0, RD),	_OO_Mrm_B	(0x0fba		 ,_b11,OP     ,_r2(RD)			,_u8(IM)))
#define _BTWim(OP, IM, MD, MB, MI, MS)	(_d16(), _REXLrm(0, MB, MI),	_OO_r_X_B	(0x0fba		      ,OP		,MD,MB,MI,MS	,_u8(IM)))
#define _BTWrr(OP, RS, RD)		(_d16(), _REXLrr(RS, RD),	_OO_Mrm		(0x0f83|((OP)<<3),_b11,_r2(RS),_r2(RD)				))
#define _BTWrm(OP, RS, MD, MB, MI, MS)	(_d16(), _REXLrm(RS, MB, MI),	_OO_r_X		(0x0f83|((OP)<<3)     ,_r2(RS)		,MD,MB,MI,MS		))

#define _BTLir(OP, IM, RD)		(_REXLrr(0, RD),		_OO_Mrm_B	(0x0fba		 ,_b11,OP     ,_r4(RD)			,_u8(IM)))
#define _BTLim(OP, IM, MD, MB, MI, MS)	(_REXLrm(0, MB, MI),		_OO_r_X_B	(0x0fba		      ,OP		,MD,MB,MI,MS	,_u8(IM)))
#define _BTLrr(OP, RS, RD)		(_REXLrr(RS, RD),		_OO_Mrm		(0x0f83|((OP)<<3),_b11,_r4(RS),_r4(RD)				))
#define _BTLrm(OP, RS, MD, MB, MI, MS)	(_REXLrm(RS, MB, MI),		_OO_r_X		(0x0f83|((OP)<<3)     ,_r4(RS)		,MD,MB,MI,MS		))

#define _BTQir(OP, IM, RD)		(_REXQrr(0, RD),		_OO_Mrm_B	(0x0fba		 ,_b11,OP     ,_r8(RD)			,_u8(IM)))
#define _BTQim(OP, IM, MD, MB, MI, MS)	(_REXQrm(0, MB, MI),		_OO_r_X_B	(0x0fba		      ,OP		,MD,MB,MI,MS	,_u8(IM)))
#define _BTQrr(OP, RS, RD)		(_REXQrr(RS, RD),		_OO_Mrm		(0x0f83|((OP)<<3),_b11,_r8(RS),_r8(RD)				))
#define _BTQrm(OP, RS, MD, MB, MI, MS)	(_REXQrm(RS, MB, MI),		_OO_r_X		(0x0f83|((OP)<<3)     ,_r8(RS)		,MD,MB,MI,MS		))

#define BTWir(IM, RD)			_BTWir(X86_BT, IM, RD)
#define BTWim(IM, MD, MB, MI, MS)	_BTWim(X86_BT, IM, MD, MI, MS)
#define BTWrr(RS, RD)			_BTWrr(X86_BT, RS, RD)
#define BTWrm(RS, MD, MB, MI, MS)	_BTWrm(X86_BT, RS, MD, MB, MI, MS)

#define BTLir(IM, RD)			_BTLir(X86_BT, IM, RD)
#define BTLim(IM, MD, MB, MI, MS)	_BTLim(X86_BT, IM, MD, MB, MI, MS)
#define BTLrr(RS, RD)			_BTLrr(X86_BT, RS, RD)
#define BTLrm(RS, MD, MB, MI, MS)	_BTLrm(X86_BT, RS, MD, MB, MI, MS)


#define BTCWir(IM, RD)			_BTWir(X86_BTC, IM, RD)
#define BTCWim(IM, MD, MB, MI, MS)	_BTWim(X86_BTC, IM, MD, MI, MS)
#define BTCWrr(RS, RD)			_BTWrr(X86_BTC, RS, RD)
#define BTCWrm(RS, MD, MB, MI, MS)	_BTWrm(X86_BTC, RS, MD, MB, MI, MS)

#define BTCLir(IM, RD)			_BTLir(X86_BTC, IM, RD)
#define BTCLim(IM, MD, MB, MI, MS)	_BTLim(X86_BTC, IM, MD, MB, MI, MS)
#define BTCLrr(RS, RD)			_BTLrr(X86_BTC, RS, RD)
#define BTCLrm(RS, MD, MB, MI, MS)	_BTLrm(X86_BTC, RS, MD, MB, MI, MS)


#define BTRWir(IM, RD)			_BTWir(X86_BTR, IM, RD)
#define BTRWim(IM, MD, MB, MI, MS)	_BTWim(X86_BTR, IM, MD, MI, MS)
#define BTRWrr(RS, RD)			_BTWrr(X86_BTR, RS, RD)
#define BTRWrm(RS, MD, MB, MI, MS)	_BTWrm(X86_BTR, RS, MD, MB, MI, MS)

#define BTRLir(IM, RD)			_BTLir(X86_BTR, IM, RD)
#define BTRLim(IM, MD, MB, MI, MS)	_BTLim(X86_BTR, IM, MD, MB, MI, MS)
#define BTRLrr(RS, RD)			_BTLrr(X86_BTR, RS, RD)
#define BTRLrm(RS, MD, MB, MI, MS)	_BTLrm(X86_BTR, RS, MD, MB, MI, MS)


#define BTSWir(IM, RD)			_BTWir(X86_BTS, IM, RD)
#define BTSWim(IM, MD, MB, MI, MS)	_BTWim(X86_BTS, IM, MD, MI, MS)
#define BTSWrr(RS, RD)			_BTWrr(X86_BTS, RS, RD)
#define BTSWrm(RS, MD, MB, MI, MS)	_BTWrm(X86_BTS, RS, MD, MB, MI, MS)

#define BTSLir(IM, RD)			_BTLir(X86_BTS, IM, RD)
#define BTSLim(IM, MD, MB, MI, MS)	_BTLim(X86_BTS, IM, MD, MB, MI, MS)
#define BTSLrr(RS, RD)			_BTLrr(X86_BTS, RS, RD)
#define BTSLrm(RS, MD, MB, MI, MS)	_BTLrm(X86_BTS, RS, MD, MB, MI, MS)



/* --- Move instructions --------------------------------------------------- */

/*									_format		Opcd		,Mod ,r	    ,m		,mem=dsp+sib	,imm... */

#define MOVBrr(RS, RD)			(_REXBrr(RS, RD),		_O_Mrm		(0x88		,_b11,_r1(RS),_r1(RD)				))
#define MOVBmr(MD, MB, MI, MS, RD)	(_REXBmr(MB, MI, RD),		_O_r_X		(0x8a		     ,_r1(RD)		,MD,MB,MI,MS		))
#define MOVBrm(RS, MD, MB, MI, MS)	(_REXBrm(RS, MB, MI),		_O_r_X		(0x88		     ,_r1(RS)		,MD,MB,MI,MS		))
#define MOVBir(IM,  R)			(_REXBrr(0, R),			_Or_B		(0xb0,_r1(R)						,_su8(IM)))
#define MOVBim(IM, MD, MB, MI, MS)	(_REXBrm(0, MB, MI),		_O_X_B		(0xc6					,MD,MB,MI,MS	,_su8(IM)))

#define MOVWrr(RS, RD)			(_d16(), _REXLrr(RS, RD),	_O_Mrm		(0x89		,_b11,_r2(RS),_r2(RD)				))
#define MOVWmr(MD, MB, MI, MS, RD)	(_d16(), _REXLmr(MB, MI, RD),	_O_r_X		(0x8b		     ,_r2(RD)		,MD,MB,MI,MS		))
#define MOVWrm(RS, MD, MB, MI, MS)	(_d16(), _REXLrm(RS, MB, MI),	_O_r_X		(0x89		     ,_r2(RS)		,MD,MB,MI,MS		))
#define MOVWir(IM,  R)			(_d16(), _REXLrr(0, R),		_Or_W		(0xb8,_r2(R)						,_su16(IM)))
#define MOVWim(IM, MD, MB, MI, MS)	(_d16(), _REXLrm(0, MB, MI),	_O_X_W		(0xc7					,MD,MB,MI,MS	,_su16(IM)))

#define MOVLrr(RS, RD)			(_REXLrr(RS, RD),		_O_Mrm		(0x89		,_b11,_r4(RS),_r4(RD)				))
#define MOVLmr(MD, MB, MI, MS, RD)	(_REXLmr(MB, MI, RD),		_O_r_X		(0x8b		     ,_r4(RD)		,MD,MB,MI,MS		))
#define MOVLrm(RS, MD, MB, MI, MS)	(_REXLrm(RS, MB, MI),		_O_r_X		(0x89		     ,_r4(RS)		,MD,MB,MI,MS		))
#define MOVLir(IM,  R)			(_REXLrr(0, R),			_Or_L		(0xb8,_r4(R)						,IM	))
#define MOVLim(IM, MD, MB, MI, MS)	(_REXLrm(0, MB, MI),		_O_X_L		(0xc7					,MD,MB,MI,MS	,IM	))



/* --- Unary and Multiply/Divide instructions ------------------------------ */

enum {
  X86_NOT  = 2,
  X86_NEG  = 3,
  X86_MUL  = 4,
  X86_IMUL = 5,
  X86_DIV  = 6,
  X86_IDIV = 7,
};

/*									_format		Opcd		,Mod ,r	    ,m		,mem=dsp+sib	,imm... */

#define _UNARYBr(OP, RS)		(_REXBrr(0, RS),		_O_Mrm		(0xf6		,_b11,OP    ,_r1(RS)				))
#define _UNARYBm(OP, MD, MB, MI, MS)	(_REXBrm(0, MB, MI),		_O_r_X		(0xf6		     ,OP		,MD,MB,MI,MS		))
#define _UNARYWr(OP, RS)		(_d16(), _REXLrr(0, RS),	_O_Mrm		(0xf7		,_b11,OP    ,_r2(RS)				))
#define _UNARYWm(OP, MD, MB, MI, MS)	(_d16(), _REXLmr(MB, MI, 0),	_O_r_X		(0xf7		     ,OP		,MD,MB,MI,MS		))
#define _UNARYLr(OP, RS)		(_REXLrr(0, RS),		_O_Mrm		(0xf7		,_b11,OP    ,_r4(RS)				))
#define _UNARYLm(OP, MD, MB, MI, MS)	(_REXLmr(MB, MI, 0),		_O_r_X		(0xf7		     ,OP		,MD,MB,MI,MS		))
#define _UNARYQr(OP, RS)		(_REXQrr(0, RS),		_O_Mrm		(0xf7		,_b11,OP    ,_r8(RS)				))
#define _UNARYQm(OP, MD, MB, MI, MS)	(_REXQmr(MB, MI, 0),		_O_r_X		(0xf7		     ,OP		,MD,MB,MI,MS		))

#define NOTBr(RS)			_UNARYBr(X86_NOT, RS)
#define NOTBm(MD, MB, MI, MS)		_UNARYBm(X86_NOT, MD, MB, MI, MS)
#define NOTWr(RS)			_UNARYWr(X86_NOT, RS)
#define NOTWm(MD, MB, MI, MS)		_UNARYWm(X86_NOT, MD, MB, MI, MS)
#define NOTLr(RS)			_UNARYLr(X86_NOT, RS)
#define NOTLm(MD, MB, MI, MS)		_UNARYLm(X86_NOT, MD, MB, MI, MS)

#define NEGBr(RS)			_UNARYBr(X86_NEG, RS)
#define NEGBm(MD, MB, MI, MS)		_UNARYBm(X86_NEG, MD, MB, MI, MS)
#define NEGWr(RS)			_UNARYWr(X86_NEG, RS)
#define NEGWm(MD, MB, MI, MS)		_UNARYWm(X86_NEG, MD, MB, MI, MS)
#define NEGLr(RS)			_UNARYLr(X86_NEG, RS)
#define NEGLm(MD, MB, MI, MS)		_UNARYLm(X86_NEG, MD, MB, MI, MS)

#define MULBr(RS)			_UNARYBr(X86_MUL, RS)
#define MULBm(MD, MB, MI, MS)		_UNARYBm(X86_MUL, MD, MB, MI, MS)
#define MULWr(RS)			_UNARYWr(X86_MUL, RS)
#define MULWm(MD, MB, MI, MS)		_UNARYWm(X86_MUL, MD, MB, MI, MS)
#define MULLr(RS)			_UNARYLr(X86_MUL, RS)
#define MULLm(MD, MB, MI, MS)		_UNARYLm(X86_MUL, MD, MB, MI, MS)

#define IMULBr(RS)			_UNARYBr(X86_IMUL, RS)
#define IMULBm(MD, MB, MI, MS)		_UNARYBm(X86_IMUL, MD, MB, MI, MS)
#define IMULWr(RS)			_UNARYWr(X86_IMUL, RS)
#define IMULWm(MD, MB, MI, MS)		_UNARYWm(X86_IMUL, MD, MB, MI, MS)
#define IMULLr(RS)			_UNARYLr(X86_IMUL, RS)
#define IMULLm(MD, MB, MI, MS)		_UNARYLm(X86_IMUL, MD, MB, MI, MS)

#define DIVBr(RS)			_UNARYBr(X86_DIV, RS)
#define DIVBm(MD, MB, MI, MS)		_UNARYBm(X86_DIV, MD, MB, MI, MS)
#define DIVWr(RS)			_UNARYWr(X86_DIV, RS)
#define DIVWm(MD, MB, MI, MS)		_UNARYWm(X86_DIV, MD, MB, MI, MS)
#define DIVLr(RS)			_UNARYLr(X86_DIV, RS)
#define DIVLm(MD, MB, MI, MS)		_UNARYLm(X86_DIV, MD, MB, MI, MS)

#define IDIVBr(RS)			_UNARYBr(X86_IDIV, RS)
#define IDIVBm(MD, MB, MI, MS)		_UNARYBm(X86_IDIV, MD, MB, MI, MS)
#define IDIVWr(RS)			_UNARYWr(X86_IDIV, RS)
#define IDIVWm(MD, MB, MI, MS)		_UNARYWm(X86_IDIV, MD, MB, MI, MS)
#define IDIVLr(RS)			_UNARYLr(X86_IDIV, RS)
#define IDIVLm(MD, MB, MI, MS)		_UNARYLm(X86_IDIV, MD, MB, MI, MS)

/*									_format		Opcd		,Mod ,r	    ,m		,mem=dsp+sib	,imm... */

#define IMULWrr(RS, RD)			(_d16(), _REXLrr(RD, RS),	_OO_Mrm		(0x0faf		,_b11,_r2(RD),_r2(RS)				))
#define IMULWmr(MD, MB, MI, MS, RD)	(_d16(), _REXLmr(MB, MI, RD),	_OO_r_X		(0x0faf		     ,_r2(RD)		,MD,MB,MI,MS		))

#define IMULWirr(IM,RS,RD)		(_d16(), _REXLrr(RS, RD),	_Os_Mrm_sW	(0x69		,_b11,_r2(RS),_r2(RD)			,_su16(IM)	))
#define IMULWimr(IM,MD,MB,MI,MS,RD)	(_d16(), _REXLmr(MB, MI, RD),	_Os_r_X_sW	(0x69		     ,_r2(RD)		,MD,MB,MI,MS	,_su16(IM)	))

#define IMULLir(IM, RD)			(_REXLrr(0, RD),		_Os_Mrm_sL	(0x69		,_b11,_r4(RD),_r4(RD)			,IM	))
#define IMULLrr(RS, RD)			(_REXLrr(RD, RS),		_OO_Mrm		(0x0faf		,_b11,_r4(RD),_r4(RS)				))
#define IMULLmr(MD, MB, MI, MS, RD)	(_REXLmr(MB, MI, RD),		_OO_r_X		(0x0faf		     ,_r4(RD)		,MD,MB,MI,MS		))


#define IMULLirr(IM,RS,RD)		(_REXLrr(RS, RD),		_Os_Mrm_sL	(0x69		,_b11,_r4(RS),_r4(RD)			,IM	))
#define IMULLimr(IM,MD,MB,MI,MS,RD)	(_REXLmr(MB, MI, RD),		_Os_r_X_sL	(0x69		     ,_r4(RD)		,MD,MB,MI,MS	,IM	))



/* --- Control Flow related instructions ----------------------------------- */

enum {
  X86_CC_O   = 0x0,
  X86_CC_NO  = 0x1,
  X86_CC_NAE = 0x2,
  X86_CC_B   = 0x2,
  X86_CC_C   = 0x2,
  X86_CC_AE  = 0x3,
  X86_CC_NB  = 0x3,
  X86_CC_NC  = 0x3,
  X86_CC_E   = 0x4,
  X86_CC_Z   = 0x4,
  X86_CC_NE  = 0x5,
  X86_CC_NZ  = 0x5,
  X86_CC_BE  = 0x6,
  X86_CC_NA  = 0x6,
  X86_CC_A   = 0x7,
  X86_CC_NBE = 0x7,
  X86_CC_S   = 0x8,
  X86_CC_NS  = 0x9,
  X86_CC_P   = 0xa,
  X86_CC_PE  = 0xa,
  X86_CC_NP  = 0xb,
  X86_CC_PO  = 0xb,
  X86_CC_L   = 0xc,
  X86_CC_NGE = 0xc,
  X86_CC_GE  = 0xd,
  X86_CC_NL  = 0xd,
  X86_CC_LE  = 0xe,
  X86_CC_NG  = 0xe,
  X86_CC_G   = 0xf,
  X86_CC_NLE = 0xf,
};

/*									_format		Opcd		,Mod ,r	    ,m		,mem=dsp+sib	,imm... */

/* FIXME: no prefix is availble to encode a 32-bit operand size in 64-bit
   mode */
#define CALLm(M)							_O_D32		(0xe8					,(int)(M)		)
#define CALLLsr(R)			(_REXLrr(0, R),			_O_Mrm		(0xff		,_b11,_b010,_r4(R)				))
#define CALLsm(D,B,I,S)			(_REXLrm(0, B, I),		_O_r_X		(0xff		     ,_b010		,(int)(D),B,I,S		))

/* FIXME: no prefix is availble to encode a 32-bit operand size in 64-bit
   mode */
#define JMPSm(M)							_O_D8		(0xeb					,(int)(M)		)
#define JMPm(M)								_O_D32		(0xe9					,(int)(M)		)
#define JMPLsr(R)			(_REXLrr(0, R),			_O_Mrm		(0xff		,_b11,_b100,_r4(R)				))
#define JMPsm(D,B,I,S)			(_REXLrm(0, B, I),		_O_r_X		(0xff		     ,_b100		,(int)(D),B,I,S		))

/*									_format		Opcd		,Mod ,r	    ,m		,mem=dsp+sib	,imm... */
#define JCCSii(CC, D)							_O_B		(0x70|(CC)				,(_sc)(int)(D)		)
#define JCCSim(CC, D)							_O_D8		(0x70|(CC)				,(int)(D)		)
#define JOSm(D)				JCCSim(0x0, D)
#define JNOSm(D)			JCCSim(0x1, D)
#define JBSm(D)				JCCSim(0x2, D)
#define JNAESm(D)			JCCSim(0x2, D)
#define JNBSm(D)			JCCSim(0x3, D)
#define JAESm(D)			JCCSim(0x3, D)
#define JESm(D)				JCCSim(0x4, D)
#define JZSm(D)				JCCSim(0x4, D)
#define JNESm(D)			JCCSim(0x5, D)
#define JNZSm(D)			JCCSim(0x5, D)
#define JBESm(D)			JCCSim(0x6, D)
#define JNASm(D)			JCCSim(0x6, D)
#define JNBESm(D)			JCCSim(0x7, D)
#define JASm(D)				JCCSim(0x7, D)
#define JSSm(D)				JCCSim(0x8, D)
#define JNSSm(D)			JCCSim(0x9, D)
#define JPSm(D)				JCCSim(0xa, D)
#define JPESm(D)			JCCSim(0xa, D)
#define JNPSm(D)			JCCSim(0xb, D)
#define JPOSm(D)			JCCSim(0xb, D)
#define JLSm(D)				JCCSim(0xc, D)
#define JNGESm(D)			JCCSim(0xc, D)
#define JNLSm(D)			JCCSim(0xd, D)
#define JGESm(D)			JCCSim(0xd, D)
#define JLESm(D)			JCCSim(0xe, D)
#define JNGSm(D)			JCCSim(0xe, D)
#define JNLESm(D)			JCCSim(0xf, D)
#define JGSm(D)				JCCSim(0xf, D)

/*									_format		Opcd		,Mod ,r	    ,m		,mem=dsp+sib	,imm... */
#define JCCii(CC, D)							_OO_L		(0x0f80|(CC)				,(int)(D)		)
#define JCCim(CC, D)							_OO_D32		(0x0f80|(CC)				,(int)(D)		)
#define JOm(D)				JCCim(0x0, D)
#define JNOm(D)				JCCim(0x1, D)
#define JBm(D)				JCCim(0x2, D)
#define JNAEm(D)			JCCim(0x2, D)
#define JNBm(D)				JCCim(0x3, D)
#define JAEm(D)				JCCim(0x3, D)
#define JEm(D)				JCCim(0x4, D)
#define JZm(D)				JCCim(0x4, D)
#define JNEm(D)				JCCim(0x5, D)
#define JNZm(D)				JCCim(0x5, D)
#define JBEm(D)				JCCim(0x6, D)
#define JNAm(D)				JCCim(0x6, D)
#define JNBEm(D)			JCCim(0x7, D)
#define JAm(D)				JCCim(0x7, D)
#define JSm(D)				JCCim(0x8, D)
#define JNSm(D)				JCCim(0x9, D)
#define JPm(D)				JCCim(0xa, D)
#define JPEm(D)				JCCim(0xa, D)
#define JNPm(D)				JCCim(0xb, D)
#define JPOm(D)				JCCim(0xb, D)
#define JLm(D)				JCCim(0xc, D)
#define JNGEm(D)			JCCim(0xc, D)
#define JNLm(D)				JCCim(0xd, D)
#define JGEm(D)				JCCim(0xd, D)
#define JLEm(D)				JCCim(0xe, D)
#define JNGm(D)				JCCim(0xe, D)
#define JNLEm(D)			JCCim(0xf, D)
#define JGm(D)				JCCim(0xf, D)

/*									_format		Opcd		,Mod ,r	    ,m		,mem=dsp+sib	,imm... */
#define SETCCir(CC, RD)			(_REXBrr(0, RD),		_OO_Mrm		(0x0f90|(CC)	,_b11,_b000,_r1(RD)				))
#define SETOr(RD)			SETCCir(0x0,RD)
#define SETNOr(RD)			SETCCir(0x1,RD)
#define SETBr(RD)			SETCCir(0x2,RD)
#define SETNAEr(RD)			SETCCir(0x2,RD)
#define SETNBr(RD)			SETCCir(0x3,RD)
#define SETAEr(RD)			SETCCir(0x3,RD)
#define SETEr(RD)			SETCCir(0x4,RD)
#define SETZr(RD)			SETCCir(0x4,RD)
#define SETNEr(RD)			SETCCir(0x5,RD)
#define SETNZr(RD)			SETCCir(0x5,RD)
#define SETBEr(RD)			SETCCir(0x6,RD)
#define SETNAr(RD)			SETCCir(0x6,RD)
#define SETNBEr(RD)			SETCCir(0x7,RD)
#define SETAr(RD)			SETCCir(0x7,RD)
#define SETSr(RD)			SETCCir(0x8,RD)
#define SETNSr(RD)			SETCCir(0x9,RD)
#define SETPr(RD)			SETCCir(0xa,RD)
#define SETPEr(RD)			SETCCir(0xa,RD)
#define SETNPr(RD)			SETCCir(0xb,RD)
#define SETPOr(RD)			SETCCir(0xb,RD)
#define SETLr(RD)			SETCCir(0xc,RD)
#define SETNGEr(RD)			SETCCir(0xc,RD)
#define SETNLr(RD)			SETCCir(0xd,RD)
#define SETGEr(RD)			SETCCir(0xd,RD)
#define SETLEr(RD)			SETCCir(0xe,RD)
#define SETNGr(RD)			SETCCir(0xe,RD)
#define SETNLEr(RD)			SETCCir(0xf,RD)
#define SETGr(RD)			SETCCir(0xf,RD)

/*									_format		Opcd		,Mod ,r	    ,m		,mem=dsp+sib	,imm... */
#define SETCCim(CC,MD,MB,MI,MS)		(_REXBrm(0, MB, MI),		_OO_r_X		(0x0f90|(CC)	     ,_b000		,MD,MB,MI,MS		))
#define SETOm(D, B, I, S)		SETCCim(0x0, D, B, I, S)
#define SETNOm(D, B, I, S)		SETCCim(0x1, D, B, I, S)
#define SETBm(D, B, I, S)		SETCCim(0x2, D, B, I, S)
#define SETNAEm(D, B, I, S)		SETCCim(0x2, D, B, I, S)
#define SETNBm(D, B, I, S)		SETCCim(0x3, D, B, I, S)
#define SETAEm(D, B, I, S)		SETCCim(0x3, D, B, I, S)
#define SETEm(D, B, I, S)		SETCCim(0x4, D, B, I, S)
#define SETZm(D, B, I, S)		SETCCim(0x4, D, B, I, S)
#define SETNEm(D, B, I, S)		SETCCim(0x5, D, B, I, S)
#define SETNZm(D, B, I, S)		SETCCim(0x5, D, B, I, S)
#define SETBEm(D, B, I, S)		SETCCim(0x6, D, B, I, S)
#define SETNAm(D, B, I, S)		SETCCim(0x6, D, B, I, S)
#define SETNBEm(D, B, I, S)		SETCCim(0x7, D, B, I, S)
#define SETAm(D, B, I, S)		SETCCim(0x7, D, B, I, S)
#define SETSm(D, B, I, S)		SETCCim(0x8, D, B, I, S)
#define SETNSm(D, B, I, S)		SETCCim(0x9, D, B, I, S)
#define SETPm(D, B, I, S)		SETCCim(0xa, D, B, I, S)
#define SETPEm(D, B, I, S)		SETCCim(0xa, D, B, I, S)
#define SETNPm(D, B, I, S)		SETCCim(0xb, D, B, I, S)
#define SETPOm(D, B, I, S)		SETCCim(0xb, D, B, I, S)
#define SETLm(D, B, I, S)		SETCCim(0xc, D, B, I, S)
#define SETNGEm(D, B, I, S)		SETCCim(0xc, D, B, I, S)
#define SETNLm(D, B, I, S)		SETCCim(0xd, D, B, I, S)
#define SETGEm(D, B, I, S)		SETCCim(0xd, D, B, I, S)
#define SETLEm(D, B, I, S)		SETCCim(0xe, D, B, I, S)
#define SETNGm(D, B, I, S)		SETCCim(0xe, D, B, I, S)
#define SETNLEm(D, B, I, S)		SETCCim(0xf, D, B, I, S)
#define SETGm(D, B, I, S)		SETCCim(0xf, D, B, I, S)

/*									_format		Opcd		,Mod ,r	     ,m		,mem=dsp+sib	,imm... */
#define CMOVWrr(CC,RS,RD)		(_d16(), _REXLrr(RD, RS),	_OO_Mrm		(0x0f40|(CC)	,_b11,_r2(RD),_r2(RS)				))
#define CMOVWmr(CC,MD,MB,MI,MS,RD)	(_d16(), _REXLmr(MB, MI, RD),	_OO_r_X		(0x0f40|(CC)	     ,_r2(RD)		,MD,MB,MI,MS		))
#define CMOVLrr(CC,RS,RD)		(_REXLrr(RD, RS),		_OO_Mrm		(0x0f40|(CC)	,_b11,_r4(RD),_r4(RS)				))
#define CMOVLmr(CC,MD,MB,MI,MS,RD)	(_REXLmr(MB, MI, RD),		_OO_r_X		(0x0f40|(CC)	     ,_r4(RD)		,MD,MB,MI,MS		))


/* --- Push/Pop instructions ----------------------------------------------- */

/*									_format		Opcd		,Mod ,r	    ,m		,mem=dsp+sib	,imm... */

#define POPWr(RD)			_m32only((_d16(),		_Or		(0x58,_r2(RD)							)))
#define POPWm(MD, MB, MI, MS)		_m32only((_d16(),		_O_r_X		(0x8f		     ,_b000		,MD,MB,MI,MS		)))

#define POPLr(RD)			_m32only(			_Or		(0x58,_r4(RD)							))
#define POPLm(MD, MB, MI, MS)		_m32only(			_O_r_X		(0x8f		     ,_b000		,MD,MB,MI,MS		))


#define PUSHWr(RS)			_m32only((_d16(),		_Or		(0x50,_r2(RS)							)))
#define PUSHWm(MD, MB, MI, MS)		_m32only((_d16(),		_O_r_X		(0xff,		     ,_b110		,MD,MB,MI,MS		)))
#define PUSHWi(IM)			_m32only((_d16(),		_Os_sW		(0x68							,IM	)))

#define PUSHLr(RS)			_m32only(			_Or		(0x50,_r4(RS)							))
#define PUSHLm(MD, MB, MI, MS)		_m32only(			_O_r_X		(0xff		     ,_b110		,MD,MB,MI,MS		))
#define PUSHLi(IM)			_m32only(			_Os_sL		(0x68							,IM	))


#define POPA_()				(_d16(),			_O		(0x61								))
#define POPAD_()							_O		(0x61								)

#define PUSHA_()			(_d16(),			_O		(0x60								))
#define PUSHAD_()							_O		(0x60								)

#define POPF_()								_O		(0x9d								)
#define PUSHF_()							_O		(0x9c								)


/* --- Test instructions --------------------------------------------------- */

/*									_format		Opcd		,Mod ,r	    ,m		,mem=dsp+sib	,imm... */

#define TESTBrr(RS, RD)			(_REXBrr(RS, RD),		_O_Mrm		(0x84		,_b11,_r1(RS),_r1(RD)				))
#define TESTBrm(RS, MD, MB, MI, MS)	(_REXBrm(RS, MB, MI),		_O_r_X		(0x84		     ,_r1(RS)		,MD,MB,MI,MS		))
#define TESTBir(IM, RD)			((RD) == _AL ? \
					(_REXBrr(0, RD),		_O_B		(0xa8							,_u8(IM))) : \
					(_REXBrr(0, RD),		_O_Mrm_B	(0xf6		,_b11,_b000  ,_r1(RD)			,_u8(IM))) )
#define TESTBim(IM, MD, MB, MI, MS)	(_REXBrm(0, MB, MI),		_O_r_X_B	(0xf6		     ,_b000		,MD,MB,MI,MS	,_u8(IM)))

#define TESTWrr(RS, RD)			(_d16(), _REXLrr(RS, RD),	_O_Mrm		(0x85		,_b11,_r2(RS),_r2(RD)				))
#define TESTWrm(RS, MD, MB, MI, MS)	(_d16(), _REXLrm(RS, MB, MI),	_O_r_X		(0x85		     ,_r2(RS)		,MD,MB,MI,MS		))
#define TESTWir(IM, RD)			((RD) == _AX ? \
					(_d16(), _REXLrr(0, RD),	_O_W		(0xa9							,_u16(IM))) : \
					(_d16(), _REXLrr(0, RD),	_O_Mrm_W	(0xf7		,_b11,_b000  ,_r2(RD)			,_u16(IM))) )
#define TESTWim(IM, MD, MB, MI, MS)	(_d16(), _REXLrm(0, MB, MI),	_O_r_X_W	(0xf7		     ,_b000		,MD,MB,MI,MS	,_u16(IM)))

#define TESTLrr(RS, RD)			(_REXLrr(RS, RD),		_O_Mrm		(0x85		,_b11,_r4(RS),_r4(RD)				))
#define TESTLrm(RS, MD, MB, MI, MS)	(_REXLrm(RS, MB, MI),		_O_r_X		(0x85		     ,_r4(RS)		,MD,MB,MI,MS		))
#define TESTLir(IM, RD)			(!_s8P(IM) && (RD) == _EAX ? \
					(_REXLrr(0, RD),		_O_L		(0xa9							,IM	)) : \
					(_REXLrr(0, RD),		_O_Mrm_L	(0xf7		,_b11,_b000  ,_r4(RD)			,IM	)) )
#define TESTLim(IM, MD, MB, MI, MS)	(_REXLrm(0, MB, MI),		_O_r_X_L	(0xf7		     ,_b000		,MD,MB,MI,MS	,IM	))



/* --- Exchange instructions ----------------------------------------------- */

/*									_format		Opcd		,Mod ,r	    ,m		,mem=dsp+sib	,imm... */

#define CMPXCHGBrr(RS, RD)		(_REXBrr(RS, RD),		_OO_Mrm		(0x0fb0		,_b11,_r1(RS),_r1(RD)				))
#define CMPXCHGBrm(RS, MD, MB, MI, MS)	(_REXBrm(RS, MB, MI),		_OO_r_X		(0x0fb0		     ,_r1(RS)		,MD,MB,MI,MS		))

#define CMPXCHGWrr(RS, RD)		(_d16(), _REXLrr(RS, RD),	_OO_Mrm		(0x0fb1		,_b11,_r2(RS),_r2(RD)				))
#define CMPXCHGWrm(RS, MD, MB, MI, MS)	(_d16(), _REXLrm(RS, MB, MI),	_OO_r_X		(0x0fb1		     ,_r2(RS)		,MD,MB,MI,MS		))

#define CMPXCHGLrr(RS, RD)		(_REXLrr(RS, RD),		_OO_Mrm		(0x0fb1		,_b11,_r4(RS),_r4(RD)				))
#define CMPXCHGLrm(RS, MD, MB, MI, MS)	(_REXLrm(RS, MB, MI),		_OO_r_X		(0x0fb1		     ,_r4(RS)		,MD,MB,MI,MS		))


#define XADDBrr(RS, RD)			(_REXBrr(RS, RD),		_OO_Mrm		(0x0fc0		,_b11,_r1(RS),_r1(RD)				))
#define XADDBrm(RS, MD, MB, MI, MS)	(_REXBrm(RS, MB, MI),		_OO_r_X		(0x0fc0		     ,_r1(RS)		,MD,MB,MI,MS		))

#define XADDWrr(RS, RD)			(_d16(), _REXLrr(RS, RD),	_OO_Mrm		(0x0fc1		,_b11,_r2(RS),_r2(RD)				))
#define XADDWrm(RS, MD, MB, MI, MS)	(_d16(), _REXLrm(RS, MB, MI),	_OO_r_X		(0x0fc1		     ,_r2(RS)		,MD,MB,MI,MS		))

#define XADDLrr(RS, RD)			(_REXLrr(RS, RD),		_OO_Mrm		(0x0fc1		,_b11,_r4(RS),_r4(RD)				))
#define XADDLrm(RS, MD, MB, MI, MS)	(_REXLrm(RS, MB, MI),		_OO_r_X		(0x0fc1		     ,_r4(RS)		,MD,MB,MI,MS		))


#define XCHGBrr(RS, RD)			(_REXBrr(RS, RD),		_O_Mrm		(0x86		,_b11,_r1(RS),_r1(RD)				))
#define XCHGBrm(RS, MD, MB, MI, MS)	(_REXBrm(RS, MB, MI),		_O_r_X		(0x86		     ,_r1(RS)		,MD,MB,MI,MS		))

#define XCHGWrr(RS, RD)			(_d16(), _REXLrr(RS, RD),	_O_Mrm		(0x87		,_b11,_r2(RS),_r2(RD)				))
#define XCHGWrm(RS, MD, MB, MI, MS)	(_d16(), _REXLrm(RS, MB, MI),	_O_r_X		(0x87		     ,_r2(RS)		,MD,MB,MI,MS		))

#define XCHGLrr(RS, RD)			(_REXLrr(RS, RD),		_O_Mrm		(0x87		,_b11,_r4(RS),_r4(RD)				))
#define XCHGLrm(RS, MD, MB, MI, MS)	(_REXLrm(RS, MB, MI),		_O_r_X		(0x87		     ,_r4(RS)		,MD,MB,MI,MS		))



/* --- Increment/Decrement instructions ------------------------------------ */

/*									_format		Opcd		,Mod ,r	    ,m		,mem=dsp+sib	,imm... */

#define DECBm(MD, MB, MI, MS)		(_REXBrm(0, MB, MI),		_O_r_X		(0xfe		     ,_b001		,MD,MB,MI,MS		))
#define DECBr(RD)			(_REXBrr(0, RD),		_O_Mrm		(0xfe		,_b11,_b001  ,_r1(RD)				))

#define DECWm(MD, MB, MI, MS)		(_d16(), _REXLrm(0, MB, MI),	_O_r_X		(0xff		     ,_b001		,MD,MB,MI,MS		))

#define DECLm(MD, MB, MI, MS)		(_REXLrm(0, MB, MI),		_O_r_X		(0xff		     ,_b001		,MD,MB,MI,MS		))


#define INCBm(MD, MB, MI, MS)		(_REXBrm(0, MB, MI),		_O_r_X		(0xfe		     ,_b000		,MD,MB,MI,MS		))
#define INCBr(RD)			(_REXBrr(0, RD),		_O_Mrm		(0xfe		,_b11,_b000  ,_r1(RD)				))

#define INCWm(MD, MB, MI, MS)		(_d16(), _REXLrm(0, MB, MI),	_O_r_X		(0xff		     ,_b000		,MD,MB,MI,MS		))

#define INCLm(MD, MB, MI, MS)		(_REXLrm(0, MB, MI),		_O_r_X		(0xff		     ,_b000		,MD,MB,MI,MS		))



/* --- Misc instructions --------------------------------------------------- */

/*									_format		Opcd		,Mod ,r	    ,m		,mem=dsp+sib	,imm... */

#define BSFWrr(RS, RD)			(_d16(), _REXLrr(RD, RS),	_OO_Mrm		(0x0fbc		,_b11,_r2(RD),_r2(RS)				))
#define BSFWmr(MD, MB, MI, MS, RD)	(_d16(), _REXLmr(MB, MI, RD),	_OO_r_X		(0x0fbc		     ,_r2(RD)		,MD,MB,MI,MS		))
#define BSRWrr(RS, RD)			(_d16(), _REXLrr(RD, RS),	_OO_Mrm		(0x0fbd		,_b11,_r2(RD),_r2(RS)				))
#define BSRWmr(MD, MB, MI, MS, RD)	(_d16(), _REXLmr(MB, MI, RD),	_OO_r_X		(0x0fbd		     ,_r2(RD)		,MD,MB,MI,MS		))

#define BSFLrr(RS, RD)			(_REXLrr(RD, RS),		_OO_Mrm		(0x0fbc		,_b11,_r4(RD),_r4(RS)				))
#define BSFLmr(MD, MB, MI, MS, RD)	(_REXLmr(MB, MI, RD),		_OO_r_X		(0x0fbc		     ,_r4(RD)		,MD,MB,MI,MS		))
#define BSRLrr(RS, RD)			(_REXLrr(RD, RS),		_OO_Mrm		(0x0fbd		,_b11,_r4(RD),_r4(RS)				))
#define BSRLmr(MD, MB, MI, MS, RD)	(_REXLmr(MB, MI, RD),		_OO_r_X		(0x0fbd		     ,_r4(RD)		,MD,MB,MI,MS		))


/*									_format		Opcd		,Mod ,r	    ,m		,mem=dsp+sib	,imm... */

#define MOVSBWrr(RS, RD)		(_d16(), _REXBLrr(RD, RS),	_OO_Mrm		(0x0fbe		,_b11,_r2(RD),_r1(RS)				))
#define MOVSBWmr(MD, MB, MI, MS, RD)	(_d16(), _REXLmr(MB, MI, RD),	_OO_r_X		(0x0fbe		     ,_r2(RD)		,MD,MB,MI,MS		))
#define MOVZBWrr(RS, RD)		(_d16(), _REXBLrr(RD, RS),	_OO_Mrm		(0x0fb6		,_b11,_r2(RD),_r1(RS)				))
#define MOVZBWmr(MD, MB, MI, MS, RD)	(_d16(), _REXLmr(MB, MI, RD),	_OO_r_X		(0x0fb6		     ,_r2(RD)		,MD,MB,MI,MS		))

#define MOVSBLrr(RS, RD)		(_REXBLrr(RD, RS),		_OO_Mrm		(0x0fbe		,_b11,_r4(RD),_r1(RS)				))
#define MOVSBLmr(MD, MB, MI, MS, RD)	(_REXLmr(MB, MI, RD),		_OO_r_X		(0x0fbe		     ,_r4(RD)		,MD,MB,MI,MS		))
#define MOVZBLrr(RS, RD)		(_REXBLrr(RD, RS),		_OO_Mrm		(0x0fb6		,_b11,_r4(RD),_r1(RS)				))
#define MOVZBLmr(MD, MB, MI, MS, RD)	(_REXLmr(MB, MI, RD),		_OO_r_X		(0x0fb6		     ,_r4(RD)		,MD,MB,MI,MS		))


#define MOVSWLrr(RS, RD)		(_REXLrr(RD, RS),		_OO_Mrm		(0x0fbf		,_b11,_r4(RD),_r2(RS)				))
#define MOVSWLmr(MD, MB, MI, MS, RD)	(_REXLmr(MB, MI, RD),		_OO_r_X		(0x0fbf		     ,_r4(RD)		,MD,MB,MI,MS		))
#define MOVZWLrr(RS, RD)		(_REXLrr(RD, RS),		_OO_Mrm		(0x0fb7		,_b11,_r4(RD),_r2(RS)				))
#define MOVZWLmr(MD, MB, MI, MS, RD)	(_REXLmr(MB, MI, RD),		_OO_r_X		(0x0fb7		     ,_r4(RD)		,MD,MB,MI,MS		))



/*									_format		Opcd		,Mod ,r	    ,m		,mem=dsp+sib	,imm... */

#define LEALmr(MD, MB, MI, MS, RD)	(_REXLmr(MB, MI, RD),		_O_r_X		(0x8d		     ,_r4(RD)		,MD,MB,MI,MS		))

#define BSWAPLr(R)			(_REXLrr(0, R),			_OOr		(0x0fc8,_r4(R)							))

#define CLC_()								_O		(0xf8								)
#define STC_()								_O		(0xf9								)

#define CMC_()								_O		(0xf5								)
#define CLD_()								_O		(0xfc								)
#define STD_()								_O		(0xfd								)

#define CBTW_()				(_d16(),			_O		(0x98								))
#define CWTL_()								_O		(0x98								)
#define CLTQ_()				_m64only(_REXQrr(0, 0),		_O		(0x98								))

#define CBW_()				CBTW_()
#define CWDE_()				CWTL_()
#define CDQE_()				CLTQ_()

#define CWTD_()				(_d16(),			_O		(0x99								))
#define CLTD_()								_O		(0x99								)
#define CQTO_()				_m64only(_REXQrr(0, 0),		_O		(0x99								))

#define CWD_()				CWTD_()
#define CDQ_()				CLTD_()
#define CQO_()				CQTO_()

#define LAHF_()				_m32only(			_O		(0x9f								))
#define SAHF_()				_m32only(			_O		(0x9e								))

/*									_format		Opcd		,Mod ,r	    ,m		,mem=dsp+sib	,imm... */

#define CPUID_()							_OO		(0x0fa2								)
#define RDTSC_()							_OO		(0xff31								)

#define ENTERii(W, B)							_O_W_B		(0xc8						  ,_su16(W),_su8(B))

#define LEAVE_()							_O		(0xc9								)
#define RET_()								_O		(0xc3								)
#define RETi(IM)							_O_W		(0xc2							,_su16(IM))

#define NOP_()								_O		(0x90								)

/* x87 instructions -- yay, we found a use for octal constants :-) */

#define ESCmi(D,B,I,S,OP)	(_REXLrm(0,B,I), _O_r_X(0xd8|(OP >> 3), (OP & 7), D,B,I,S))
#define ESCri(RD,OP)		_O_Mrm(0xd8|(OP >> 3), _b11, (OP & 7), RD)

#define ESCrri(RS,RD,OP)	((RS) == _ST0 ? ESCri(RD,(OP|040))			\
				 : (RD) == _ST0 ? ESCri(RS,OP)				\
				 : JITFAIL ("coprocessor instruction without st0"))

#define FLDSm(D,B,I,S)		ESCmi(D,B,I,S,010)     /* fld m32real  */
#define FILDLm(D,B,I,S)		ESCmi(D,B,I,S,030)     /* fild m32int  */
#define FLDLm(D,B,I,S)		ESCmi(D,B,I,S,050)     /* fld m64real  */
#define FILDWm(D,B,I,S)		ESCmi(D,B,I,S,070)     /* fild m16int  */
#define FSTSm(D,B,I,S)		ESCmi(D,B,I,S,012)     /* fst m32real  */
#define FISTLm(D,B,I,S)		ESCmi(D,B,I,S,032)     /* fist m32int  */
#define FSTLm(D,B,I,S)		ESCmi(D,B,I,S,052)     /* fst m64real  */
#define FISTWm(D,B,I,S)		ESCmi(D,B,I,S,072)     /* fist m16int  */
#define FSTPSm(D,B,I,S)		ESCmi(D,B,I,S,013)     /* fstp m32real */
#define FISTPLm(D,B,I,S)	ESCmi(D,B,I,S,033)     /* fistp m32int */
#define FSTPLm(D,B,I,S)		ESCmi(D,B,I,S,053)     /* fstp m64real */
#define FISTPWm(D,B,I,S)	ESCmi(D,B,I,S,073)     /* fistp m16int */
#define FLDTm(D,B,I,S)		ESCmi(D,B,I,S,035)     /* fld m80real  */
#define FILDQm(D,B,I,S)		ESCmi(D,B,I,S,075)     /* fild m64int  */
#define FSTPTm(D,B,I,S)		ESCmi(D,B,I,S,037)     /* fstp m80real */
#define FISTPQm(D,B,I,S)	ESCmi(D,B,I,S,077)     /* fistp m64int */

#define FADDrr(RS,RD)		ESCrri(RS,RD,000)
#define FMULrr(RS,RD)		ESCrri(RS,RD,001)
#define FSUBrr(RS,RD)		ESCrri(RS,RD,004)
#define FSUBRrr(RS,RD)		ESCrri(RS,RD,005)
#define FDIVrr(RS,RD)		ESCrri(RS,RD,006)
#define FDIVRrr(RS,RD)		ESCrri(RS,RD,007)

#define FLDr(RD)		ESCri(RD,010)
#define FXCHr(RD)		ESCri(RD,011)
#define FFREEr(RD)		ESCri(RD,050)
#define FSTr(RD)		ESCri(RD,052)
#define FSTPr(RD)		ESCri(RD,053)
#define FCOMr(RD)		ESCri(RD,002)
#define FCOMPr(RD)		ESCri(RD,003)
#define FCOMIr(RD)		ESCri(RD,036)
#define FCOMIPr(RD)		ESCri(RD,076)
#define FUCOMr(RD)		ESCri(RD,054)
#define FUCOMPr(RD)		ESCri(RD,055)
#define FUCOMIr(RD)		ESCri(RD,035)
#define FUCOMIPr(RD)		ESCri(RD,075)
#define FADDPr(RD)		ESCri(RD,060)
#define FMULPr(RD)		ESCri(RD,061)
#define FSUBPr(RD)		ESCri(RD,064)
#define FSUBRPr(RD)		ESCri(RD,065)
#define FDIVPr(RD)		ESCri(RD,066)
#define FDIVRPr(RD)		ESCri(RD,067)

#define FNSTSWr(RD)		((RD == _AX || RD == _EAX) ? _OO (0xdfe0)		\
				 : JITFAIL ("AX or EAX expected"))
/* N byte NOPs */
#define NOPi(N)		(((  (N)    >= 8) ? (_jit_B(0x8d),_jit_B(0xb4),_jit_B(0x26),_jit_I(0x00),_jit_B(0x90)) : (void) 0), \
			 (( ((N)&7) == 7) ? (_jit_B(0x8d),_jit_B(0xb4),_jit_B(0x26),_jit_I(0x00)) : \
			  ( ((N)&7) == 6) ? (_jit_B(0x8d),_jit_B(0xb6),_jit_I(0x00)) : \
			  ( ((N)&7) == 5) ? (_jit_B(0x90),_jit_B(0x8d),_jit_B(0x74),_jit_B(0x26),_jit_B(0x00)) : \
/* leal 0(,%esi), %esi */ ( ((N)&7) == 4) ? (_jit_B(0x8d),_jit_B(0x74),_jit_B(0x26),_jit_B(0x00)) : \
/* leal (,%esi), %esi */  ( ((N)&7) == 3) ? (_jit_B(0x8d),_jit_B(0x76),_jit_B(0x00)) : \
/* movl %esi, %esi */	  ( ((N)&7) == 2) ? (_jit_B(0x89),_jit_B(0xf6)) : \
			  ( ((N)&7) == 1) ? (_jit_B(0x90)) : \
			  ( ((N)&7) == 0) ? 0 : \
			  JITFAIL(".align argument too large")))


/* --- Media 128-bit instructions ------------------------------------------ */

enum {
  X86_SSE_MOV    = 0x10,
  X86_SSE_MOVLP  = 0x12,
  X86_SSE_MOVHP  = 0x16,
  X86_SSE_MOVA   = 0x28,
  X86_SSE_CVTIS  = 0x2a,
  X86_SSE_CVTTSI = 0x2c,
  X86_SSE_CVTSI  = 0x2d,
  X86_SSE_UCOMI  = 0x2e,
  X86_SSE_COMI   = 0x2f,
  X86_SSE_SQRT   = 0x51,
  X86_SSE_RSQRT  = 0x52,
  X86_SSE_RCP    = 0x53,
  X86_SSE_AND    = 0x54,
  X86_SSE_ANDN   = 0x55,
  X86_SSE_OR     = 0x56,
  X86_SSE_XOR    = 0x57,
  X86_SSE_ADD    = 0x58,
  X86_SSE_MUL    = 0x59,
  X86_SSE_CVTSD  = 0x5a,
  X86_SSE_CVTDT  = 0x5b,
  X86_SSE_SUB    = 0x5c,
  X86_SSE_MIN    = 0x5d,
  X86_SSE_DIV    = 0x5e,
  X86_SSE_MAX    = 0x5f,
  X86_SSE_MOV2   = 0xd6
};

/*										_format		Opcd		,Mod ,r	     ,m		,mem=dsp+sib	,imm... */

#define __SSELrr(OP,RS,RSA,RD,RDA)		(_REXLrr(RD, RS),		_OO_Mrm		(0x0f00|(OP)	,_b11,RDA(RD),RSA(RS)				))
#define __SSELmr(OP,MD,MB,MI,MS,RD,RDA)		(_REXLmr(MB, MI, RD),		_OO_r_X		(0x0f00|(OP)	     ,RDA(RD)		,MD,MB,MI,MS		))
#define __SSELrm(OP,RS,RSA,MD,MB,MI,MS)		(_REXLrm(RS, MB, MI),		_OO_r_X		(0x0f00|(OP)	     ,RSA(RS)		,MD,MB,MI,MS		))
#define __SSEL1rm(OP,RS,RSA,MD,MB,MI,MS)	(_REXLrm(RS, MB, MI),		_OO_r_X		(0x0f01|(OP)	     ,RSA(RS)		,MD,MB,MI,MS		))

#define _SSELrr(PX,OP,RS,RSA,RD,RDA)					(_jit_B(PX), __SSELrr(OP, RS, RSA, RD, RDA))
#define _SSELmr(PX,OP,MD,MB,MI,MS,RD,RDA)				(_jit_B(PX), __SSELmr(OP, MD, MB, MI, MS, RD, RDA))
#define _SSELrm(PX,OP,RS,RSA,MD,MB,MI,MS)				(_jit_B(PX), __SSELrm(OP, RS, RSA, MD, MB, MI, MS))
#define _SSEL1rm(PX,OP,RS,RSA,MD,MB,MI,MS)				(_jit_B(PX), __SSEL1rm(OP, RS, RSA, MD, MB, MI, MS))

#define _SSEPSrr(OP,RS,RD)		__SSELrr (      OP, RS,_rX, RD,_rX)
#define _SSEPSmr(OP,MD,MB,MI,MS,RD)	__SSELmr (      OP, MD, MB, MI, MS, RD,_rX)
#define _SSEPSrm(OP,RS,MD,MB,MI,MS)	__SSELrm (      OP, RS,_rX, MD, MB, MI, MS)
#define _SSEPS1rm(OP,RS,MD,MB,MI,MS)	__SSEL1rm(      OP, RS,_rX, MD, MB, MI, MS)

#define _SSEPDrr(OP,RS,RD)		 _SSELrr (0x66, OP, RS,_rX, RD,_rX)
#define _SSEPDmr(OP,MD,MB,MI,MS,RD)	 _SSELmr (0x66, OP, MD, MB, MI, MS, RD,_rX)
#define _SSEPDrm(OP,RS,MD,MB,MI,MS)	 _SSELrm (0x66, OP, RS,_rX, MD, MB, MI, MS)
#define _SSEPD1rm(OP,RS,MD,MB,MI,MS)	 _SSEL1rm(0x66, OP, RS,_rX, MD, MB, MI, MS)

#define _SSESSrr(OP,RS,RD)		 _SSELrr (0xf3, OP, RS,_rX, RD,_rX)
#define _SSESSmr(OP,MD,MB,MI,MS,RD)	 _SSELmr (0xf3, OP, MD, MB, MI, MS, RD,_rX)
#define _SSESSrm(OP,RS,MD,MB,MI,MS)	 _SSELrm (0xf3, OP, RS,_rX, MD, MB, MI, MS)
#define _SSESS1rm(OP,RS,MD,MB,MI,MS)	 _SSEL1rm(0xf3, OP, RS,_rX, MD, MB, MI, MS)

#define _SSESDrr(OP,RS,RD)		 _SSELrr (0xf2, OP, RS,_rX, RD,_rX)
#define _SSESDmr(OP,MD,MB,MI,MS,RD)	 _SSELmr (0xf2, OP, MD, MB, MI, MS, RD,_rX)
#define _SSESDrm(OP,RS,MD,MB,MI,MS)	 _SSELrm (0xf2, OP, RS,_rX, MD, MB, MI, MS)
#define _SSESD1rm(OP,RS,MD,MB,MI,MS)	 _SSEL1rm(0xf2, OP, RS,_rX, MD, MB, MI, MS)

#define ADDPSrr(RS, RD)			_SSEPSrr(X86_SSE_ADD, RS, RD)
#define ADDPSmr(MD, MB, MI, MS, RD)	_SSEPSmr(X86_SSE_ADD, MD, MB, MI, MS, RD)
#define ADDPDrr(RS, RD)			_SSEPDrr(X86_SSE_ADD, RS, RD)
#define ADDPDmr(MD, MB, MI, MS, RD)	_SSEPDmr(X86_SSE_ADD, MD, MB, MI, MS, RD)

#define ADDSSrr(RS, RD)			_SSESSrr(X86_SSE_ADD, RS, RD)
#define ADDSSmr(MD, MB, MI, MS, RD)	_SSESSmr(X86_SSE_ADD, MD, MB, MI, MS, RD)
#define ADDSDrr(RS, RD)			_SSESDrr(X86_SSE_ADD, RS, RD)
#define ADDSDmr(MD, MB, MI, MS, RD)	_SSESDmr(X86_SSE_ADD, MD, MB, MI, MS, RD)

#define ANDNPSrr(RS, RD)		_SSEPSrr(X86_SSE_ANDN, RS, RD)
#define ANDNPSmr(MD, MB, MI, MS, RD)	_SSEPSmr(X86_SSE_ANDN, MD, MB, MI, MS, RD)
#define ANDNPDrr(RS, RD)		_SSEPDrr(X86_SSE_ANDN, RS, RD)
#define ANDNPDmr(MD, MB, MI, MS, RD)	_SSEPDmr(X86_SSE_ANDN, MD, MB, MI, MS, RD)

#define ANDNSSrr			ANDNPSrr
#define ANDNSSmr			ANDNPSrr
#define ANDNSDrr			ANDNPDrr
#define ANDNSDmr			ANDNPDrr

#define ANDPSrr(RS, RD)			_SSEPSrr(X86_SSE_AND, RS, RD)
#define ANDPSmr(MD, MB, MI, MS, RD)	_SSEPSmr(X86_SSE_AND, MD, MB, MI, MS, RD)
#define ANDPDrr(RS, RD)			_SSEPDrr(X86_SSE_AND, RS, RD)
#define ANDPDmr(MD, MB, MI, MS, RD)	_SSEPDmr(X86_SSE_AND, MD, MB, MI, MS, RD)

#define ANDSSrr				ANDPSrr
#define ANDSSmr				ANDPSrr
#define ANDSDrr				ANDPDrr
#define ANDSDmr				ANDPDrr

#define DIVPSrr(RS, RD)			_SSEPSrr(X86_SSE_DIV, RS, RD)
#define DIVPSmr(MD, MB, MI, MS, RD)	_SSEPSmr(X86_SSE_DIV, MD, MB, MI, MS, RD)
#define DIVPDrr(RS, RD)			_SSEPDrr(X86_SSE_DIV, RS, RD)
#define DIVPDmr(MD, MB, MI, MS, RD)	_SSEPDmr(X86_SSE_DIV, MD, MB, MI, MS, RD)

#define DIVSSrr(RS, RD)			_SSESSrr(X86_SSE_DIV, RS, RD)
#define DIVSSmr(MD, MB, MI, MS, RD)	_SSESSmr(X86_SSE_DIV, MD, MB, MI, MS, RD)
#define DIVSDrr(RS, RD)			_SSESDrr(X86_SSE_DIV, RS, RD)
#define DIVSDmr(MD, MB, MI, MS, RD)	_SSESDmr(X86_SSE_DIV, MD, MB, MI, MS, RD)

#define MAXPSrr(RS, RD)			_SSEPSrr(X86_SSE_MAX, RS, RD)
#define MAXPSmr(MD, MB, MI, MS, RD)	_SSEPSmr(X86_SSE_MAX, MD, MB, MI, MS, RD)
#define MAXPDrr(RS, RD)			_SSEPDrr(X86_SSE_MAX, RS, RD)
#define MAXPDmr(MD, MB, MI, MS, RD)	_SSEPDmr(X86_SSE_MAX, MD, MB, MI, MS, RD)

#define MAXSSrr(RS, RD)			_SSESSrr(X86_SSE_MAX, RS, RD)
#define MAXSSmr(MD, MB, MI, MS, RD)	_SSESSmr(X86_SSE_MAX, MD, MB, MI, MS, RD)
#define MAXSDrr(RS, RD)			_SSESDrr(X86_SSE_MAX, RS, RD)
#define MAXSDmr(MD, MB, MI, MS, RD)	_SSESDmr(X86_SSE_MAX, MD, MB, MI, MS, RD)

#define MINPSrr(RS, RD)			_SSEPSrr(X86_SSE_MIN, RS, RD)
#define MINPSmr(MD, MB, MI, MS, RD)	_SSEPSmr(X86_SSE_MIN, MD, MB, MI, MS, RD)
#define MINPDrr(RS, RD)			_SSEPDrr(X86_SSE_MIN, RS, RD)
#define MINPDmr(MD, MB, MI, MS, RD)	_SSEPDmr(X86_SSE_MIN, MD, MB, MI, MS, RD)

#define MINSSrr(RS, RD)			_SSESSrr(X86_SSE_MIN, RS, RD)
#define MINSSmr(MD, MB, MI, MS, RD)	_SSESSmr(X86_SSE_MIN, MD, MB, MI, MS, RD)
#define MINSDrr(RS, RD)			_SSESDrr(X86_SSE_MIN, RS, RD)
#define MINSDmr(MD, MB, MI, MS, RD)	_SSESDmr(X86_SSE_MIN, MD, MB, MI, MS, RD)

#define MULPSrr(RS, RD)			_SSEPSrr(X86_SSE_MUL, RS, RD)
#define MULPSmr(MD, MB, MI, MS, RD)	_SSEPSmr(X86_SSE_MUL, MD, MB, MI, MS, RD)
#define MULPDrr(RS, RD)			_SSEPDrr(X86_SSE_MUL, RS, RD)
#define MULPDmr(MD, MB, MI, MS, RD)	_SSEPDmr(X86_SSE_MUL, MD, MB, MI, MS, RD)

#define MULSSrr(RS, RD)			_SSESSrr(X86_SSE_MUL, RS, RD)
#define MULSSmr(MD, MB, MI, MS, RD)	_SSESSmr(X86_SSE_MUL, MD, MB, MI, MS, RD)
#define MULSDrr(RS, RD)			_SSESDrr(X86_SSE_MUL, RS, RD)
#define MULSDmr(MD, MB, MI, MS, RD)	_SSESDmr(X86_SSE_MUL, MD, MB, MI, MS, RD)

#define ORPSrr(RS, RD)			_SSEPSrr(X86_SSE_OR, RS, RD)
#define ORPSmr(MD, MB, MI, MS, RD)	_SSEPSmr(X86_SSE_OR, MD, MB, MI, MS, RD)
#define ORPDrr(RS, RD)			_SSEPDrr(X86_SSE_OR, RS, RD)
#define ORPDmr(MD, MB, MI, MS, RD)	_SSEPDmr(X86_SSE_OR, MD, MB, MI, MS, RD)

#define ORSSrr				ORPSrr
#define ORSSmr				ORPSrr
#define ORSDrr				ORPDrr
#define ORSDmr				ORPDrr

#define RCPPSrr(RS, RD)			_SSEPSrr(X86_SSE_RCP, RS, RD)
#define RCPPSmr(MD, MB, MI, MS, RD)	_SSEPSmr(X86_SSE_RCP, MD, MB, MI, MS, RD)
#define RCPSSrr(RS, RD)			_SSESSrr(X86_SSE_RCP, RS, RD)
#define RCPSSmr(MD, MB, MI, MS, RD)	_SSESSmr(X86_SSE_RCP, MD, MB, MI, MS, RD)

#define RSQRTPSrr(RS, RD)		_SSEPSrr(X86_SSE_RSQRT, RS, RD)
#define RSQRTPSmr(MD, MB, MI, MS, RD)	_SSEPSmr(X86_SSE_RSQRT, MD, MB, MI, MS, RD)
#define RSQRTSSrr(RS, RD)		_SSESSrr(X86_SSE_RSQRT, RS, RD)
#define RSQRTSSmr(MD, MB, MI, MS, RD)	_SSESSmr(X86_SSE_RSQRT, MD, MB, MI, MS, RD)

#define SQRTPSrr(RS, RD)		_SSEPSrr(X86_SSE_SQRT, RS, RD)
#define SQRTPSmr(MD, MB, MI, MS, RD)	_SSEPSmr(X86_SSE_SQRT, MD, MB, MI, MS, RD)
#define SQRTPDrr(RS, RD)		_SSEPDrr(X86_SSE_SQRT, RS, RD)
#define SQRTPDmr(MD, MB, MI, MS, RD)	_SSEPDmr(X86_SSE_SQRT, MD, MB, MI, MS, RD)

#define SQRTSSrr(RS, RD)		_SSESSrr(X86_SSE_SQRT, RS, RD)
#define SQRTSSmr(MD, MB, MI, MS, RD)	_SSESSmr(X86_SSE_SQRT, MD, MB, MI, MS, RD)
#define SQRTSDrr(RS, RD)		_SSESDrr(X86_SSE_SQRT, RS, RD)
#define SQRTSDmr(MD, MB, MI, MS, RD)	_SSESDmr(X86_SSE_SQRT, MD, MB, MI, MS, RD)

#define SUBPSrr(RS, RD)			_SSEPSrr(X86_SSE_SUB, RS, RD)
#define SUBPSmr(MD, MB, MI, MS, RD)	_SSEPSmr(X86_SSE_SUB, MD, MB, MI, MS, RD)
#define SUBPDrr(RS, RD)			_SSEPDrr(X86_SSE_SUB, RS, RD)
#define SUBPDmr(MD, MB, MI, MS, RD)	_SSEPDmr(X86_SSE_SUB, MD, MB, MI, MS, RD)

#define SUBSSrr(RS, RD)			_SSESSrr(X86_SSE_SUB, RS, RD)
#define SUBSSmr(MD, MB, MI, MS, RD)	_SSESSmr(X86_SSE_SUB, MD, MB, MI, MS, RD)
#define SUBSDrr(RS, RD)			_SSESDrr(X86_SSE_SUB, RS, RD)
#define SUBSDmr(MD, MB, MI, MS, RD)	_SSESDmr(X86_SSE_SUB, MD, MB, MI, MS, RD)

#define XORPSrr(RS, RD)			_SSEPSrr(X86_SSE_XOR, RS, RD)
#define XORPSmr(MD, MB, MI, MS, RD)	_SSEPSmr(X86_SSE_XOR, MD, MB, MI, MS, RD)
#define XORPDrr(RS, RD)			_SSEPDrr(X86_SSE_XOR, RS, RD)
#define XORPDmr(MD, MB, MI, MS, RD)	_SSEPDmr(X86_SSE_XOR, MD, MB, MI, MS, RD)

#define XORSSrr				XORPSrr
#define XORSSmr				XORPSrr
#define XORSDrr				XORPDrr
#define XORSDmr				XORPDrr

/* No prefixes here.  */
#define COMISSrr(RS, RD)		_SSEPSrr(X86_SSE_COMI, RS, RD)
#define COMISSmr(MD, MB, MI, MS, RD)	_SSEPSmr(X86_SSE_COMI, MD, MB, MI, MS, RD)
#define COMISDrr(RS, RD)		_SSEPDrr(X86_SSE_COMI, RS, RD)
#define COMISDmr(MD, MB, MI, MS, RD)	_SSEPDmr(X86_SSE_COMI, MD, MB, MI, MS, RD)

/* No prefixes here.  */
#define UCOMISSrr(RS, RD)		_SSEPSrr(X86_SSE_UCOMI, RS, RD)
#define UCOMISSmr(MD, MB, MI, MS, RD)	_SSEPSmr(X86_SSE_UCOMI, MD, MB, MI, MS, RD)
#define UCOMISDrr(RS, RD)		_SSEPDrr(X86_SSE_UCOMI, RS, RD)
#define UCOMISDmr(MD, MB, MI, MS, RD)	_SSEPDmr(X86_SSE_UCOMI, MD, MB, MI, MS, RD)

#define MOVSSrr(RS, RD)			_SSESSrr (X86_SSE_MOV, RS, RD)
#define MOVSSmr(MD, MB, MI, MS, RD)	_SSESSmr (X86_SSE_MOV, MD, MB, MI, MS, RD)
#define MOVSSrm(RS, MD, MB, MI, MS)	_SSESS1rm(X86_SSE_MOV, RS, MD, MB, MI, MS)

#define MOVSDrr(RS, RD)			_SSESDrr (X86_SSE_MOV, RS, RD)
#define MOVSDmr(MD, MB, MI, MS, RD)	_SSESDmr (X86_SSE_MOV, MD, MB, MI, MS, RD)
#define MOVSDrm(RS, MD, MB, MI, MS)	_SSESD1rm(X86_SSE_MOV, RS, MD, MB, MI, MS)

#define MOVAPSrr(RS, RD)		_SSEPSrr (X86_SSE_MOVA, RS, RD)
#define MOVAPSmr(MD, MB, MI, MS, RD)	_SSEPSmr (X86_SSE_MOVA, MD, MB, MI, MS, RD)
#define MOVAPSrm(RS, MD, MB, MI, MS)	_SSEPS1rm(X86_SSE_MOVA, RS, MD, MB, MI, MS)

#define MOVAPDrr(RS, RD)		_SSEPDrr (X86_SSE_MOVA, RS, RD)
#define MOVAPDmr(MD, MB, MI, MS, RD)	_SSEPDmr (X86_SSE_MOVA, MD, MB, MI, MS, RD)
#define MOVAPDrm(RS, MD, MB, MI, MS)	_SSEPD1rm(X86_SSE_MOVA, RS, MD, MB, MI, MS)

#define CVTPS2PIrr(RS, RD)		__SSELrr(      X86_SSE_CVTSI, RS,_rX, RD,_rM)
#define CVTPS2PImr(MD, MB, MI, MS, RD)	__SSELmr(      X86_SSE_CVTSI, MD, MB, MI, MS, RD,_rM)
#define CVTPD2PIrr(RS, RD)		 _SSELrr(0x66, X86_SSE_CVTSI, RS,_rX, RD,_rM)
#define CVTPD2PImr(MD, MB, MI, MS, RD)	 _SSELmr(0x66, X86_SSE_CVTSI, MD, MB, MI, MS, RD,_rM)

#define CVTPI2PSrr(RS, RD)		__SSELrr(      X86_SSE_CVTIS, RS,_rM, RD,_rX)
#define CVTPI2PSmr(MD, MB, MI, MS, RD)	__SSELmr(      X86_SSE_CVTIS, MD, MB, MI, MS, RD,_rX)
#define CVTPI2PDrr(RS, RD)		 _SSELrr(0x66, X86_SSE_CVTIS, RS,_rM, RD,_rX)
#define CVTPI2PDmr(MD, MB, MI, MS, RD)	 _SSELmr(0x66, X86_SSE_CVTIS, MD, MB, MI, MS, RD,_rX)

#define CVTPS2PDrr(RS, RD)		__SSELrr(      X86_SSE_CVTSD, RS,_rX, RD,_rX)
#define CVTPS2PDmr(MD, MB, MI, MS, RD)	__SSELmr(      X86_SSE_CVTSD, MD, MB, MI, MS, RD,_rX)
#define CVTPD2PSrr(RS, RD)		 _SSELrr(0x66, X86_SSE_CVTSD, RS,_rX, RD,_rX)
#define CVTPD2PSmr(MD, MB, MI, MS, RD)	 _SSELmr(0x66, X86_SSE_CVTSD, MD, MB, MI, MS, RD,_rX)

#define CVTSS2SDrr(RS, RD)		 _SSELrr(0xf3, X86_SSE_CVTSD, RS,_rX, RD,_rX)
#define CVTSS2SDmr(MD, MB, MI, MS, RD)	 _SSELmr(0xf3, X86_SSE_CVTSD, MD, MB, MI, MS, RD,_rX)
#define CVTSD2SSrr(RS, RD)		 _SSELrr(0xf2, X86_SSE_CVTSD, RS,_rX, RD,_rX)
#define CVTSD2SSmr(MD, MB, MI, MS, RD)	 _SSELmr(0xf2, X86_SSE_CVTSD, MD, MB, MI, MS, RD,_rX)

#define CVTTSS2SILrr(RS, RD)		 _SSELrr(0xf3, X86_SSE_CVTTSI, RS,_rX, RD,_r4)
#define CVTTSS2SILmr(MD, MB, MI, MS, RD) _SSELmr(0xf3, X86_SSE_CVTTSI, MD, MB, MI, MS, RD,_r4)
#define CVTTSD2SILrr(RS, RD)		 _SSELrr(0xf2, X86_SSE_CVTTSI, RS,_rX, RD,_r4)
#define CVTTSD2SILmr(MD, MB, MI, MS, RD) _SSELmr(0xf2, X86_SSE_CVTTSI, MD, MB, MI, MS, RD,_r4)

#define CVTSS2SILrr(RS, RD)		 _SSELrr(0xf3, X86_SSE_CVTSI, RS,_rX, RD,_r4)
#define CVTSS2SILmr(MD, MB, MI, MS, RD)	 _SSELmr(0xf3, X86_SSE_CVTSI, MD, MB, MI, MS, RD,_r4)
#define CVTSD2SILrr(RS, RD)		 _SSELrr(0xf2, X86_SSE_CVTSI, RS,_rX, RD,_r4)
#define CVTSD2SILmr(MD, MB, MI, MS, RD)	 _SSELmr(0xf2, X86_SSE_CVTSI, MD, MB, MI, MS, RD,_r4)

#define CVTSI2SSLrr(RS, RD)		 _SSELrr(0xf3, X86_SSE_CVTIS, RS,_r4, RD,_rX)
#define CVTSI2SSLmr(MD, MB, MI, MS, RD)	 _SSELmr(0xf3, X86_SSE_CVTIS, MD, MB, MI, MS, RD,_rX)
#define CVTSI2SDLrr(RS, RD)		 _SSELrr(0xf2, X86_SSE_CVTIS, RS,_r4, RD,_rX)
#define CVTSI2SDLmr(MD, MB, MI, MS, RD)	 _SSELmr(0xf2, X86_SSE_CVTIS, MD, MB, MI, MS, RD,_rX)

#define MOVDLXrr(RS, RD)		 _SSELrr(0x66, 0x6e, RS,_r4, RD,_rX)
#define MOVDLXmr(MD, MB, MI, MS, RD)	 _SSELmr(0x66, 0x6e, MD, MB, MI, MS, RD,_rX)

#define MOVDXLrr(RS, RD)		 _SSELrr(0x66, 0x7e, RS,_rX, RD,_r4)
#define MOVDXLrm(RS, MD, MB, MI, MS)	 _SSELrm(0x66, 0x7e, RS,_rX, MD, MB, MI, MS)

#define MOVDLMrr(RS, RD)		__SSELrr(      0x6e, RS,_r4, RD,_rM)
#define MOVDLMmr(MD, MB, MI, MS, RD)	__SSELmr(      0x6e, MD, MB, MI, MS, RD,_rM)

#define MOVDMLrr(RS, RD)		__SSELrr(      0x7e, RS,_rM, RD,_r4)
#define MOVDMLrm(RS, MD, MB, MI, MS)	__SSELrm(      0x7e, RS,_rM, MD, MB, MI, MS)

#define MOVDQ2Qrr(RS, RD)		 _SSELrr(0xf2, X86_SSE_MOV2, RS,_rX, RD,_rM)
#define MOVQ2DQrr(RS, RD)		 _SSELrr(0xf3, X86_SSE_MOV2, RS,_rM, RD,_rX)
#define MOVHLPSrr(RS, RD)		__SSELrr(      X86_SSE_MOVLP, RS,_rX, RD,_rX)
#define MOVLHPSrr(RS, RD)		__SSELrr(      X86_SSE_MOVHP, RS,_rX, RD,_rX)

#define MOVDQArr(RS, RD)		 _SSELrr(0x66, 0x6f, RS,_rX, RD,_rX)
#define MOVDQAmr(MD, MB, MI, MS, RD)	 _SSELmr(0x66, 0x6f, MD, MB, MI, MS, RD,_rX)
#define MOVDQArm(RS, MD, MB, MI, MS)	 _SSELrm(0x66, 0x7f, RS,_rX, MD, MB, MI, MS)

#define MOVDQUrr(RS, RD)		 _SSELrr(0xf3, 0x6f, RS,_rX, RD,_rX)
#define MOVDQUmr(MD, MB, MI, MS, RD)	 _SSELmr(0xf3, 0x6f, MD, MB, MI, MS, RD,_rX)
#define MOVDQUrm(RS, MD, MB, MI, MS)	 _SSELrm(0xf3, 0x7f, RS,_rX, MD, MB, MI, MS)

#define MOVHPDmr(MD, MB, MI, MS, RD)	 _SSELmr (0x66, X86_SSE_MOVHP, MD, MB, MI, MS, RD,_rX)
#define MOVHPDrm(RS, MD, MB, MI, MS)	 _SSEL1rm(0x66, X86_SSE_MOVHP, RS,_rX, MD, MB, MI, MS)
#define MOVHPSmr(MD, MB, MI, MS, RD)	__SSELmr (      X86_SSE_MOVHP, MD, MB, MI, MS, RD,_rX)
#define MOVHPSrm(RS, MD, MB, MI, MS)	__SSEL1rm(      X86_SSE_MOVHP, RS,_rX, MD, MB, MI, MS)

#define MOVLPDmr(MD, MB, MI, MS, RD)	 _SSELmr (0x66, X86_SSE_MOVLP, MD, MB, MI, MS, RD,_rX)
#define MOVLPDrm(RS, MD, MB, MI, MS)	 _SSEL1rm(0x66, X86_SSE_MOVLP, RS,_rX, MD, MB, MI, MS)
#define MOVLPSmr(MD, MB, MI, MS, RD)	__SSELmr (      X86_SSE_MOVLP, MD, MB, MI, MS, RD,_rX)
#define MOVLPSrm(RS, MD, MB, MI, MS)	__SSEL1rm(      X86_SSE_MOVLP, RS,_rX, MD, MB, MI, MS)

/*** References:										*/
/*												*/
/* [1] "Intel Architecture Software Developer's Manual Volume 1: Basic Architecture",		*/
/*     Intel Corporation 1997.									*/
/*												*/
/* [2] "Intel Architecture Software Developer's Manual Volume 2: Instruction Set Reference",	*/
/*     Intel Corporation 1997.									*/

#endif
#endif /* __lightning_asm_i386_h */

