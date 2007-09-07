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



#ifndef __lightning_core_h
#define __lightning_core_h

#define JIT_CAN_16 1
#define JIT_AP			_EBP

#include "core-i386.h"

struct jit_local_state {
  int	framesize;
  int	argssize;
  int	alloca_offset;
  int alloca_space;
  int	alloca_slack;
  int unbound_stack;
  int args_clamped;
  int arg_padding;
};

/* The number of bytes past EBP where the alloca's start. */
#define EBP_OFFSET 12

// #define jit_base_prolog() (PUSHLr(_EBX), PUSHLr(_EDI), PUSHLr(_EBP), MOVLrr(_ESP, _EBP))
#define jit_base_prolog() (PUSHLr(_EBP), MOVLrr(_ESP, _EBP), PUSHLr(_EDI), PUSHLr(_EBX), PUSHLr(_ESI), SUBLir(12, _ESP))
//#define jit_base_prolog() (PUSHLr(_EBP), MOVLrr(_ESP, _EBP), PUSHLr(_EBX))
#define jit_prolog(n) (_jitl.args_clamped = 0, _jitl.arg_padding = 0, _jitl.framesize = 8, _jitl.alloca_offset = EBP_OFFSET, _jitl.alloca_space = _jitl.alloca_slack = 12, _jitl.unbound_stack = 0, _jitl.argssize = 0, jit_base_prolog())


/* Used internally.  SLACK is used by the Darwin ABI which keeps the stack
   aligned to 16-bytes.  */

#define jit_allocai_internal(amount, slack)				  \
  (((amount) < _jitl.alloca_slack					  \
    ? 0									  \
    : (_jitl.alloca_slack += (amount) + (slack),			  \
       ((amount) + (slack) == sizeof (int)				  \
        ? PUSHLr(_EAX)							  \
        : SUBLir((amount) + (slack), _ESP)))),				  \
   _jitl.alloca_slack -= (amount),					  \
   _jitl.alloca_offset -= (amount))
   
/* Stack */
#define jit_pushr_i(rs)		(PUSHLr(rs), _jitl.unbound_stack++)
#define jit_popr_i(rs)		(_jitl.unbound_stack--, POPLr(rs))

/* The += in argssize allows for stack pollution */

#define jit_prepare_i(ni)	(_jitl.argssize += (ni))

#ifdef __APPLE__
/* Stack must stay 16-byte aligned: */

# define jit_prepare_clamp()	({ \
  int _ni = _jitl.argssize; \
  int _sb = _ni + _jitl.unbound_stack; \
  int _next4 = (_sb + 3) & ~3; \
  int _padding; \
  _jitl.args_clamped++; \
  if(_sb != _next4 ) { \
    _padding = 4 * (_next4 - _sb); \
    SUBLir(_padding, JIT_SP); \
  } \
  _jitl.arg_padding = 4 * _next4;\
})

  // _jitl.argssize += ((_ni + 3) & ~(0x3)); 

/*
# define jit_prepare_i(ni)	(((ni & 0x3) \
                                  ? SUBLir(4 * ((((ni) + 3) & ~(0x3)) - (ni)), JIT_SP) \
                                  : (void)0), \
                                 _jitl.argssize += (((ni) + 3) & ~(0x3)))
*/
//#define jit_allocai(n)						
//  jit_allocai_internal ((n), (_jitl.alloca_slack - (n)) & 15)

#define jit_allocai(n) ({ \
    int _pos; \
    int _n = (n); \
    int _x = (_n + 3) & ~3;\
    _pos = _jitl.alloca_offset; \
    _jitl.alloca_offset += _x; \
    if(_x <= _jitl.alloca_slack) { \
      _jitl.alloca_slack -= _x; \
    } else { \
      _x = (_n + 15) & ~15; \
      _jitl.alloca_space += _x; \
      _jitl.alloca_slack = _x; \
      SUBLir(_x, _ESP); \
    }; _pos; \
})

#else
#define jit_prepare_clamp() (_jitl.args_clamped++)
#define jit_allocai(n)						\
  jit_allocai_internal ((n), 0)
#endif

#define jit_pusharg_i(rs)	({ if(!_jitl.args_clamped) jit_prepare_clamp(); PUSHLr(rs); })
#define jit_finish(sub)        ((void)jit_calli((sub)), ADDLir( _jitl.arg_padding, JIT_SP), _jitl.argssize = 0, _jitl.args_clamped = 0, _jitl.arg_padding = 0)
#define jit_finishr(reg)	(jit_callr((reg)), ADDLir(_jitl.arg_padding, JIT_SP), _jitl.argssize = 0)

#define	jit_arg_c()		((_jitl.framesize += sizeof(int)) - sizeof(int))
#define	jit_arg_uc()		((_jitl.framesize += sizeof(int)) - sizeof(int))
#define	jit_arg_s()		((_jitl.framesize += sizeof(int)) - sizeof(int))
#define	jit_arg_us()		((_jitl.framesize += sizeof(int)) - sizeof(int))
#define	jit_arg_i()		((_jitl.framesize += sizeof(int)) - sizeof(int))
#define	jit_arg_ui()		((_jitl.framesize += sizeof(int)) - sizeof(int))
#define	jit_arg_l()		((_jitl.framesize += sizeof(long)) - sizeof(long))
#define	jit_arg_ul()		((_jitl.framesize += sizeof(long)) - sizeof(long))
#define	jit_arg_p()		((_jitl.framesize += sizeof(long)) - sizeof(long))

#define jit_movi_p(d, is)       (jit_movi_l(d, ((long)(is))), _jit.x.pc)
#define jit_patch_long_at(jump_pc,v)  (*_PSL((jump_pc) - sizeof(long)) = _jit_SL((jit_insn *)(v) - (jump_pc)))
#define jit_patch_at(jump_pc,v)  jit_patch_long_at(jump_pc, v)
// #define jit_ret()		((_jitl.alloca_offset < 0 ? LEAVE_() : ADDLir(12, _ESP), POPLr(_ESI), POPLr(_EBX)), POPLr(_EDI), POPLr(_EBP), RET_())
#define jit_ret()		(ADDLir(_jitl.alloca_space, _ESP), POPLr(_ESI), POPLr(_EBX), POPLr(_EDI), POPLr(_EBP), RET_())
// #define jit_ret()		(POPLr(_EBX), POPLr(_EBP), RET_())

/* Memory */

#define jit_check8(rs)          ( (rs) <= _EBX )
#define jit_reg8(rs)            ( ((rs) == _SI || (rs) == _DI) ? _AL : (_rN(rs) | _AL ))
#define jit_reg16(rs)           ( _rN(rs) | _AX )

/* In jit_replace below, _EBX is dummy */
#define jit_movbrm(rs, dd, db, di, ds)                                                \
        (jit_check8(rs)                                                         \
                ? MOVBrm(jit_reg8(rs), dd, db, di, ds)                          \
                : jit_replace(_EBX, rs, _EAX, MOVBrm(_AL, dd, db, di, ds)))

#define jit_ldi_c(d, is)                MOVSBLmr((is), 0,    0,    0, (d))
#define jit_ldxi_c(d, rs, is)           MOVSBLmr((is), (rs), 0,    0, (d))

#define jit_ldi_uc(d, is)               MOVZBLmr((is), 0,    0,    0, (d))
#define jit_ldxi_uc(d, rs, is)          MOVZBLmr((is), (rs), 0,    0, (d))

#define jit_sti_c(id, rs)               jit_movbrm((rs), (id), 0,    0,    0)
#define jit_stxi_c(id, rd, rs)          jit_movbrm((rs), (id), (rd), 0,    0)

#define jit_ldi_s(d, is)                MOVSWLmr((is), 0,    0,    0, (d))
#define jit_ldxi_s(d, rs, is)           MOVSWLmr((is), (rs), 0,    0, (d))

#define jit_ldi_us(d, is)               MOVZWLmr((is), 0,    0,    0,  (d))
#define jit_ldxi_us(d, rs, is)          MOVZWLmr((is), (rs), 0,    0,  (d))

#define jit_sti_s(id, rs)               MOVWrm(jit_reg16(rs), (id), 0,    0,    0)
#define jit_stxi_s(id, rd, rs)          MOVWrm(jit_reg16(rs), (id), (rd), 0,    0)

#define jit_ldi_i(d, is)                MOVLmr((is), 0,    0,    0,  (d))
#define jit_ldxi_i(d, rs, is)           MOVLmr((is), (rs), 0,    0,  (d))

#define jit_sti_i(id, rs)               MOVLrm((rs), (id), 0,    0,    0)
#define jit_stxi_i(id, rd, rs)          MOVLrm((rs), (id), (rd), 0,    0)

#endif /* __lightning_core_h */

