/******************************** -*- C -*- ****************************
 *
 *	Run-time assembler for the i386
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

/*	OPCODE	+ i		= immediate operand
 *		+ r		= register operand
 *		+ m		= memory operand (disp,base,index,scale)
 *		+ sr/sm		= a star preceding a register or memory
 */

#include "asm-i386.h"

#define _r1(R)          ( ((R) & ~3) == _AL || ((R) & ~3) == _AH ? _rN(R) : JITFAIL( "8-bit register required"))
#define _rA(R)          _r4(R)

/* Use RIP-addressing in 64-bit mode, if possible */
#define _r_X(   R, D,B,I,S,O)	(_r0P(I) ? (_r0P(B)    ? _r_D   (R,D                ) : \
				           (_rsp12P(B) ? _r_DBIS(R,D,_ESP,_ESP,1)   : \
						         _r_DB  (R,D,     B       )))  : \
				 (_r0P(B)	       ? _r_4IS (R,D,	         I,S)   : \
				 (!_rspP(I)            ? _r_DBIS(R,D,     B,     I,S)   : \
						         JITFAIL("illegal index register: %esp"))))
#define _m32only(X)		(X)
#define _m64only(X)		JITFAIL("invalid instruction in 32-bit mode")
#define _m64(X)			((void)0)

#define _AH		0x24
#define _CH		0x25
#define _DH		0x26
#define _BH		0x27

#define CALLsr(R)			CALLLsr(R)
#define JMPsr(R)			JMPLsr(R)

#define DECWr(RD)	(_d16(),	_Or		(0x48,_r2(RD)							))
#define DECLr(RD)		 	_Or		(0x48,_r4(RD)							)
#define INCWr(RD)	(_d16(),	_Or		(0x40,_r2(RD)							))
#define INCLr(RD)	 		_Or		(0x40,_r4(RD)							)

#endif
#endif /* __lightning_asm_h */
