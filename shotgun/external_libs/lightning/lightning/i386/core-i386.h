/******************************** -*- C -*- ****************************
 *
 *	Platform-independent layer (i386 version)
 *
 ***********************************************************************/


/***********************************************************************
 *
 * Copyright 2000, 2001, 2002, 2003, 2006 Free Software Foundation, Inc.
 * Written by Paolo Bonzini and Matthew Flatt.
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



#ifndef __lightning_core_i386_h
#define __lightning_core_i386_h

#define JIT_FP			_EBP
#define JIT_SP			_ESP
#define JIT_RET			_EAX

#define JIT_R_NUM		3
#define JIT_V_NUM		3
#define JIT_R(i)		(_EAX + (i))
#define JIT_V(i)		((i) == 0 ? _EBX : _ESI + (i) - 1)


/* 3-parameter operation */
#define jit_opr_(d, s1, s2, op1d, op2d)					\
	( (s2 == d) ? op1d :						\
	  (  ((s1 == d) ? (void)0 : (void)MOVLrr(s1, d)), op2d )	\
	)

/* 3-parameter operation, with immediate */
#define jit_op_(d, s1, op2d)				\
	((s1 == d) ? op2d : (MOVLrr(s1, d), op2d))

/* 3-parameter operation, optimizable */
#define jit_opo_(d, s1, s2, op1d, op2d, op12d)		\
	((s2 == d) ? op2d : 				\
	((s1 == d) ? op1d : op12d))

/* 3-parameter operation, optimizable, with immediate */
#define jit_opi_(d, rs, opdi, opdri)			\
	((rs == d) ? opdi : opdri)

/* An operand is forced into a register */
#define jit_replace(rd, rs, forced, op)					\
	((rd == forced) ? JITSORRY("Register conflict for " # op) :	\
	 (rs == forced) ? op : (jit_pushr_i(forced), MOVLrr(rs, forced), op, jit_popr_i(forced)))

/* For LT, LE, ... */
#define jit_replace8(d, op)						\
	(jit_check8(d)							\
	  ? (MOVLir(0, d), op(d))					\
	  : (jit_pushr_i(_EAX), MOVLir(0, _EAX), op(_EAX), MOVLrr(_EAX, (d)), jit_popr_i(_EAX)))

#define jit_bool_r(d, s1, s2, op)					\
	(CMPLrr(s2, s1), jit_replace8(d, op))

#define jit_bool_i(d, rs, is, op)					\
	(CMPLir(is, rs), jit_replace8(d, op))

/* When CMP with 0 can be replaced with TEST */
#define jit_bool_i0(d, rs, is, op, op0)					\
	((is) != 0							\
	  ? (CMPLir(is, rs), jit_replace8(d, op)) 			\
	  : (TESTLrr(rs, rs), jit_replace8(d, op0)))

/* For BLT, BLE, ... */
#define jit_bra_r(s1, s2, op)		(CMPLrr(s2, s1), op, _jit.x.pc)
#define jit_bra_i(rs, is, op)		(CMPLir(is, rs), op, _jit.x.pc)

/* When CMP with 0 can be replaced with TEST */
#define jit_bra_i0(rs, is, op, op0)					\
	( (is) == 0 ? (TESTLrr(rs, rs), op0, _jit.x.pc) : (CMPLir(is, rs), op, _jit.x.pc))

/* Reduce arguments of XOR/OR/TEST */
#define jit_reduce_(op)	op
#define jit_reduce(op, is, rs)							\
	(_u8P(is) && jit_check8(rs) ? jit_reduce_(op##Bir(is, jit_reg8(rs))) :	\
	(_u16P(is) && JIT_CAN_16 ? jit_reduce_(op##Wir(is, jit_reg16(rs))) :			\
	jit_reduce_(op##Lir(is, rs)) ))

/* Helper macros for MUL/DIV/IDIV */
#define jit_might(d, s1, op)					\
	((s1 == d) ? 0 : op)

#define jit_mulr_ui_(s1, s2)	jit_opr_(_EAX, s1, s2, MULLr(s1),  MULLr(s2))
#define jit_mulr_i_(s1, s2)	jit_opr_(_EAX, s1, s2, IMULLr(s1), IMULLr(s2))


#define jit_muli_i_(is, rs)				\
	(MOVLir(is, rs == _EAX ? _EDX : _EAX),		\
	 IMULLr(rs == _EAX ? _EDX : rs))

#define jit_muli_ui_(is, rs)				\
	(MOVLir(is, rs == _EAX ? _EDX : _EAX),		\
	 IMULLr(rs == _EAX ? _EDX : rs))

#define jit_divi_i_(result, d, rs, is)			\
	(jit_might (d,    _EAX, jit_pushr_i(_EAX)),		\
	jit_might (d,    _ECX, jit_pushr_i(_ECX)),		\
	jit_might (d,    _EDX, jit_pushr_i(_EDX)),		\
	jit_might (rs,   _EAX, MOVLrr(rs, _EAX)),	\
	jit_might (rs,   _EDX, MOVLrr(rs, _EDX)),	\
	MOVLir(is, _ECX),				\
	SARLir(31, _EDX),				\
	IDIVLr(_ECX),					\
	jit_might(d,    result, MOVLrr(result, d)),	\
	jit_might(d,     _EDX,  jit_popr_i(_EDX)),		\
	jit_might(d,     _ECX,  jit_popr_i(_ECX)),		\
	jit_might(d,     _EAX,  jit_popr_i(_EAX)))

#define jit_divr_i_(result, d, s1, s2)			\
	(jit_might (d,    _EAX, jit_pushr_i(_EAX)),		\
	jit_might (d,    _ECX, jit_pushr_i(_ECX)),		\
	jit_might (d,    _EDX, jit_pushr_i(_EDX)),		\
	((s1 == _ECX) ? jit_pushr_i(_ECX) : 0),		\
	jit_might (s2,   _ECX, MOVLrr(s2, _ECX)),	\
	((s1 == _ECX) ? jit_popr_i(_EDX) :			\
	jit_might (s1,   _EDX, MOVLrr(s1, _EDX))),	\
	MOVLrr(_EDX, _EAX),				\
	SARLir(31, _EDX),				\
	IDIVLr(_ECX),					\
	jit_might(d,    result, MOVLrr(result, d)),	\
	jit_might(d,     _EDX,  jit_popr_i(_EDX)),		\
	jit_might(d,     _ECX,  jit_popr_i(_ECX)),		\
	jit_might(d,     _EAX,  jit_popr_i(_EAX)))

#define jit_divi_ui_(result, d, rs, is)			\
	(jit_might (d,    _EAX, jit_pushr_i(_EAX)),		\
	jit_might (d,    _ECX, jit_pushr_i(_ECX)),		\
	jit_might (d,    _EDX, jit_pushr_i(_EDX)),		\
	jit_might (rs,   _EAX, MOVLrr(rs, _EAX)),	\
	MOVLir(is, _ECX),				\
	XORLrr(_EDX, _EDX),				\
	DIVLr(_ECX),					\
	jit_might(d,    result, MOVLrr(result, d)),	\
	jit_might(d,     _EDX,  jit_popr_i(_EDX)),		\
	jit_might(d,     _ECX,  jit_popr_i(_ECX)),		\
	jit_might(d,     _EAX,  jit_popr_i(_EAX)))

#define jit_divr_ui_(result, d, s1, s2)			\
	(jit_might (d,    _EAX, jit_pushr_i(_EAX)),		\
	jit_might (d,    _ECX, jit_pushr_i(_ECX)),		\
	jit_might (d,    _EDX, jit_pushr_i(_EDX)),		\
	((s1 == _ECX) ? jit_pushr_i(_ECX) : 0),		\
	jit_might (s2,   _ECX, MOVLrr(s2, _ECX)),	\
	((s1 == _ECX) ? jit_popr_i(_EAX) :			\
	jit_might (s1,   _EAX, MOVLrr(s1, _EAX))),	\
	XORLrr(_EDX, _EDX),				\
	DIVLr(_ECX),					\
	jit_might(d,    result, MOVLrr(result, d)),	\
	jit_might(d,     _EDX,  jit_popr_i(_EDX)),		\
	jit_might(d,     _ECX,  jit_popr_i(_ECX)),		\
	jit_might(d,     _EAX,  jit_popr_i(_EAX)))


/* ALU */
#define jit_addi_i(d, rs, is)	jit_opi_((d), (rs),       ADDLir((is), (d)), 			LEALmr((is), (rs), 0, 0, (d))  )
#define jit_addr_i(d, s1, s2)	jit_opo_((d), (s1), (s2), ADDLrr((s2), (d)), ADDLrr((s1), (d)), LEALmr(0, (s1), (s2), 1, (d))  )
#define jit_addci_i(d, rs, is)	jit_op_ ((d), (rs),       ADDLir((is), (d)) 		       )
#define jit_addcr_i(d, s1, s2)	jit_opr_((d), (s1), (s2), ADDLrr((s1), (d)), ADDLrr((s2), (d)) )
#define jit_addxi_i(d, rs, is)	jit_op_ ((d), (rs),       ADCLir((is), (d)) 		       )
#define jit_addxr_i(d, s1, s2)	jit_opr_((d), (s1), (s2), ADCLrr((s1), (d)), ADCLrr((s2), (d)) )
#define jit_andi_i(d, rs, is)	jit_op_ ((d), (rs),       ANDLir((is), (d)) 		       )
#define jit_andr_i(d, s1, s2)	jit_opr_((d), (s1), (s2), ANDLrr((s1), (d)), ANDLrr((s2), (d)) )
#define jit_orr_i(d, s1, s2)	jit_opr_((d), (s1), (s2),  ORLrr((s1), (d)),  ORLrr((s2), (d)) )
#define jit_subr_i(d, s1, s2)	jit_opr_((d), (s1), (s2), (SUBLrr((s1), (d)), NEGLr(d)),	SUBLrr((s2), (d))	       )
#define jit_subcr_i(d, s1, s2)	jit_subr_i((d), (s1), (s2))
#define jit_subxr_i(d, s1, s2)	jit_opr_((d), (s1), (s2), SBBLrr((s1), (d)), SBBLrr((s2), (d)) )
#define jit_subxi_i(d, rs, is)	jit_op_ ((d), (rs),       SBBLir((is), (d)) 		       )
#define jit_xorr_i(d, s1, s2)	jit_opr_((d), (s1), (s2), XORLrr((s1), (d)), XORLrr((s2), (d)) )


/* These can sometimes use byte or word versions! */
#define jit_ori_i(d, rs, is)	jit_op_ ((d), (rs),        jit_reduce(OR, (is), (d))	       )
#define jit_xori_i(d, rs, is)	jit_op_ ((d), (rs),        jit_reduce(XOR, (is), (d))	       )

#define jit_muli_i(d, rs, is)	jit_op_ ((d), (rs),       IMULLir((is), (d)) 		       )
#define jit_mulr_i(d, s1, s2)	jit_opr_((d), (s1), (s2), IMULLrr((s1), (d)), IMULLrr((s2), (d)) )

/* As far as low bits are concerned, signed and unsigned multiplies are
   exactly the same. */
#define jit_muli_ui(d, rs, is)	jit_op_ ((d), (rs),       IMULLir((is), (d)) 		       )
#define jit_mulr_ui(d, s1, s2)	jit_opr_((d), (s1), (s2), IMULLrr((s1), (d)), IMULLrr((s2), (d)) )

#define jit_hmuli_i(d, rs, is)														\
	((d) == _EDX ? (	      jit_pushr_i(_EAX), jit_muli_i_((is), (rs)), 				     jit_popr_i(_EAX)		) :	\
	((d) == _EAX ? (jit_pushr_i(_EDX),		    jit_muli_i_((is), (rs)), MOVLrr(_EDX, _EAX),	     jit_popr_i(_EDX) ) :	\
	               (jit_pushr_i(_EDX), jit_pushr_i(_EAX), jit_muli_i_((is), (rs)), MOVLrr(_EDX, (d)), jit_popr_i(_EAX), jit_popr_i(_EDX) )))

#define jit_hmulr_i(d, s1, s2)													\
	((d) == _EDX ? (	      jit_pushr_i(_EAX), jit_mulr_i_((s1), (s2)), 			  jit_popr_i(_EAX)		    ) :	\
	((d) == _EAX ? (jit_pushr_i(_EDX),		    jit_mulr_i_((s1), (s2)), MOVLrr(_EDX, _EAX), 	       jit_popr_i(_EDX)  ) :	\
	 	       (jit_pushr_i(_EDX), jit_pushr_i(_EAX), jit_mulr_i_((s1), (s2)), MOVLrr(_EDX, (d)),   jit_popr_i(_EAX), jit_popr_i(_EDX)  )))

#define jit_hmuli_ui(d, rs, is)														\
	((d) == _EDX ? (	      jit_pushr_i(_EAX), jit_muli_ui_((is), (rs)), 				      jit_popr_i(_EAX)		) :	\
	((d) == _EAX ? (jit_pushr_i(_EDX),		    jit_muli_ui_((is), (rs)), MOVLrr(_EDX, _EAX),	      jit_popr_i(_EDX) ) :	\
	               (jit_pushr_i(_EDX), jit_pushr_i(_EAX), jit_muli_ui_((is), (rs)), MOVLrr(_EDX, (d)), jit_popr_i(_EAX), jit_popr_i(_EDX) )))

#define jit_hmulr_ui(d, s1, s2)													\
	((d) == _EDX ? (	      jit_pushr_i(_EAX), jit_mulr_ui_((s1), (s2)), 			  jit_popr_i(_EAX)		    ) :	\
	((d) == _EAX ? (jit_pushr_i(_EDX),		    jit_mulr_ui_((s1), (s2)), MOVLrr(_EDX, _EAX), 	       jit_popr_i(_EDX)  ) :	\
	 	       (jit_pushr_i(_EDX), jit_pushr_i(_EAX), jit_mulr_ui_((s1), (s2)), MOVLrr(_EDX, (d)),  jit_popr_i(_EAX), jit_popr_i(_EDX)  )))

#define jit_divi_i(d, rs, is)	jit_divi_i_(_EAX, (d), (rs), (is))
#define jit_divi_ui(d, rs, is)	jit_divi_ui_(_EAX, (d), (rs), (is))
#define jit_modi_i(d, rs, is)	jit_divi_i_(_EDX, (d), (rs), (is))
#define jit_modi_ui(d, rs, is)	jit_divi_ui_(_EDX, (d), (rs), (is))
#define jit_divr_i(d, s1, s2)	jit_divr_i_(_EAX, (d), (s1), (s2))
#define jit_divr_ui(d, s1, s2)	jit_divr_ui_(_EAX, (d), (s1), (s2))
#define jit_modr_i(d, s1, s2)	jit_divr_i_(_EDX, (d), (s1), (s2))
#define jit_modr_ui(d, s1, s2)	jit_divr_ui_(_EDX, (d), (s1), (s2))


/* Shifts */
#define jit_lshi_i(d, rs, is)	((is) <= 3 ?   LEALmr(0, 0, (rs), 1 << (is), (d))   :   jit_op_ ((d), (rs), SHLLir((is), (d)) ))
#define jit_rshi_i(d, rs, is)								jit_op_ ((d), (rs), SARLir((is), (d))  )
#define jit_rshi_ui(d, rs, is)								jit_op_ ((d), (rs), SHRLir((is), (d))  )
#define jit_lshr_i(d, r1, r2)	jit_replace((r1), (r2), _ECX, 				jit_op_ ((d), (r1), SHLLrr(_CL,  (d)) ))
#define jit_rshr_i(d, r1, r2)	jit_replace((r1), (r2), _ECX, 				jit_op_ ((d), (r1), SARLrr(_CL,  (d)) ))
#define jit_rshr_ui(d, r1, r2)	jit_replace((r1), (r2), _ECX, 				jit_op_ ((d), (r1), SHRLrr(_CL,  (d)) ))

/* Stack */
#define jit_retval_i(rd)	((void)jit_movr_i ((rd), _EAX))

/* Unary */
#define jit_negr_i(d, rs)	jit_opi_((d), (rs), NEGLr(d), (XORLrr((d), (d)), SUBLrr((rs), (d))) )

#define jit_movr_i(d, rs)	((void)((rs) == (d) ? 0 : MOVLrr((rs), (d))))
#define jit_movi_i(d, is)	((is) ? MOVLir((is), (d)) : XORLrr ((d), (d)) )
#define jit_patch_movi(pa,pv)   (*_PSL((pa) - sizeof(long)) = _jit_SL((pv)))

#define jit_ntoh_ui(d, rs)	jit_op_((d), (rs), BSWAPLr(d))
#define jit_ntoh_us(d, rs)	jit_op_((d), (rs), RORWir(8, d))

/* Boolean */
#define jit_ltr_i(d, s1, s2)	jit_bool_r((d), (s1), (s2), SETLr  )
#define jit_ler_i(d, s1, s2)	jit_bool_r((d), (s1), (s2), SETLEr )
#define jit_gtr_i(d, s1, s2)	jit_bool_r((d), (s1), (s2), SETGr  )
#define jit_ger_i(d, s1, s2)	jit_bool_r((d), (s1), (s2), SETGEr )
#define jit_eqr_i(d, s1, s2)	jit_bool_r((d), (s1), (s2), SETEr  )
#define jit_ner_i(d, s1, s2)	jit_bool_r((d), (s1), (s2), SETNEr )
#define jit_ltr_ui(d, s1, s2)	jit_bool_r((d), (s1), (s2), SETBr  )
#define jit_ler_ui(d, s1, s2)	jit_bool_r((d), (s1), (s2), SETBEr )
#define jit_gtr_ui(d, s1, s2)	jit_bool_r((d), (s1), (s2), SETAr  )
#define jit_ger_ui(d, s1, s2)	jit_bool_r((d), (s1), (s2), SETAEr )

#define jit_lti_i(d, rs, is)	jit_bool_i0((d), (rs), (is), SETLr,  SETSr  )
#define jit_lei_i(d, rs, is)	jit_bool_i ((d), (rs), (is), SETLEr	    )
#define jit_gti_i(d, rs, is)	jit_bool_i ((d), (rs), (is), SETGr 	    )
#define jit_gei_i(d, rs, is)	jit_bool_i0((d), (rs), (is), SETGEr, SETNSr )
#define jit_eqi_i(d, rs, is)	jit_bool_i0((d), (rs), (is), SETEr,  SETEr  )
#define jit_nei_i(d, rs, is)	jit_bool_i0((d), (rs), (is), SETNEr, SETNEr )
#define jit_lti_ui(d, rs, is)	jit_bool_i ((d), (rs), (is), SETBr	    )
#define jit_lei_ui(d, rs, is)	jit_bool_i0((d), (rs), (is), SETBEr, SETEr  )
#define jit_gti_ui(d, rs, is)	jit_bool_i0((d), (rs), (is), SETAr,  SETNEr )
#define jit_gei_ui(d, rs, is)	jit_bool_i0((d), (rs), (is), SETAEr, INCLr  )

/* Jump */
#define jit_bltr_i(label, s1, s2)	jit_bra_r((s1), (s2), JLm(label) )
#define jit_bler_i(label, s1, s2)	jit_bra_r((s1), (s2), JLEm(label) )
#define jit_bgtr_i(label, s1, s2)	jit_bra_r((s1), (s2), JGm(label) )
#define jit_bger_i(label, s1, s2)	jit_bra_r((s1), (s2), JGEm(label) )
#define jit_beqr_i(label, s1, s2)	jit_bra_r((s1), (s2), JEm(label) )
#define jit_bner_i(label, s1, s2)	jit_bra_r((s1), (s2), JNEm(label) )
#define jit_bltr_ui(label, s1, s2)	jit_bra_r((s1), (s2), JBm(label) )
#define jit_bler_ui(label, s1, s2)	jit_bra_r((s1), (s2), JBEm(label) )
#define jit_bgtr_ui(label, s1, s2)	jit_bra_r((s1), (s2), JAm(label) )
#define jit_bger_ui(label, s1, s2)	jit_bra_r((s1), (s2), JAEm(label) )
#define jit_bmsr_i(label, s1, s2)	(TESTLrr((s1), (s2)), JNZm(label), _jit.x.pc)
#define jit_bmcr_i(label, s1, s2)	(TESTLrr((s1), (s2)), JZm(label),  _jit.x.pc)
#define jit_boaddr_i(label, s1, s2)	(ADDLrr((s2), (s1)), JOm(label), _jit.x.pc)
#define jit_bosubr_i(label, s1, s2)	(SUBLrr((s2), (s1)), JOm(label), _jit.x.pc)
#define jit_boaddr_ui(label, s1, s2)	(ADDLrr((s2), (s1)), JCm(label), _jit.x.pc)
#define jit_bosubr_ui(label, s1, s2)	(SUBLrr((s2), (s1)), JCm(label), _jit.x.pc)

#define jit_blti_i(label, rs, is)	jit_bra_i0((rs), (is), JLm(label), JSm(label) )
#define jit_blei_i(label, rs, is)	jit_bra_i ((rs), (is), JLEm(label)		    )
#define jit_bgti_i(label, rs, is)	jit_bra_i ((rs), (is), JGm(label)		    )
#define jit_bgei_i(label, rs, is)	jit_bra_i0((rs), (is), JGEm(label), JNSm(label) )
#define jit_beqi_i(label, rs, is)	jit_bra_i0((rs), (is), JEm(label), JEm(label) )
#define jit_bnei_i(label, rs, is)	jit_bra_i0((rs), (is), JNEm(label), JNEm(label) )
#define jit_blti_ui(label, rs, is)	jit_bra_i ((rs), (is), JBm(label)		    )
#define jit_blei_ui(label, rs, is)	jit_bra_i0((rs), (is), JBEm(label), JEm(label) )
#define jit_bgti_ui(label, rs, is)	jit_bra_i0((rs), (is), JAm(label), JNEm(label) )
#define jit_bgei_ui(label, rs, is)	jit_bra_i ((rs), (is), JAEm(label)		    )
#define jit_boaddi_i(label, rs, is)	(ADDLir((is), (rs)), JOm(label), _jit.x.pc)
#define jit_bosubi_i(label, rs, is)	(SUBLir((is), (rs)), JOm(label), _jit.x.pc)
#define jit_boaddi_ui(label, rs, is)	(ADDLir((is), (rs)), JCm(label), _jit.x.pc)
#define jit_bosubi_ui(label, rs, is)	(SUBLir((is), (rs)), JCm(label), _jit.x.pc)

#define jit_bmsi_i(label, rs, is)	(jit_reduce(TEST, (is), (rs)), JNZm(label), _jit.x.pc)
#define jit_bmci_i(label, rs, is)	(jit_reduce(TEST, (is), (rs)), JZm(label),  _jit.x.pc)

#define jit_jmpi(label)			(JMPm( ((unsigned long) (label))), _jit.x.pc)
#define jit_calli(label)		(CALLm( ((unsigned long) (label))), _jit.x.pc)
#define jit_callr(reg)			CALLsr(reg)
#define jit_jmpr(reg)			JMPsr(reg)

/* Memory */
#define jit_ldr_c(d, rs)                MOVSBLmr(0,    (rs), 0,    0, (d))
#define jit_ldxr_c(d, s1, s2)           MOVSBLmr(0,    (s1), (s2), 1, (d))
							    
#define jit_ldr_uc(d, rs)               MOVZBLmr(0,    (rs), 0,    0, (d))
#define jit_ldxr_uc(d, s1, s2)          MOVZBLmr(0,    (s1), (s2), 1, (d))
							    
#define jit_str_c(rd, rs)               jit_movbrm((rs), 0,    (rd), 0,    0)
#define jit_stxr_c(d1, d2, rs)          jit_movbrm((rs), 0,    (d1), (d2), 1)
							    
#define jit_ldr_s(d, rs)                MOVSWLmr(0,    (rs), 0,    0, (d))
#define jit_ldxr_s(d, s1, s2)           MOVSWLmr(0,    (s1), (s2), 1, (d))
							    
#define jit_ldr_us(d, rs)               MOVZWLmr(0,    (rs), 0,    0,  (d))
#define jit_ldxr_us(d, s1, s2)          MOVZWLmr(0,    (s1), (s2), 1,  (d))
							    
#define jit_str_s(rd, rs)               MOVWrm(jit_reg16(rs), 0,    (rd), 0,    0)
#define jit_stxr_s(d1, d2, rs)          MOVWrm(jit_reg16(rs), 0,    (d1), (d2), 1)
							    
#define jit_ldr_i(d, rs)                MOVLmr(0,    (rs), 0,    0,  (d))
#define jit_ldxr_i(d, s1, s2)           MOVLmr(0,    (s1), (s2), 1,  (d))
							    
#define jit_str_i(rd, rs)               MOVLrm((rs), 0,    (rd), 0,    0)
#define jit_stxr_i(d1, d2, rs)          MOVLrm((rs), 0,    (d1), (d2), 1)
							    
/* Extra */
#define jit_nop()			NOP_()

#define _jit_alignment(pc, n)		(((pc ^ _MASK(4)) + 1) & _MASK(n))
#define jit_align(n) 			NOPi(_jit_alignment(_jit_UL(_jit.x.pc), (n)))

#endif /* __lightning_core_i386_h */

