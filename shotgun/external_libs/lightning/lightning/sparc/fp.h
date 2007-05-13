/******************************** -*- C -*- ****************************
 *
 *	Run-time assembler & support macros for the SPARC math unit
 *
 ***********************************************************************/


/***********************************************************************
 *
 * Copyright 2000, 2001, 2002, 2004 Free Software Foundation, Inc.
 * Written by Paolo Bonzini.
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




#ifndef __lightning_asm_fp_h
#define __lightning_asm_fp_h

#include <float.h>

#define JIT_FPR_NUM	6
#define JIT_FPRET	0
#define JIT_FPR(i)	(30-(i)*2)
#define JIT_FPTMP	18

#define jit_addr_f(rd,s1,s2)	FADDSrrr((s1), (s2), (rd))
#define jit_subr_f(rd,s1,s2)	FSUBSrrr((s1), (s2), (rd))
#define jit_mulr_f(rd,s1,s2)	FMULSrrr((s1), (s2), (rd))
#define jit_divr_f(rd,s1,s2)	FDIVSrrr((s1), (s2), (rd))

#define jit_addr_d(rd,s1,s2)	FADDDrrr((s1), (s2), (rd))
#define jit_subr_d(rd,s1,s2)	FSUBDrrr((s1), (s2), (rd))
#define jit_mulr_d(rd,s1,s2)	FMULDrrr((s1), (s2), (rd))
#define jit_divr_d(rd,s1,s2)	FDIVDrrr((s1), (s2), (rd))

#define jit_movr_f(rd,rs)	FMOVSrr((rs), (rd))
#define jit_abs_d(rd,rs)	FABSSrr((rs), (rd))
#define jit_negr_d(rd,rs)	FNEGSrr((rs), (rd))
#define jit_sqrt_d(rd,rs)	FSQRTSrr((rs), (rd))
#define jit_movr_d(rd,rs)	FMOVDrr((rs), (rd))
#define jit_abs_f(rd,rs)	FABSDrr((rs), (rd))
#define jit_negr_f(rd,rs)	FNEGDrr((rs), (rd))
#define jit_sqrt_f(rd,rs)	FSQRTDrr((rs), (rd))
#define jit_extr_f_d(rd, rs)	FSTODrr((rs), (rd))
#define jit_extr_d_f(rd, rs)	FDTOSrr((rs), (rd))

#define jit_movi_f(rd,immf)			  \
    do {					  \
      float _v = (immf);                          \
      _1(_jit.x.pc + 3), LDFmr(_Ro(7), 8, (rd));  \
      memcpy(_jit.x.uc_pc, &_v, sizeof (float));  \
      _jit.x.uc_pc += sizeof (float);             \
    } while(0)

#define jit_movi_d(rd,immd)			 	\
    do {					 	\
      double _v = (immd);                        	\
      if ((long)_jit.x.pc & 4) NOP();			\
      _1(_jit.x.pc + 4);				\
      LDDFmr(_Ro(7), 8, (rd));				\
      memcpy(_jit.x.uc_pc, &_v, sizeof (double));	\
      _jit.x.uc_pc += sizeof (double);           	\
    } while(0)


#define jit_ldxi_f(rd, rs, is)		jit_chk_imm((is), LDFmr((rs), (is), (rd)), LDFxr((rs), JIT_BIG, (rd)))
#define jit_ldxi_d(rd, rs, is)		jit_chk_imm((is), LDDFmr((rs), (is), (rd)), LDDFxr((rs), JIT_BIG, (rd)))
#define jit_ldxr_f(rd, s1, s2)		LDFxr((s1), (s2), (rd))
#define jit_ldxr_d(rd, s1, s2)		LDDFxr((s1), (s2), (rd))
#define jit_stxi_f(id, rd, rs)		jit_chk_imm((id), STFrm((rs), (rd), (id)), STFrx((rs), (rd),  JIT_BIG))
#define jit_stxi_d(id, rd, rs)		jit_chk_imm((id), STDFrm((rs), (rd), (id)), STDFrx((rs), (rd),  JIT_BIG))
#define jit_stxr_f(d1, d2, rs)		STFrx((rs), (d1), (d2))
#define jit_stxr_d(d1, d2, rs)		STDFrx((rs), (d1), (d2))

#define jit_truncr_f_i(rd, rs) (		\
	_1(_jit.x.pc + 3),			\
	FSTOIrr((rs), JIT_FPTMP),		\
	NOP(),					\
	STFrm(JIT_FPTMP, _Ro(7), 8),		\
	LDmr(_Ro(7), 8, (rd)))

#define jit_truncr_d_i(rd, rs) (		\
	_1(_jit.x.pc + 3),			\
	FDTOIrr((rs), JIT_FPTMP),		\
	NOP(),					\
	STFrm(JIT_FPTMP, _Ro(7), 8),		\
	LDmr(_Ro(7), 8, (rd)))

#define jit_extr_i_d(rd, rs)		(_1 (_jit.x.pc + 3), NOP(), NOP(), STrm((rs), _Ro(7), 8), LDFmr(_Ro(7), 8, (rd)), FITODrr((rd), (rd)))
#define jit_extr_i_f(rd, rs)		(_1 (_jit.x.pc + 3), NOP(), NOP(), STrm((rs), _Ro(7), 8), LDFmr(_Ro(7), 8, (rd)), FITOSrr((rd), (rd)))

#define jit_do_round_f(rd, rs, fixup, mode) do {		\
	jit_movi_f (JIT_FPTMP, fixup);				\
        _1(_jit.x.pc + 4);                                      \
        SETHIir(_HI(mode << 29), JIT_BIG);                      \
        NOP();                                                  \
        NOP();                                                  \
        STFSRm(_Ro(7), 8);              /* store fsr */         \
        LDmr(_Ro(7), 8, rd);                                    \
        XORrrr(rd, JIT_BIG, JIT_BIG);   /* adjust mode */       \
        STrm(JIT_BIG, _Ro(7), 12);                              \
        LDFSRm(_Ro(7), 12);              /* load fsr */         \
	FADDSrrr ((rs), JIT_FPTMP, JIT_FPTMP);			\
        LDFSRm(_Ro(7), 8);                                      \
        FSTOIrr(JIT_FPTMP, JIT_FPTMP);   		        \
        STFrm(JIT_FPTMP, _Ro(7), 8);			        \
        LDmr(_Ro(7), 8, (rd));					\
	ADDCCrrr ((rd), (rd), 0);				\
	SUBXrrr ((rd), 0, (rd));				\
  } while (0);

#define jit_do_round_d(rd, rs, fixup, mode) do {		\
	jit_movi_d (JIT_FPTMP, fixup);				\
        _1(_jit.x.pc + 4);                                      \
        SETHIir(_HI(mode << 29), JIT_BIG);                      \
        NOP();                                                  \
        NOP();                                                  \
        STFSRm(_Ro(7), 8);              /* store fsr */         \
        LDmr(_Ro(7), 8, rd);                                    \
        XORrrr(rd, JIT_BIG, JIT_BIG);   /* adjust mode */       \
        STrm(JIT_BIG, _Ro(7), 12);                              \
        LDFSRm(_Ro(7), 12);              /* load fsr */         \
	FADDDrrr ((rs), JIT_FPTMP, JIT_FPTMP);			\
        LDFSRm(_Ro(7), 8);                                      \
        FDTOIrr(JIT_FPTMP, JIT_FPTMP);   		        \
        STFrm(JIT_FPTMP, _Ro(7), 8);			        \
        LDmr(_Ro(7), 8, (rd));					\
	ADDCCrrr ((rd), (rd), 0);				\
	SUBXrrr ((rd), 0, (rd));				\
  } while (0);

#define jit_roundr_f_i(rd, rs) do {			\
	jit_movi_f (JIT_FPTMP, 0.5);			\
	FADDSrrr ((rs), JIT_FPTMP, JIT_FPTMP);		\
	jit_truncr_f_i ((rd), JIT_FPTMP);		\
	ADDCCrrr ((rd), (rd), 0);			\
	SUBXrrr ((rd), 0, (rd));			\
  } while (0)

#define jit_roundr_d_i(rd, rs) do {			\
	jit_movi_d (JIT_FPTMP, 0.5);			\
	FADDDrrr ((rs), JIT_FPTMP, JIT_FPTMP);		\
	jit_truncr_d_i ((rd), JIT_FPTMP);		\
	ADDCCrrr ((rd), (rd), 0);			\
	SUBXrrr ((rd), 0, (rd));			\
  } while (0)

#define jit_ceilr_f_i(rd, rs) 				\
	jit_do_round_f ((rd), (rs), 1.0f - FLT_EPSILON, 3)

#define jit_ceilr_d_i(rd, rs) 				\
	jit_do_round_d ((rd), (rs), 1.0 - DBL_EPSILON, 3)

#define jit_floorr_f_i(rd, rs) 				\
	jit_do_round_f ((rd), (rs), FLT_EPSILON, 2)

#define jit_floorr_d_i(rd, rs) 				\
	jit_do_round_d ((rd), (rs), DBL_EPSILON, 2)

#define jit_ltr_d(d, s1, s2)            (FCMPDrr ((s1), (s2)), FBLi(_jit.x.pc + 3), MOVir (1, (d)), MOVir (0, (d)))
#define jit_ltr_f(d, s1, s2)            (FCMPSrr ((s1), (s2)), FBLi(_jit.x.pc + 3), MOVir (1, (d)), MOVir (0, (d)))
#define jit_ler_d(d, s1, s2)            (FCMPDrr ((s1), (s2)), FBLEi(_jit.x.pc + 3), MOVir (1, (d)), MOVir (0, (d)))
#define jit_ler_f(d, s1, s2)            (FCMPSrr ((s1), (s2)), FBLEi(_jit.x.pc + 3), MOVir (1, (d)), MOVir (0, (d)))
#define jit_eqr_d(d, s1, s2)            (FCMPDrr ((s1), (s2)), FBEi(_jit.x.pc + 3), MOVir (1, (d)), MOVir (0, (d)))
#define jit_eqr_f(d, s1, s2)            (FCMPSrr ((s1), (s2)), FBEi(_jit.x.pc + 3), MOVir (1, (d)), MOVir (0, (d)))
#define jit_ner_d(d, s1, s2)            (FCMPDrr ((s1), (s2)), FBNEi(_jit.x.pc + 3), MOVir (1, (d)), MOVir (0, (d)))
#define jit_ner_f(d, s1, s2)            (FCMPSrr ((s1), (s2)), FBNEi(_jit.x.pc + 3), MOVir (1, (d)), MOVir (0, (d)))
#define jit_ger_d(d, s1, s2)            (FCMPDrr ((s1), (s2)), FBGEi(_jit.x.pc + 3), MOVir (1, (d)), MOVir (0, (d)))
#define jit_ger_f(d, s1, s2)            (FCMPSrr ((s1), (s2)), FBGEi(_jit.x.pc + 3), MOVir (1, (d)), MOVir (0, (d)))
#define jit_gtr_d(d, s1, s2)            (FCMPDrr ((s1), (s2)), FBGi(_jit.x.pc + 3), MOVir (1, (d)), MOVir (0, (d)))
#define jit_gtr_f(d, s1, s2)            (FCMPSrr ((s1), (s2)), FBGi(_jit.x.pc + 3), MOVir (1, (d)), MOVir (0, (d)))
#define jit_unltr_d(d, s1, s2)          (FCMPDrr ((s1), (s2)), FBULi(_jit.x.pc + 3), MOVir (1, (d)), MOVir (0, (d)))
#define jit_unltr_f(d, s1, s2)          (FCMPSrr ((s1), (s2)), FBULi(_jit.x.pc + 3), MOVir (1, (d)), MOVir (0, (d)))
#define jit_unler_d(d, s1, s2)          (FCMPDrr ((s1), (s2)), FBULEi(_jit.x.pc + 3), MOVir (1, (d)), MOVir (0, (d)))
#define jit_unler_f(d, s1, s2)          (FCMPSrr ((s1), (s2)), FBULEi(_jit.x.pc + 3), MOVir (1, (d)), MOVir (0, (d)))
#define jit_uneqr_d(d, s1, s2)          (FCMPDrr ((s1), (s2)), FBUEi(_jit.x.pc + 3), MOVir (1, (d)), MOVir (0, (d)))
#define jit_uneqr_f(d, s1, s2)          (FCMPSrr ((s1), (s2)), FBUEi(_jit.x.pc + 3), MOVir (1, (d)), MOVir (0, (d)))
#define jit_ltgtr_d(d, s1, s2)          (FCMPDrr ((s1), (s2)), FBLGi(_jit.x.pc + 3), MOVir (1, (d)), MOVir (0, (d)))
#define jit_ltgtr_f(d, s1, s2)          (FCMPSrr ((s1), (s2)), FBLGi(_jit.x.pc + 3), MOVir (1, (d)), MOVir (0, (d)))
#define jit_unger_d(d, s1, s2)          (FCMPDrr ((s1), (s2)), FBUGEi(_jit.x.pc + 3), MOVir (1, (d)), MOVir (0, (d)))
#define jit_unger_f(d, s1, s2)          (FCMPSrr ((s1), (s2)), FBUGEi(_jit.x.pc + 3), MOVir (1, (d)), MOVir (0, (d)))
#define jit_ungtr_d(d, s1, s2)          (FCMPDrr ((s1), (s2)), FBUGi(_jit.x.pc + 3), MOVir (1, (d)), MOVir (0, (d)))
#define jit_ungtr_f(d, s1, s2)          (FCMPSrr ((s1), (s2)), FBUGi(_jit.x.pc + 3), MOVir (1, (d)), MOVir (0, (d)))
#define jit_ordr_d(d, s1, s2)           (FCMPDrr ((s1), (s2)), FBOi(_jit.x.pc + 3), MOVir (1, (d)), MOVir (0, (d)))
#define jit_ordr_f(d, s1, s2)           (FCMPSrr ((s1), (s2)), FBOi(_jit.x.pc + 3), MOVir (1, (d)), MOVir (0, (d)))
#define jit_unordr_d(d, s1, s2)         (FCMPDrr ((s1), (s2)), FBUi(_jit.x.pc + 3), MOVir (1, (d)), MOVir (0, (d)))
#define jit_unordr_f(d, s1, s2)         (FCMPSrr ((s1), (s2)), FBUi(_jit.x.pc + 3), MOVir (1, (d)), MOVir (0, (d)))

#define jit_prepare_f(num)              (_jitl.nextarg_put += (num))
#define jit_prepare_d(num)              (_jitl.nextarg_put += 2 * (num))

#define jit_arg_f()                     (_jitl.nextarg_get++)
#define jit_arg_d()                     (_jitl.nextarg_get += _jitl.nextarg_get & 1, _jitl.nextarg_get += 2, _jitl.nextarg_get - 2)

#define jit_getarg_f(rd, ofs)           (STrm(ofs, _Ri(6), -24), LDFmr (_Ri(6), -24, (rd)))
#define jit_getarg_d(rd, ofs)           (STDrm(ofs, _Ri(6), -24), LDDFmr (_Ri(6), -24, (rd)))

#define jit_pusharg_f(rs)               (STFrm((rs), _Ri(6), -24), --_jitl.nextarg_put, LDmr (_Ri(6), -24, _Ro(_jitl.nextarg_put)))
#define jit_pusharg_d(rs)               (STDFrm((rs), _Ri(6), -24), _jitl.nextarg_put -= 2, LDmr (_Ri(6), -24, _Ro(_jitl.nextarg_put)))

#endif /* __lightning_asm_fp_h */
