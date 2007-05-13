/******************************** -*- C -*- ****************************
 *
 *	Run-time assembler for the SPARC
 *
 ***********************************************************************/


/***********************************************************************
 *
 * Copyright 1999, 2000, 2001, 2002 Ian Piumarta
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





#ifndef __lightning_asm_h
#define __lightning_asm_h


/* <imm> = [0-9]+		-> add i, one parameter (imm)
 * <reg> = %<imm>		-> add r, one parameter (imm	or _Rr(imm) )
 *	   %g<imm>		-> add r, one parameter (imm	or _Rg(imm) )
 *	   %o<imm>		-> add r, one parameter (imm+8	or _Ro(imm) )
 *	   %l<imm>		-> add r, one parameter (imm+16 or _Rl(imm) )
 *	   %i<imm>		-> add r, one parameter (imm+24 or _Ri(imm) )
 * <mem> = <imm>(<reg>)		-> add m, two parameters (reg,imm)
 * <idx> = <reg>(<reg>)		-> add x, two parameters (reg,reg)
 */


typedef unsigned int jit_insn;

#ifndef LIGHTNING_DEBUG
#define _d30(BD)	(_ck_d (30, ((_jit_SL (_jit_UL (BD) - _jit_UL (_jit.x.pc))) >> 2)))
#define _d22(BD)	(_ck_d (22, ((_jit_SL (_jit_UL (BD) - _jit_UL (_jit.x.pc)) >> 2))))

#define _HI(I)		(_jit_UL(I) >>     (10))
#define _LO(I)		(_jit_UL(I) & _MASK(10))

/* register names */

#define _y		0
#define _psr		1

#define _Rr(N)		( 0+(N))
#define _Rg(N)		( 0+(N))
#define _Ro(N)		( 8+(N))
#define _Rl(N)		(16+(N))
#define _Ri(N)		(24+(N))

/* instruction formats -- Figure 5-1, page 44 in */
/* SPARC International, "The SPARC Architecture Manual, Version 8", Prentice-Hall, 1992.  */

#define _0i(RD,     OP2,	  IMM)	_jit_I((0<<30)|		(_u5(RD)<<25)|(_u3(OP2)<<22)|					       _u22(IMM))
#define _0(  A, CC, OP2,	  DSP)	_jit_I((0<<30)|(_u1(A)<<29)|(_u4(CC)<<25)|(_u3(OP2)<<22)|					       _d22(DSP))
#define _0d( A, CC, OP2,	  DSP)	_jit_I((0<<30)|(_u1(A)<<29)|(_u4(CC)<<25)|(_u3(OP2)<<22)|					       _u22(DSP))

#define _1(			  DSP)	_jit_I((1<<30)|										       _d30(DSP))

#define _2( RD, OP3, RS1, I, ASI, RS2)	_jit_I((2<<30)|		(_u5(RD)<<25)|(_u6(OP3)<<19)|(_u5(RS1)<<14)|(_u1(I)<<13)|(_u8(ASI)<<5)|_u5 (RS2))
#define _2i(RD, OP3, RS1, I,	  IMM)	_jit_I((2<<30)|		(_u5(RD)<<25)|(_u6(OP3)<<19)|(_u5(RS1)<<14)|(_u1(I)<<13)|	       _s13(IMM))
#define _2f(RD, OP3, RS1,    OPF, RS2)	_jit_I((2<<30)|		(_u5(RD)<<25)|(_u6(OP3)<<19)|(_u5(RS1)<<14)|		 (_u9(OPF)<<5)|_u5 (RS2))

#define _3( RD, OP3, RS1, I, ASI, RS2)	_jit_I((3<<30)|		(_u5(RD)<<25)|(_u6(OP3)<<19)|(_u5(RS1)<<14)|(_u1(I)<<13)|(_u8(ASI)<<5)|_u5 (RS2))
#define _3i(RD, OP3, RS1, I,	  IMM)	_jit_I((3<<30)|		(_u5(RD)<<25)|(_u6(OP3)<<19)|(_u5(RS1)<<14)|(_u1(I)<<13)|	       _s13(IMM))

#define _FP1(RD, RS1, OPF, RS2)	_2f((RD), 52, (RS1), (OPF), (RS2))
#define _FP2(RD, RS1, OPF, RS2)	_2f((RD), 53, (RS1), (OPF), (RS2))

/* basic instructions  [Section B, page 87] */

#define ADDrrr(RS1, RS2, RD)	_2   ((RD),  0, (RS1), 0, 0, (RS2))
#define ADDrir(RS1, IMM, RD)	_2i  ((RD),  0, (RS1), 1,    (IMM))
#define ADDCCrrr(RS1, RS2, RD)	_2   ((RD), 16, (RS1), 0, 0, (RS2))
#define ADDCCrir(RS1, IMM, RD)	_2i  ((RD), 16, (RS1), 1,    (IMM))
#define ADDXrrr(RS1, RS2, RD)	_2   ((RD),  8, (RS1), 0, 0, (RS2))
#define ADDXrir(RS1, IMM, RD)	_2i  ((RD),  8, (RS1), 1,    (IMM))
#define ADDXCCrrr(RS1, RS2, RD)	_2   ((RD), 24, (RS1), 0, 0, (RS2))
#define ADDXCCrir(RS1, IMM, RD)	_2i  ((RD), 24, (RS1), 1,    (IMM))
#define ANDrrr(RS1, RS2, RD)	_2   ((RD),  1, (RS1), 0, 0, (RS2))
#define ANDrir(RS1, IMM, RD)	_2i  ((RD),  1, (RS1), 1,    (IMM))
#define ANDCCrrr(RS1, RS2, RD)	_2   ((RD), 17, (RS1), 0, 0, (RS2))
#define ANDCCrir(RS1, IMM, RD)	_2i  ((RD), 17, (RS1), 1,    (IMM))

#define BNi(DISP)		_0   (0,  0, 2, (DISP))
#define BN_Ai(DISP)		_0   (1,  0, 2, (DISP))
#define BEi(DISP)		_0   (0,  1, 2, (DISP))
#define BE_Ai(DISP)		_0   (1,  1, 2, (DISP))
#define BLEi(DISP)		_0   (0,  2, 2, (DISP))
#define BLE_Ai(DISP)		_0   (1,  2, 2, (DISP))
#define BLi(DISP)		_0   (0,  3, 2, (DISP))
#define BL_Ai(DISP)		_0   (1,  3, 2, (DISP))
#define BLEUi(DISP)		_0   (0,  4, 2, (DISP))
#define BLEU_Ai(DISP)		_0   (1,  4, 2, (DISP))
#define BCSi(DISP)		_0   (0,  5, 2, (DISP))
#define BCS_Ai(DISP)		_0   (1,  5, 2, (DISP))
#define BNEGi(DISP)		_0   (0,  6, 2, (DISP))
#define BNEG_Ai(DISP)		_0   (1,  6, 2, (DISP))
#define BVSi(DISP)		_0   (0,  7, 2, (DISP))
#define BVS_Ai(DISP)		_0   (1,  7, 2, (DISP))
#define BAi(DISP)		_0   (0,  8, 2, (DISP))
#define BA_Ai(DISP)		_0   (1,  8, 2, (DISP))
#define BNEi(DISP)		_0   (0,  9, 2, (DISP))
#define BNE_Ai(DISP)		_0   (1,  9, 2, (DISP))
#define BGi(DISP)		_0   (0, 10, 2, (DISP))
#define BG_Ai(DISP)		_0   (1, 10, 2, (DISP))
#define BGEi(DISP)		_0   (0, 11, 2, (DISP))
#define BGE_Ai(DISP)		_0   (1, 11, 2, (DISP))
#define BGUi(DISP)		_0   (0, 12, 2, (DISP))
#define BGU_Ai(DISP)		_0   (1, 12, 2, (DISP))
#define BCCi(DISP)		_0   (0, 13, 2, (DISP))
#define BCC_Ai(DISP)		_0   (1, 13, 2, (DISP))
#define BPOSi(DISP)		_0   (0, 14, 2, (DISP))
#define BPOS_Ai(DISP)		_0   (1, 14, 2, (DISP))
#define BVCi(DISP)		_0   (0, 15, 2, (DISP))
#define BVC_Ai(DISP)		_0   (1, 15, 2, (DISP))

#define CALLi(DISP)		_1   ((DISP))

#define FLUSHrr(RS1, RS2)	_2   (0, 0x3b, (RS1), 0, 0, (RS2))
#define FLUSHir(IMM, RS1)	_2i  (0, 0x3b, (RS1), 1,    (IMM))

#define JMPLxr(RS1, RS2, RD)	_2   ((RD), 56, (RS1), 0, 0, (RS2))
#define JMPLmr(RS1, IMM, RD)	_2i  ((RD), 56, (RS1), 1,    (IMM))

#define LDxr(RS1, RS2, RD)	_3   ((RD),  0, (RS1), 0, 0, (RS2))
#define LDmr(RS1, IMM, RD)	_3i  ((RD),  0, (RS1), 1,    (IMM))
#define LDUBxr(RS1, RS2, RD)	_3   ((RD),  1, (RS1), 0, 0, (RS2))
#define LDUBmr(RS1, IMM, RD)	_3i  ((RD),  1, (RS1), 1,    (IMM))
#define LDUHxr(RS1, RS2, RD)	_3   ((RD),  2, (RS1), 0, 0, (RS2))
#define LDUHmr(RS1, IMM, RD)	_3i  ((RD),  2, (RS1), 1,    (IMM))
#define LDDxr(RS1, RS2, RD)	_3   ((RD),  3, (RS1), 0, 0, (RS2))
#define LDDmr(RS1, IMM, RD)	_3i  ((RD),  3, (RS1), 1,    (IMM))
#define LDSBxr(RS1, RS2, RD)	_3   ((RD),  9, (RS1), 0, 0, (RS2))
#define LDSBmr(RS1, IMM, RD)	_3i  ((RD),  9, (RS1), 1,    (IMM))
#define LDSHxr(RS1, RS2, RD)	_3   ((RD), 10, (RS1), 0, 0, (RS2))
#define LDSHmr(RS1, IMM, RD)	_3i  ((RD), 10, (RS1), 1,    (IMM))

#define ORrrr(RS1, RS2, RD)	_2   ((RD),  2, (RS1), 0, 0, (RS2))
#define ORrir(RS1, IMM, RD)	_2i  ((RD),  2, (RS1), 1,    (IMM))
#define ORCCrrr(RS1, RS2, RD)	_2   ((RD), 18, (RS1), 0, 0, (RS2))
#define ORCCrir(RS1, IMM, RD)	_2i  ((RD), 18, (RS1), 1,    (IMM))

#define RDir(RS, RD)		 _2   ((RD), (RS)|0x28, 0, 0, 0,0)
#define RESTORErrr(RS1, RS2, RD) _2   ((RD), 61, (RS1), 0, 0, (RS2))
#define RESTORErir(RS1, IMM, RD) _2i  ((RD), 61, (RS1), 1,    (IMM))

#define SAVErrr(RS1, RS2, RD)	_2   ((RD), 60, (RS1), 0, 0, (RS2))
#define SAVErir(RS1, IMM, RD)	_2i  ((RD), 60, (RS1), 1,    (IMM))
#define SDIVrrr(RS1, RS2, RD)	_2   ((RD), 15, (RS1), 0, 0, (RS2))
#define SDIVrir(RS1, IMM, RD)	_2i  ((RD), 15, (RS1), 1,    (IMM))
#define SDIVCCrrr(RS1, RS2, RD) _2   ((RD), 31, (RS1), 0, 0, (RS2))
#define SDIVCCrir(RS1, IMM, RD) _2i  ((RD), 31, (RS1), 1,    (IMM))
#define SETHIir(IMM, RD)	_0i  ((RD), 4, (IMM))
#define SLLrrr(RS1, RS2, RD)	_2   ((RD), 37, (RS1), 0, 0, (RS2))
#define SLLrir(RS1, IMM, RD)	_2i  ((RD), 37, (RS1), 1,    (IMM))
#define SMULrrr(RS1, RS2, RD)	_2   ((RD), 11, (RS1), 0, 0, (RS2))
#define SMULrir(RS1, IMM, RD)	_2i  ((RD), 11, (RS1), 1,    (IMM))
#define SMULCCrrr(RS1, RS2, RD) _2   ((RD), 27, (RS1), 0, 0, (RS2))
#define SMULCCrir(RS1, IMM, RD) _2i  ((RD), 27, (RS1), 1,    (IMM))
#define SRArrr(RS1, RS2, RD)	_2   ((RD), 39, (RS1), 0, 0, (RS2))
#define SRArir(RS1, IMM, RD)	_2i  ((RD), 39, (RS1), 1,    (IMM))
#define SRLrrr(RS1, RS2, RD)	_2   ((RD), 38, (RS1), 0, 0, (RS2))
#define SRLrir(RS1, IMM, RD)	_2i  ((RD), 38, (RS1), 1,    (IMM))
#define STrx(RS, RD1, RD2)	_3   ((RS),  4, (RD1), 0, 0, (RD2))
#define STrm(RS, RD, IMM)	_3i  ((RS),  4, (RD),  1,    (IMM))
#define STBrx(RS, RD1, RD2)	_3   ((RS),  5, (RD1), 0, 0, (RD2))
#define STBrm(RS, RD, IMM)	_3i  ((RS),  5, (RD),  1,    (IMM))
#define STBAR()			_0i  (0, 0x28, 15, 0, 0)
#define STHrx(RS, RD1, RD2)	_3   ((RS),  6, (RD1), 0, 0, (RD2))
#define STHrm(RS, RD, IMM)	_3i  ((RS),  6, (RD),  1,    (IMM))
#define STDrx(RS, RD1, RD2)	_3   ((RS),  7, (RD1), 0, 0, (RD2))
#define STDrm(RS, RD, IMM)	_3i  ((RS),  7, (RD),  1,    (IMM))
#define SUBrrr(RS1, RS2, RD)	_2   ((RD),  4, (RS1), 0, 0, (RS2))
#define SUBrir(RS1, IMM, RD)	_2i  ((RD),  4, (RS1), 1,    (IMM))
#define SUBCCrrr(RS1, RS2, RD)	_2   ((RD), 20, (RS1), 0, 0, (RS2))
#define SUBCCrir(RS1, IMM, RD)	_2i  ((RD), 20, (RS1), 1,    (IMM))
#define SUBXrrr(RS1, RS2, RD)	_2   ((RD), 12, (RS1), 0, 0, (RS2))
#define SUBXrir(RS1, IMM, RD)	_2i  ((RD), 12, (RS1), 1,    (IMM))
#define SUBXCCrrr(RS1, RS2, RD)	_2   ((RD), 28, (RS1), 0, 0, (RS2))
#define SUBXCCrir(RS1, IMM, RD)	_2i  ((RD), 28, (RS1), 1,    (IMM))

#define UDIVrrr(RS1, RS2, RD)	_2   ((RD), 14, (RS1), 0, 0, (RS2))
#define UDIVrir(RS1, IMM, RD)	_2i  ((RD), 14, (RS1), 1,    (IMM))
#define UDIVCCrrr(RS1, RS2, RD) _2   ((RD), 30, (RS1), 0, 0, (RS2))
#define UDIVCCrir(RS1, IMM, RD) _2i  ((RD), 30, (RS1), 1,    (IMM))
#define UMULrrr(RS1, RS2, RD)	_2   ((RD), 10, (RS1), 0, 0, (RS2))
#define UMULrir(RS1, IMM, RD)	_2i  ((RD), 10, (RS1), 1,    (IMM))
#define UMULCCrrr(RS1, RS2, RD) _2   ((RD), 26, (RS1), 0, 0, (RS2))
#define UMULCCrir(RS1, IMM, RD) _2i  ((RD), 26, (RS1), 1,    (IMM))

#define WRrri(RS1, RS2, RD)	_2   (0, (RD)|0x30,   RS1, 0, 0, (RS2))
#define WRrii(RS1, IMM, RD)	_2i  (0, (RD)|0x30,   RS1, 1,	 (IMM))

#define XORrrr(RS1, RS2, RD)	_2   ((RD),  3, (RS1), 0, 0, (RS2))
#define XORrir(RS1, IMM, RD)	_2i  ((RD),  3, (RS1), 1,    (IMM))
#define XORCCrrr(RS1, RS2, RD)	_2   ((RD), 19, (RS1), 0, 0, (RS2))
#define XORCCrir(RS1, IMM, RD)	_2i  ((RD), 19, (RS1), 1,    (IMM))

/* synonyms */

#define Bi(DISP)		BAi((DISP))
#define B_Ai(DISP)		BA_Ai((DISP))
#define BNZi(DISP)		BNEi((DISP))
#define BNZ_Ai(DISP)		BNE_Ai((DISP))
#define BZi(DISP)		BEi((DISP))
#define BZ_Ai(DISP)		BE_Ai((DISP))
#define BGEUi(DISP)		BCCi((DISP))
#define BGEU_Ai(DISP)		BCC_Ai((DISP))
#define BLUi(DISP)		BCSi((DISP))
#define BLU_Ai(DISP)		BCS_Ai((DISP))

#define LDUWxr(RS1, RS2, RD)	LDxr((RS1), (RS2), (RD))
#define LDUWmr(RS1, IMM, RD)	LDmr((RS1), (IMM), (RD))
#define LDSWxr(RS1, RS2, RD)	LDxr((RS1), (RS2), (RD))
#define LDSWmr(RS1, IMM, RD)	LDmr((RS1), (IMM), (RD))

#define STWrx(RS, RD1, RD2)	STrx((RS), (RD1),   (RD2))
#define STWrm(RS, RD, IMM)	STrm((RS), (RD), (IMM))

/* synthetic instructions [Table A-1, page 85] */

#define BCLRrr(R,S)		ANDNrrr((R), (S), (S))
#define BCLRir(I,R)		ANDNrir((R), (I), (R))
#define BSETrr(R,S)		ORrrr((R), (S), (S))
#define BSETir(I,R)		ORrir((R), (I), (R))
#define BTOGrr(R,S)		XORrrr((R), (S), (S))
#define BTOGir(I,R)		XORrir((R), (I), (R))
#define BTSTrr(R,S)		ANDCCrrr((R), (S), 0)
#define BTSTir(I,R)		ANDCCrir((R), (I), 0)

#define CALLm(R,I)		JMPLmr((R), (I), _Ro(7))
#define CALLx(R,S)		JMPLxr((R), (S), _Ro(7))

#define CLRr(R)			ORrrr(0, 0, (R))
#define CLRBm(R,I)		STBrm(0, (R), (I))
#define CLRBx(R,S)		STBrm(0, (R), (S))
#define CLRHm(R,I)		STHrm(0, (R), (I))
#define CLRHx(R,S)		STHrm(0, (R), (S))
#define CLRm(R,I)		STrm(0, (R), (I))
#define CLRx(R,S)		STrm(0, (R), (S))

#define CMPrr(RS1, RS2)		SUBCCrrr((RS1), (RS2), 0)
#define CMPri(RS1, IMM)		SUBCCrir((RS1), (IMM), 0)

#define DECr(R)			SUBrir((R), 1, (R))
#define DECir(I,R)		SUBrir((R), (I), (R))
#define DECCCr(R)		SUBCCrir((R), 1, (R))
#define DECCCir(I,R)		SUBCCrir((R), (I), (R))

#define INCr(R)			ADDrir((R), 1, (R))
#define INCir(I,R)		ADDrir((R), (I), (R))
#define INCCCr(R)		ADDCCrir((R), 1, (R))
#define INCCCir(I,R)		ADDCCrir((R), (I), (R))

#define JMPm(R,I)		JMPLmr((R), (I), 0)
#define JMPx(R,S)		JMPLxr((R), (S), 0)

#define MOVrr(R,S)		ORrrr(0, (R), (S))
#define MOVir(I, R)		ORrir(0, (I), (R))

#define NEGrr(R,S)		SUBrrr(0, (R), (S))
#define NEGr(R)			SUBrrr(0, (R), (R))
#define NOP()			SETHIir(0, 0)

#define NOTrr(R,S)		XNORrrr((R), 0, (S))
#define NOTr(R)			XNORrrr((R), 0, (R))

#define RESTORE()		RESTORErrr(0, 0, 0)
#define RET()			JMPLmr(_Ri(7),8 ,0)
#define RETL()			JMPLmr(_Ro(7),8 ,0)

#define SAVE()			SAVErrr(0, 0, 0)
#define SETir(I,R)		(_siP(13,(I)) ? MOVir((I),(R)) : SETir2(_HI(I), _LO(I), (R)))
#define SETir2(H,L,R)		(SETHIir(H,R), (L ? ORrir(R,L,R) : 0))

/* BNZ,a executes the delay instruction if NZ (so skips if Z)
 * BZ,a  executes the delay instruction if Z  (so skips if NZ). */
#define SKIPZ()			_0d  (1,  9, 2, 2) /* BNZ,a .+8 */
#define SKIPNZ()		_0d  (1,  1, 2, 2) /* BZ,a  .+8 */
#define SKIP()			_0d  (1,  0, 2, 0) /* BN,a  .   */

#define TSTr(R)			ORCCrrr(0, (R), 0)

#define WRii(IMM, RD)		WRrii(0, (IMM), (RD))
#define WRri(RS2, RD)		WRrri(0, (RS2), (RD))

#define LDFSRx(RS1, RS2)	_3   (0, 33, (RS1), 0, 0, (RS2))
#define LDFSRm(RS1, IMM)	_3i  (0, 33, (RS1), 1,    (IMM))
#define STFSRx(RD1, RD2)	_3   (0, 37, (RD1), 0, 0, (RD2))
#define STFSRm(RD, IMM)		_3i  (0, 37, (RD),  1,    (IMM))

#define FITODrr(FRS, FRD)		_FP1((FRD),  0, 200, (FRS))
#define FITOSrr(FRS, FRD)		_FP1((FRD),  0, 196, (FRS))
#define FDTOIrr(FRS, FRD)		_FP1((FRD),  0, 210, (FRS))
#define FSTOIrr(FRS, FRD)		_FP1((FRD),  0, 209, (FRS))
#define FSTODrr(FRS, FRD)		_FP1((FRD),  0, 201, (FRS))
#define FDTOSrr(FRS, FRD)		_FP1((FRD),  0, 198, (FRS))
#define FMOVSrr(FRS, FRD)		_FP1((FRD),  0,   1, (FRS))
#define FNEGSrr(FRS, FRD)		_FP1((FRD),  0,   5, (FRS))
#define FABSSrr(FRS, FRD)		_FP1((FRD),  0,   9, (FRS))
#define FMOVDrr(FRS, FRD)		_FP1((FRD),  0,   2, (FRS))
#define FNEGDrr(FRS, FRD)		_FP1((FRD),  0,   6, (FRS))
#define FABSDrr(FRS, FRD)		_FP1((FRD),  0,  10, (FRS))
#define FSQRTDrr(FRS, FRD)		_FP1((FRD),  0,  42, (FRS))
#define FSQRTSrr(FRS, FRD)		_FP1((FRD),  0,  41, (FRS))

#define FADDSrrr(FRS1, FRS2, FRD)	_FP1((FRD),  (FRS1),  65, (FRS2))
#define FSUBSrrr(FRS1, FRS2, FRD)	_FP1((FRD),  (FRS1),  69, (FRS2))
#define FMULSrrr(FRS1, FRS2, FRD)	_FP1((FRD),  (FRS1),  73, (FRS2))
#define FDIVSrrr(FRS1, FRS2, FRD)	_FP1((FRD),  (FRS1),  77, (FRS2))

#define FADDDrrr(FRS1, FRS2, FRD)	_FP1((FRD),  (FRS1),  66, (FRS2))
#define FSUBDrrr(FRS1, FRS2, FRD)	_FP1((FRD),  (FRS1),  70, (FRS2))
#define FMULDrrr(FRS1, FRS2, FRD)	_FP1((FRD),  (FRS1),  74, (FRS2))
#define FDIVDrrr(FRS1, FRS2, FRD)	_FP1((FRD),  (FRS1),  78, (FRS2))

#define FCMPSrr(FRS1, FRS2)		_FP2(0,      (FRS1),  81, (FRS2))
#define FCMPDrr(FRS1, FRS2)		_FP2(0,      (FRS1),  82, (FRS2))

#define LDFxr(RS1, RS2, RD)	_3   ((RD), 32, (RS1), 0, 0, (RS2))
#define LDFmr(RS1, IMM, RD)	_3i  ((RD), 32, (RS1), 1,    (IMM))
#define LDDFxr(RS1, RS2, RD)	_3   ((RD), 35, (RS1), 0, 0, (RS2))
#define LDDFmr(RS1, IMM, RD)	_3i  ((RD), 35, (RS1), 1,    (IMM))
#define STFrx(RS, RD1, RD2)	_3   ((RS), 36, (RD1), 0, 0, (RD2))
#define STFrm(RS, RD1, IMM)	_3i  ((RS), 36, (RD1), 1,    (IMM))
#define STDFrx(RS, RD1, RD2)	_3   ((RS), 39, (RD1), 0, 0, (RD2))
#define STDFrm(RS, RD1, IMM)	_3i  ((RS), 39, (RD1), 1,    (IMM))

#define FBNi(DISP)		_0   (0,  0, 6, (DISP))
#define FBN_Ai(DISP)		_0   (1,  0, 6, (DISP))
#define FBNEi(DISP)		_0   (0,  1, 6, (DISP))
#define FBNE_Ai(DISP)		_0   (1,  1, 6, (DISP))
#define FBLGi(DISP)		_0   (0,  2, 6, (DISP))
#define FBLG_Ai(DISP)		_0   (1,  2, 6, (DISP))
#define FBULi(DISP)		_0   (0,  3, 6, (DISP))
#define FBUL_Ai(DISP)		_0   (1,  3, 6, (DISP))
#define FBLi(DISP)		_0   (0,  4, 6, (DISP))
#define FBL_Ai(DISP)		_0   (1,  4, 6, (DISP))
#define FBUGi(DISP)		_0   (0,  5, 6, (DISP))
#define FBUG_Ai(DISP)		_0   (1,  5, 6, (DISP))
#define FBGi(DISP)		_0   (0,  6, 6, (DISP))
#define FBG_Ai(DISP)		_0   (1,  6, 6, (DISP))
#define FBUi(DISP)		_0   (0,  7, 6, (DISP))
#define FBU_Ai(DISP)		_0   (1,  7, 6, (DISP))
#define FBAi(DISP)		_0   (0,  8, 6, (DISP))
#define FBA_Ai(DISP)		_0   (1,  8, 6, (DISP))
#define FBEi(DISP)		_0   (0,  9, 6, (DISP))
#define FBE_Ai(DISP)		_0   (1,  9, 6, (DISP))
#define FBUEi(DISP)		_0   (0, 10, 6, (DISP))
#define FBUE_Ai(DISP)		_0   (1, 10, 6, (DISP))
#define FBGEi(DISP)		_0   (0, 11, 6, (DISP))
#define FBGE_Ai(DISP)		_0   (1, 11, 6, (DISP))
#define FBUGEi(DISP)		_0   (0, 12, 6, (DISP))
#define FBUGE_Ai(DISP)		_0   (1, 12, 6, (DISP))
#define FBLEi(DISP)		_0   (0, 13, 6, (DISP))
#define FBLE_Ai(DISP)		_0   (1, 13, 6, (DISP))
#define FBULEi(DISP)		_0   (0, 14, 6, (DISP))
#define FBULE_Ai(DISP)		_0   (1, 14, 6, (DISP))
#define FBOi(DISP)		_0   (0, 15, 6, (DISP))
#define FBO_Ai(DISP)		_0   (1, 15, 6, (DISP))

#endif
#endif /* __ccg_asm_sparc_h */
