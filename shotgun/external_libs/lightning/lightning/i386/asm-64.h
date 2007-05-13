/******************************** -*- C -*- ****************************
 *
 *	Run-time assembler for the x86-64
 *
 ***********************************************************************/


/***********************************************************************
 *
 * Copyright 2003 Gwenole Beauchesne
 * Copyright 2006 Free Software Foundation, Inc.
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
 * Free Software Foundation, 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 *
 ***********************************************************************/




#ifndef __lightning_asm_h
#define __lightning_asm_h

#ifndef LIGHTNING_DEBUG

#include "asm-i386.h"

/*	OPCODE	+ i		= immediate operand
 *		+ r		= register operand
 *		+ m		= memory operand (disp,base,index,scale)
 *		+ sr/sm		= a star preceding a register or memory
 */


#define _rA(R)          _r8(R)

/* Use RIP-addressing in 64-bit mode, if possible */
#if 0
#define _x86_RIP_addressing_possible(D,O)	(X86_RIP_RELATIVE_ADDR && \
						((unsigned long)x86_get_target() + 4 + (O) - (D) <= 0xffffffff))

#define _r_X(   R, D,B,I,S,O)	(_r0P(I) ? (_r0P(B)    ? (!X86_TARGET_64BIT ? _r_D(R,D) : \
					                 (_x86_RIP_addressing_possible(D, O) ? \
				                          _r_D(R, (D) - ((unsigned long)x86_get_target() + 4 + (O))) : \
				                          _r_DSIB(R,D))) : \
					                 _r_DSIB(R,D                ))  : \
				           (_rIP(B)    ? _r_D   (R,D                )   : \
				           (_rsp12P(B) ? _r_DBIS(R,D,_RSP,_RSP,1)   : \
						         _r_DB  (R,D,     B       ))))  : \
				 (_r0P(B)	       ? _r_4IS (R,D,	         I,S)   : \
				 (!_rspP(I)            ? _r_DBIS(R,D,     B,     I,S)   : \
						         JITFAIL("illegal index register: %esp"))))
#else
#define _r_X(   R, D,B,I,S,O)	(_r0P(I) ? (_r0P(B)    ? _r_DSIB(R,D                )   : \
				           (_rIP(B)    ? _r_D   (R,D                )   : \
				           (_rsp12P(B) ? _r_DBIS(R,D,_RSP,_RSP,1)   : \
						         _r_DB  (R,D,     B       ))))  : \
				 (_r0P(B)	       ? _r_4IS (R,D,	         I,S)   : \
				 (!_rspP(I)            ? _r_DBIS(R,D,     B,     I,S)   : \
						         JITFAIL("illegal index register: %esp"))))
#endif


#define _m32only(X)		(JITFAIL("invalid instruction in 64-bit mode"))
#define _m64only(X)		(X)
#define _m64(X)			(X)

#define _SPL		0x14
#define _BPL		0x15
#define _SIL		0x16
#define _DIL		0x17
#define _R8B		0x18
#define _R9B		0x19
#define _R10B		0x1A
#define _R11B		0x1B
#define _R12B		0x1C
#define _R13B		0x1D
#define _R14B		0x1E
#define _R15B		0x1F

#define _R8W		0x38
#define _R9W		0x39
#define _R10W		0x3A
#define _R11W		0x3B
#define _R12W		0x3C
#define _R13W		0x3D
#define _R14W		0x3E
#define _R15W		0x3F
#define _R8D		0x48
#define _R9D		0x49
#define _R10D		0x4A
#define _R11D		0x4B
#define _R12D		0x4C
#define _R13D		0x4D
#define _R14D		0x4E
#define _R15D		0x4F

#define _RAX		0x50
#define _RCX		0x51
#define _RDX		0x52
#define _RBX		0x53
#define _RSP		0x54
#define _RBP		0x55
#define _RSI		0x56
#define _RDI		0x57
#define _R8		0x48
#define _R9		0x49
#define _R10		0x4A
#define _R11		0x4B
#define _R12		0x4C
#define _R13		0x4D
#define _R14		0x4E
#define _R15		0x4F
#define _RIP		-2

#define _r1(R)          ( ((unsigned) _rC((R) - 16)) < (0x30 - 16)      ? _rN(R) : JITFAIL( "8-bit register required"))

#if 0
#define _r8(R)		( (_rC(R) == 0x50)			? _rN(R) : JITFAIL("64-bit register required"))
#else
#define _r8(R)		( (_rC(R) == 0x50)			? _rN(R) : _r4(R))
#endif

#define _r1e8lP(R)	((int)(R) >= _SPL && (int)(R) <= _DIL)

#define DECWr(RD)	(_d16(), _REXLrr(0, RD),	_O_Mrm		(0xff		,_b11,_b001  ,_r2(RD)				))
#define DECLr(RD)	(_REXLrr(0, RD),		_O_Mrm		(0xff		,_b11,_b001  ,_r4(RD)				))
#define INCWr(RD)	(_d16(), _REXLrr(0, RD),	_O_Mrm		(0xff		,_b11,_b000  ,_r2(RD)				))
#define INCLr(RD)	(_REXLrr(0, RD),		_O_Mrm		(0xff		,_b11,_b000  ,_r4(RD)				))

#define ADCQrr(RS, RD)			_ALUQrr(X86_ADC, RS, RD)
#define ADCQmr(MD, MB, MI, MS, RD)	_ALUQmr(X86_ADC, MD, MB, MI, MS, RD)
#define ADCQrm(RS, MD, MB, MI, MS)	_ALUQrm(X86_ADC, RS, MD, MB, MI, MS)
#define ADCQir(IM, RD)			_ALUQir(X86_ADC, IM, RD)
#define ADCQim(IM, MD, MB, MI, MS)	_ALUQim(X86_ADC, IM, MD, MB, MI, MS)

#define ADDQrr(RS, RD)			_ALUQrr(X86_ADD, RS, RD)
#define ADDQmr(MD, MB, MI, MS, RD)	_ALUQmr(X86_ADD, MD, MB, MI, MS, RD)
#define ADDQrm(RS, MD, MB, MI, MS)	_ALUQrm(X86_ADD, RS, MD, MB, MI, MS)
#define ADDQir(IM, RD)			_ALUQir(X86_ADD, IM, RD)
#define ADDQim(IM, MD, MB, MI, MS)	_ALUQim(X86_ADD, IM, MD, MB, MI, MS)

#define ANDQrr(RS, RD)			_ALUQrr(X86_AND, RS, RD)
#define ANDQmr(MD, MB, MI, MS, RD)	_ALUQmr(X86_AND, MD, MB, MI, MS, RD)
#define ANDQrm(RS, MD, MB, MI, MS)	_ALUQrm(X86_AND, RS, MD, MB, MI, MS)
#define ANDQir(IM, RD)			_ALUQir(X86_AND, IM, RD)
#define ANDQim(IM, MD, MB, MI, MS)	_ALUQim(X86_AND, IM, MD, MB, MI, MS)

#define CMPQrr(RS, RD)			_ALUQrr(X86_CMP, RS, RD)
#define CMPQmr(MD, MB, MI, MS, RD)	_ALUQmr(X86_CMP, MD, MB, MI, MS, RD)
#define CMPQrm(RS, MD, MB, MI, MS)	_ALUQrm(X86_CMP, RS, MD, MB, MI, MS)
#define CMPQir(IM, RD)			_ALUQir(X86_CMP, IM, RD)
#define CMPQim(IM, MD, MB, MI, MS)	_ALUQim(X86_CMP, IM, MD, MB, MI, MS)

#define ORQrr(RS, RD)			_ALUQrr(X86_OR, RS, RD)
#define ORQmr(MD, MB, MI, MS, RD)	_ALUQmr(X86_OR, MD, MB, MI, MS, RD)
#define ORQrm(RS, MD, MB, MI, MS)	_ALUQrm(X86_OR, RS, MD, MB, MI, MS)
#define ORQir(IM, RD)			_ALUQir(X86_OR, IM, RD)
#define ORQim(IM, MD, MB, MI, MS)	_ALUQim(X86_OR, IM, MD, MB, MI, MS)

#define SBBQrr(RS, RD)			_ALUQrr(X86_SBB, RS, RD)
#define SBBQmr(MD, MB, MI, MS, RD)	_ALUQmr(X86_SBB, MD, MB, MI, MS, RD)
#define SBBQrm(RS, MD, MB, MI, MS)	_ALUQrm(X86_SBB, RS, MD, MB, MI, MS)
#define SBBQir(IM, RD)			_ALUQir(X86_SBB, IM, RD)
#define SBBQim(IM, MD, MB, MI, MS)	_ALUQim(X86_SBB, IM, MD, MB, MI, MS)

#define SUBQrr(RS, RD)			_ALUQrr(X86_SUB, RS, RD)
#define SUBQmr(MD, MB, MI, MS, RD)	_ALUQmr(X86_SUB, MD, MB, MI, MS, RD)
#define SUBQrm(RS, MD, MB, MI, MS)	_ALUQrm(X86_SUB, RS, MD, MB, MI, MS)
#define SUBQir(IM, RD)			_ALUQir(X86_SUB, IM, RD)
#define SUBQim(IM, MD, MB, MI, MS)	_ALUQim(X86_SUB, IM, MD, MB, MI, MS)

#define XORQrr(RS, RD)			_ALUQrr(X86_XOR, RS, RD)
#define XORQmr(MD, MB, MI, MS, RD)	_ALUQmr(X86_XOR, MD, MB, MI, MS, RD)
#define XORQrm(RS, MD, MB, MI, MS)	_ALUQrm(X86_XOR, RS, MD, MB, MI, MS)
#define XORQir(IM, RD)			_ALUQir(X86_XOR, IM, RD)
#define XORQim(IM, MD, MB, MI, MS)	_ALUQim(X86_XOR, IM, MD, MB, MI, MS)

#define ROLQir(IM, RD)			_ROTSHIQir(X86_ROL, IM, RD)
#define ROLQim(IM, MD, MB, MI, MS)	_ROTSHIQim(X86_ROL, IM, MD, MB, MI, MS)
#define ROLQrr(RS, RD)			_ROTSHIQrr(X86_ROL, RS, RD)
#define ROLQrm(RS, MD, MB, MI, MS)	_ROTSHIQrm(X86_ROL, RS, MD, MB, MI, MS)

#define RORQir(IM, RD)			_ROTSHIQir(X86_ROR, IM, RD)
#define RORQim(IM, MD, MB, MI, MS)	_ROTSHIQim(X86_ROR, IM, MD, MB, MI, MS)
#define RORQrr(RS, RD)			_ROTSHIQrr(X86_ROR, RS, RD)
#define RORQrm(RS, MD, MB, MI, MS)	_ROTSHIQrm(X86_ROR, RS, MD, MB, MI, MS)

#define RCLQir(IM, RD)			_ROTSHIQir(X86_RCL, IM, RD)
#define RCLQim(IM, MD, MB, MI, MS)	_ROTSHIQim(X86_RCL, IM, MD, MB, MI, MS)
#define RCLQrr(RS, RD)			_ROTSHIQrr(X86_RCL, RS, RD)
#define RCLQrm(RS, MD, MB, MI, MS)	_ROTSHIQrm(X86_RCL, RS, MD, MB, MI, MS)

#define RCRQir(IM, RD)			_ROTSHIQir(X86_RCR, IM, RD)
#define RCRQim(IM, MD, MB, MI, MS)	_ROTSHIQim(X86_RCR, IM, MD, MB, MI, MS)
#define RCRQrr(RS, RD)			_ROTSHIQrr(X86_RCR, RS, RD)
#define RCRQrm(RS, MD, MB, MI, MS)	_ROTSHIQrm(X86_RCR, RS, MD, MB, MI, MS)

#define SHLQir(IM, RD)			_ROTSHIQir(X86_SHL, IM, RD)
#define SHLQim(IM, MD, MB, MI, MS)	_ROTSHIQim(X86_SHL, IM, MD, MB, MI, MS)
#define SHLQrr(RS, RD)			_ROTSHIQrr(X86_SHL, RS, RD)
#define SHLQrm(RS, MD, MB, MI, MS)	_ROTSHIQrm(X86_SHL, RS, MD, MB, MI, MS)

#define SHRQir(IM, RD)			_ROTSHIQir(X86_SHR, IM, RD)
#define SHRQim(IM, MD, MB, MI, MS)	_ROTSHIQim(X86_SHR, IM, MD, MB, MI, MS)
#define SHRQrr(RS, RD)			_ROTSHIQrr(X86_SHR, RS, RD)
#define SHRQrm(RS, MD, MB, MI, MS)	_ROTSHIQrm(X86_SHR, RS, MD, MB, MI, MS)

#define SALQir				SHLQir
#define SALQim				SHLQim
#define SALQrr				SHLQrr
#define SALQrm				SHLQrm

#define SARQir(IM, RD)			_ROTSHIQir(X86_SAR, IM, RD)
#define SARQim(IM, MD, MB, MI, MS)	_ROTSHIQim(X86_SAR, IM, MD, MB, MI, MS)
#define SARQrr(RS, RD)			_ROTSHIQrr(X86_SAR, RS, RD)
#define SARQrm(RS, MD, MB, MI, MS)	_ROTSHIQrm(X86_SAR, RS, MD, MB, MI, MS)

#define BTQir(IM, RD)			_BTQir(X86_BT, IM, RD)
#define BTQim(IM, MD, MB, MI, MS)	_BTQim(X86_BT, IM, MD, MB, MI, MS)
#define BTQrr(RS, RD)			_BTQrr(X86_BT, RS, RD)
#define BTQrm(RS, MD, MB, MI, MS)	_BTQrm(X86_BT, RS, MD, MB, MI, MS)

#define BTCQir(IM, RD)			_BTQir(X86_BTC, IM, RD)
#define BTCQim(IM, MD, MB, MI, MS)	_BTQim(X86_BTC, IM, MD, MB, MI, MS)
#define BTCQrr(RS, RD)			_BTQrr(X86_BTC, RS, RD)
#define BTCQrm(RS, MD, MB, MI, MS)	_BTQrm(X86_BTC, RS, MD, MB, MI, MS)

#define BTRQir(IM, RD)			_BTQir(X86_BTR, IM, RD)
#define BTRQim(IM, MD, MB, MI, MS)	_BTQim(X86_BTR, IM, MD, MB, MI, MS)
#define BTRQrr(RS, RD)			_BTQrr(X86_BTR, RS, RD)
#define BTRQrm(RS, MD, MB, MI, MS)	_BTQrm(X86_BTR, RS, MD, MB, MI, MS)

#define BTSQir(IM, RD)			_BTQir(X86_BTS, IM, RD)
#define BTSQim(IM, MD, MB, MI, MS)	_BTQim(X86_BTS, IM, MD, MB, MI, MS)
#define BTSQrr(RS, RD)			_BTQrr(X86_BTS, RS, RD)
#define BTSQrm(RS, MD, MB, MI, MS)	_BTQrm(X86_BTS, RS, MD, MB, MI, MS)

#define MOVQrr(RS, RD)			(_REXQrr(RS, RD),		_O_Mrm		(0x89		,_b11,_r8(RS),_r8(RD)				))
#define MOVQmr(MD, MB, MI, MS, RD)	(_REXQmr(MB, MI, RD),		_O_r_X		(0x8b		     ,_r8(RD)		,MD,MB,MI,MS		))
#define MOVQrm(RS, MD, MB, MI, MS)	(_REXQrm(RS, MB, MI),		_O_r_X		(0x89		     ,_r8(RS)		,MD,MB,MI,MS		))
#define MOVQir(IM,  R)			(_REXQrr(0, R),			_Or_Q		(0xb8,_r8(R)						,IM	))
#define MOVQim(IM, MD, MB, MI, MS)	(_REXQrm(0, MB, MI),		_O_X_L		(0xc7					,MD,MB,MI,MS	,IM	))

#define NOTQr(RS)			_UNARYQr(X86_NOT, RS)
#define NOTQm(MD, MB, MI, MS)		_UNARYQm(X86_NOT, MD, MB, MI, MS)

#define NEGQr(RS)			_UNARYQr(X86_NEG, RS)
#define NEGQm(MD, MB, MI, MS)		_UNARYQm(X86_NEG, MD, MB, MI, MS)

#define MULQr(RS)			_UNARYQr(X86_MUL, RS)
#define MULQm(MD, MB, MI, MS)		_UNARYQm(X86_MUL, MD, MB, MI, MS)

#define IMULQr(RS)			_UNARYQr(X86_IMUL, RS)
#define IMULQm(MD, MB, MI, MS)		_UNARYQm(X86_IMUL, MD, MB, MI, MS)

#define DIVQr(RS)			_UNARYQr(X86_DIV, RS)
#define DIVQm(MD, MB, MI, MS)		_UNARYQm(X86_DIV, MD, MB, MI, MS)

#define IDIVQr(RS)			_UNARYQr(X86_IDIV, RS)
#define IDIVQm(MD, MB, MI, MS)		_UNARYQm(X86_IDIV, MD, MB, MI, MS)

#define IMULQir(IM, RD)			(_REXQrr(0, RD),		_Os_Mrm_sL	(0x69		,_b11,_r8(RD),_r8(RD)			,IM	))
#define IMULQrr(RS, RD)			(_REXQrr(RD, RS),		_OO_Mrm		(0x0faf		,_b11,_r8(RD),_r8(RS)				))
#define IMULQmr(MD, MB, MI, MS, RD)	(_REXQmr(MB, MI, RD),		_OO_r_X		(0x0faf		     ,_r8(RD)		,MD,MB,MI,MS		))
#define IMULQirr(IM,RS,RD)		(_REXQrr(RS, RD),		_Os_Mrm_sL	(0x69		,_b11,_r8(RS),_r8(RD)			,IM	))
#define IMULQimr(IM,MD,MB,MI,MS,RD)	(_REXQmr(MB, MI, RD),		_Os_r_X_sL	(0x69		     ,_r8(RD)		,MD,MB,MI,MS	,IM	))

#define CALLQsr(R)			(_REXQrr(0, R),			_O_Mrm		(0xff		,_b11,_b010,_r8(R)				))
#define JMPQsr(R)			(_REXQrr(0, R),			_O_Mrm		(0xff		,_b11,_b100,_r8(R)				))

#define CMOVQrr(CC,RS,RD)		(_REXQrr(RD, RS),		_OO_Mrm		(0x0f40|(CC)	,_b11,_r8(RD),_r8(RS)				))
#define CMOVQmr(CC,MD,MB,MI,MS,RD)	(_REXQmr(MB, MI, RD),		_OO_r_X		(0x0f40|(CC)	     ,_r8(RD)		,MD,MB,MI,MS		))

#define POPQr(RD)			_m64only((_REXQr(RD),		_Or		(0x58,_r8(RD)							)))
#define POPQm(MD, MB, MI, MS)		_m64only((_REXQm(MB, MI), 	_O_r_X		(0x8f		     ,_b000		,MD,MB,MI,MS		)))

#define PUSHQr(RS)			_m64only((_REXQr(RS),		_Or		(0x50,_r8(RS)							)))
#define PUSHQm(MD, MB, MI, MS)		_m64only((_REXQm(MB, MI),	_O_r_X		(0xff		     ,_b110		,MD,MB,MI,MS		)))
#define PUSHQi(IM)			_m64only(			_Os_sL		(0x68							,IM	))

#define TESTQrr(RS, RD)			(_REXQrr(RS, RD),		_O_Mrm		(0x85		,_b11,_r8(RS),_r8(RD)				))
#define TESTQrm(RS, MD, MB, MI, MS)	(_REXQrm(RS, MB, MI),		_O_r_X		(0x85		     ,_r8(RS)		,MD,MB,MI,MS		))
#define TESTQir(IM, RD)			(!_s8P(IM) && (RD) == _RAX ? \
                                        (_REXQrr(0, RD),                _O_L            (0xa9                                                   ,IM     )) : \
                                        (_REXQrr(0, RD),                _O_Mrm_L        (0xf7           ,_b11,_b000  ,_r8(RD)                   ,IM     )) )
#define TESTQim(IM, MD, MB, MI, MS)	(_REXQrm(0, MB, MI),		_O_r_X_L	(0xf7		     ,_b000		,MD,MB,MI,MS	,IM	))

#define CMPXCHGQrr(RS, RD)		(_REXQrr(RS, RD),		_OO_Mrm		(0x0fb1		,_b11,_r8(RS),_r8(RD)				))
#define CMPXCHGQrm(RS, MD, MB, MI, MS)	(_REXQrm(RS, MB, MI),		_OO_r_X		(0x0fb1		     ,_r8(RS)		,MD,MB,MI,MS		))

#define XADDQrr(RS, RD)			(_REXQrr(RS, RD),		_OO_Mrm		(0x0fc1		,_b11,_r8(RS),_r8(RD)				))
#define XADDQrm(RS, MD, MB, MI, MS)	(_REXQrm(RS, MB, MI),		_OO_r_X		(0x0fc1		     ,_r8(RS)		,MD,MB,MI,MS		))

#define XCHGQrr(RS, RD)			(_REXQrr(RS, RD),		_O_Mrm		(0x87		,_b11,_r8(RS),_r8(RD)				))
#define XCHGQrm(RS, MD, MB, MI, MS)	(_REXQrm(RS, MB, MI),		_O_r_X		(0x87		     ,_r8(RS)		,MD,MB,MI,MS		))

#define DECQm(MD, MB, MI, MS)		(_REXQrm(0, MB, MI),		_O_r_X		(0xff		     ,_b001		,MD,MB,MI,MS		))
#define DECQr(RD)			(_REXQrr(0, RD),		_O_Mrm		(0xff		,_b11,_b001  ,_r8(RD)				))
#define INCQm(MD, MB, MI, MS)		(_REXQrm(0, MB, MI),		_O_r_X		(0xff		     ,_b000		,MD,MB,MI,MS		))
#define INCQr(RD)			(_REXQrr(0, RD),		_O_Mrm		(0xff		,_b11,_b000  ,_r8(RD)				))

#define BSFQrr(RS, RD)			(_REXQrr(RD, RS),		_OO_Mrm		(0x0fbc		,_b11,_r8(RD),_r8(RS)				))
#define BSFQmr(MD, MB, MI, MS, RD)	(_REXQmr(MB, MI, RD),		_OO_r_X		(0x0fbc		     ,_r8(RD)		,MD,MB,MI,MS		))

#define BSRQrr(RS, RD)			(_REXQrr(RD, RS),		_OO_Mrm		(0x0fbd		,_b11,_r8(RD),_r8(RS)				))
#define BSRQmr(MD, MB, MI, MS, RD)	(_REXQmr(MB, MI, RD),		_OO_r_X		(0x0fbd		     ,_r8(RD)		,MD,MB,MI,MS		))

#define MOVSBQrr(RS, RD)		(_REXQrr(RD, RS),		_OO_Mrm		(0x0fbe		,_b11,_r8(RD),_r1(RS)				))
#define MOVSBQmr(MD, MB, MI, MS, RD)	(_REXQmr(MB, MI, RD),		_OO_r_X		(0x0fbe		     ,_r8(RD)		,MD,MB,MI,MS		))

#define MOVZBQrr(RS, RD)		(_REXQrr(RD, RS),		_OO_Mrm		(0x0fb6		,_b11,_r8(RD),_r1(RS)				))
#define MOVZBQmr(MD, MB, MI, MS, RD)	(_REXQmr(MB, MI, RD),		_OO_r_X		(0x0fb6		     ,_r8(RD)		,MD,MB,MI,MS		))

#define MOVSWQrr(RS, RD)		(_REXQrr(RD, RS),		_OO_Mrm		(0x0fbf		,_b11,_r8(RD),_r2(RS)				))
#define MOVSWQmr(MD, MB, MI, MS, RD)	(_REXQmr(MB, MI, RD),		_OO_r_X		(0x0fbf		     ,_r8(RD)		,MD,MB,MI,MS		))

#define MOVZWQrr(RS, RD)		(_REXQrr(RD, RS),		_OO_Mrm		(0x0fb7		,_b11,_r8(RD),_r2(RS)				))
#define MOVZWQmr(MD, MB, MI, MS, RD)	(_REXQmr(MB, MI, RD),		_OO_r_X		(0x0fb7		     ,_r8(RD)		,MD,MB,MI,MS		))

#define MOVSLQrr(RS, RD)		_m64only((_REXQrr(RD, RS),	_O_Mrm		(0x63		,_b11,_r8(RD),_r4(RS)				)))
#define MOVSLQmr(MD, MB, MI, MS, RD)	_m64only((_REXQmr(MB, MI, RD),	_O_r_X		(0x63		     ,_r8(RD)		,MD,MB,MI,MS		)))

#define BSWAPQr(R)			(_REXQrr(0, R),			_OOr		(0x0fc8,_r8(R)							))



#define __SSEQrr(OP,RS,RSA,RD,RDA)		(_REXQrr(RD, RS),		_OO_Mrm		(0x0f00|(OP)	,_b11,RDA(RD),RSA(RS)				))
#define __SSEQmr(OP,MD,MB,MI,MS,RD,RDA)		(_REXQmr(MB, MI, RD),		_OO_r_X		(0x0f00|(OP)	     ,RDA(RD)		,MD,MB,MI,MS		))
#define __SSEQrm(OP,RS,RSA,MD,MB,MI,MS)		(_REXQrm(RS, MB, MI),		_OO_r_X		(0x0f00|(OP)	     ,RSA(RS)		,MD,MB,MI,MS		))
#define __SSEQ1rm(OP,RS,RSA,MD,MB,MI,MS)	(_REXQrm(RS, MB, MI),		_OO_r_X		(0x0f01|(OP)	     ,RSA(RS)		,MD,MB,MI,MS		))

#define _SSEQrr(PX,OP,RS,RSA,RD,RDA)					(_jit_B(PX), __SSEQrr(OP, RS, RSA, RD, RDA))
#define _SSEQmr(PX,OP,MD,MB,MI,MS,RD,RDA)				(_jit_B(PX), __SSEQmr(OP, MD, MB, MI, MS, RD, RDA))
#define _SSEQrm(PX,OP,RS,RSA,MD,MB,MI,MS)				(_jit_B(PX), __SSEQrm(OP, RS, RSA, MD, MB, MI, MS))
#define _SSEQ1rm(PX,OP,RS,RSA,MD,MB,MI,MS)				(_jit_B(PX), __SSEQ1rm(OP, RS, RSA, MD, MB, MI, MS))

#define CVTTSS2SIQrr(RS, RD)		 _SSEQrr(0xf3, X86_SSE_CVTTSI, RS,_rX, RD,_r8)
#define CVTTSS2SIQmr(MD, MB, MI, MS, RD) _SSEQmr(0xf3, X86_SSE_CVTTSI, MD, MB, MI, MS, RD,_r8)
#define CVTTSD2SIQrr(RS, RD)		 _SSEQrr(0xf2, X86_SSE_CVTTSI, RS,_rX, RD,_r8)
#define CVTTSD2SIQmr(MD, MB, MI, MS, RD) _SSEQmr(0xf2, X86_SSE_CVTTSI, MD, MB, MI, MS, RD,_r8)

#define CVTSS2SIQrr(RS, RD)		 _SSEQrr(0xf3, X86_SSE_CVTSI, RS,_rX, RD,_r8)
#define CVTSS2SIQmr(MD, MB, MI, MS, RD)	 _SSEQmr(0xf3, X86_SSE_CVTSI, MD, MB, MI, MS, RD,_r8)
#define CVTSD2SIQrr(RS, RD)		 _SSEQrr(0xf2, X86_SSE_CVTSI, RS,_rX, RD,_r8)
#define CVTSD2SIQmr(MD, MB, MI, MS, RD)	 _SSEQmr(0xf2, X86_SSE_CVTSI, MD, MB, MI, MS, RD,_r8)

#define CVTSI2SSQrr(RS, RD)		 _SSEQrr(0xf3, X86_SSE_CVTIS, RS,_r8, RD,_rX)
#define CVTSI2SSQmr(MD, MB, MI, MS, RD)	 _SSEQmr(0xf3, X86_SSE_CVTIS, MD, MB, MI, MS, RD,_rX)
#define CVTSI2SDQrr(RS, RD)		 _SSEQrr(0xf2, X86_SSE_CVTIS, RS,_r8, RD,_rX)
#define CVTSI2SDQmr(MD, MB, MI, MS, RD)	 _SSEQmr(0xf2, X86_SSE_CVTIS, MD, MB, MI, MS, RD,_rX)

#define MOVDQXrr(RS, RD)		 _SSEQrr(0x66, 0x6e, RS,_r8, RD,_rX)
#define MOVDQXmr(MD, MB, MI, MS, RD)	 _SSEQmr(0x66, 0x6e, MD, MB, MI, MS, RD,_rX)

#define MOVDXQrr(RS, RD)		 _SSEQrr(0x66, 0x7e, RS,_rX, RD,_r8)
#define MOVDXQrm(RS, MD, MB, MI, MS)	 _SSEQrm(0x66, 0x7e, RS,_rX, MD, MB, MI, MS)
#define MOVDQMrr(RS, RD)		__SSEQrr(      0x6e, RS,_r8, RD,_rM)
#define MOVDQMmr(MD, MB, MI, MS, RD)	__SSEQmr(      0x6e, MD, MB, MI, MS, RD,_rM)
#define MOVDMQrr(RS, RD)		__SSEQrr(      0x7e, RS,_rM, RD,_r8)
#define MOVDMQrm(RS, MD, MB, MI, MS)	__SSEQrm(      0x7e, RS,_rM, MD, MB, MI, MS)



#define CALLsr(R)			CALLQsr(R)
#define JMPsr(R)			JMPQsr(R)

#endif
#endif /* __lightning_asm_h */

