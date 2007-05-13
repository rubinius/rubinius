/******************************** -*- C -*- ****************************
 *
 *	Sample RPN calculator using GNU lightning
 *	Binary operators: + - * / % & | ^ < <= > >= = != << >> >>>
 *	Unary operators: _ (unary minus)  and ~ (unary NOT)
 *
 ***********************************************************************/


/***********************************************************************
 *
 * Copyright 2000, 2004 Free Software Foundation, Inc.
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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "lightning.h"

static jit_insn codeBuffer[1024];

typedef int (*pifi) (int);	/* Pointer to Int Function of Int */


enum stack_kind { IMM, EXPR, ARG };
enum operator { LE, GE, NE, LSH, RSHU, RSH };

struct stack_element
{
  enum stack_kind kind;
  int imm;
};

/* Return a new operator TOK2 such that A TOK B = B TOK2 A,
   or 0 if there is none.  */
int
swap_op (int tok)
{
  switch (tok)
    {
    case '<':
    case '>':
      /* Swap < and >.  */
      return '<' ^ '>' ^ tok;

    case LE:
    case GE:
      /* Swap <= and >=.  */
      return LE ^ GE ^ tok;

    case '+':
    case '*':
    case '&':
    case '|':
    case '^':
    case '=':
    case NE:
      /* These are commutative.  */
      return tok;

    default:
      return 0;
    }
}

/* Perform constant folding on the two operands X and Y,
   passing them through the operator TOK.  */
int
fold (int x, int y, int tok)
{
  switch (tok)
    {
    case '+': return x + y;
    case '-': return x - y;
    case '*': return x * y;
    case '/': return x / y;
    case '%': return x % y;
    case '=': return x == y;
    case '<': return x < y;
    case '>': return x > y;
    case '&': return x & y;
    case '|': return x | y;
    case '^': return x ^ y;
    case LE: return x <= y;
    case GE: return x >= y;
    case NE: return x != y;
    case LSH: return x << y;
    case RSH: return x >> y;
    case RSHU: return ((unsigned) x) >> y;
    default: abort ();
    }
}

/* Store in R0 the result of evaluating the operator TOK with
   a register operand SRC and an immediate operand IMM.  */
void
gen_reg_imm (int src, int imm, int tok)
{
  switch (tok)
    {
    case '+': jit_addi_i (JIT_R0, src, imm); break;
    case '-': jit_subi_i (JIT_R0, src, imm); break;
    case '*': jit_muli_i (JIT_R0, src, imm); break;
    case '/': jit_divi_i (JIT_R0, src, imm); break;
    case '%': jit_modi_i (JIT_R0, src, imm); break;
    case '&': jit_andi_i (JIT_R0, src, imm); break;
    case '|': jit_ori_i (JIT_R0, src, imm); break;
    case '^': jit_xori_i (JIT_R0, src, imm); break;
    case '=': jit_eqi_i (JIT_R0, src, imm); break;
    case '<': jit_lti_i (JIT_R0, src, imm); break;
    case '>': jit_gti_i (JIT_R0, src, imm); break;
    case LE: jit_lei_i (JIT_R0, src, imm); break;
    case GE: jit_gei_i (JIT_R0, src, imm); break;
    case NE: jit_nei_i (JIT_R0, src, imm); break;
    case LSH: jit_lshi_i (JIT_R0, src, imm); break;
    case RSH: jit_rshi_i (JIT_R0, src, imm); break;
    case RSHU: jit_rshi_ui (JIT_R0, src, imm); break;
    default: abort ();
    }
}

/* Store in R0 the result of evaluating the operator TOK with
   two register operands SRC1 and SRC2.  */
void
gen_reg_reg (int src1, int src2, int tok)
{
  switch (tok)
    {
    case '+': jit_addr_i (JIT_R0, src1, src2); break;
    case '-': jit_subr_i (JIT_R0, src1, src2); break;
    case '*': jit_mulr_i (JIT_R0, src1, src2); break;
    case '/': jit_divr_i (JIT_R0, src1, src2); break;
    case '%': jit_modr_i (JIT_R0, src1, src2); break;
    case '&': jit_andr_i (JIT_R0, src1, src2); break;
    case '|': jit_orr_i (JIT_R0, src1, src2); break;
    case '^': jit_xorr_i (JIT_R0, src1, src2); break;
    case '=': jit_eqr_i (JIT_R0, src1, src2); break;
    case '<': jit_ltr_i (JIT_R0, src1, src2); break;
    case '>': jit_gtr_i (JIT_R0, src1, src2); break;
    case LE: jit_ler_i (JIT_R0, src1, src2); break;
    case GE: jit_ger_i (JIT_R0, src1, src2); break;
    case NE: jit_ner_i (JIT_R0, src1, src2); break;
    case LSH: jit_lshr_i (JIT_R0, src1, src2); break;
    case RSH: jit_rshr_i (JIT_R0, src1, src2); break;
    case RSHU: jit_rshr_ui (JIT_R0, src1, src2); break;
    default: abort ();
    }
}

static void
pushr (int reg, int *sp)
{
  jit_stxi_i (*sp, JIT_FP, reg);
  *sp += sizeof (int);
}

static void
popr (int reg, int *sp)
{
  *sp -= sizeof (int);
  jit_ldxi_i (reg, JIT_FP, *sp);
}

/* This function does all of lexing, parsing, and picking a good
   order of evaluation...  Needless to say, this is not the best
   possible design, but it avoids cluttering everything with globals. */
pifi
compile_rpn (char *expr)
{
  struct stack_element stack[32];
  int sp = 0;
  int curr_tos = -1;		/* stack element currently in R0 */
  int spill_base, spill_sp;

  pifi fn;
  int ofs;
  fn = (pifi) (jit_get_ip ().iptr);
  jit_leaf (1);
  ofs = jit_arg_i ();

  spill_sp = spill_base = jit_allocai (32 * sizeof (int));

  while (*expr)
    {
      int with_imm;
      int imm;
      int tok;
      int src1, src2;

      /* This is the lexer.  */
      switch (*expr)
	{
	case ' ': case '\t':
	  expr++;
	  continue;

	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
	  stack[sp].kind = IMM;
	  stack[sp++].imm = strtol (expr, &expr, 0);
	  continue;

	case 'x':
	  expr++;
	  stack[sp++].kind = ARG;
	  continue;

	case '~':
	  /* NOT.  Implemented as a XOR with -1.  */
	  stack[sp].kind = IMM;
	  stack[sp++].imm = ~0;
	  tok = '^';
	  break;

	case '_':
	  /* Unary minus.  Transform to 0 - X and go on.
	     Also used to enter negative constants (32_ = -32).  */
	  expr++;
	  stack[sp] = stack[sp - 1];

	  /* Ensure CURR_TOS is correct.  */
	  if (curr_tos == sp - 1)
	    curr_tos = sp;

	  stack[sp - 1].kind = IMM;
	  stack[sp - 1].imm = 0;
	  sp++;
	  tok = '-';
	  break;

	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
	case '&':
	case '|':
	case '^':
	case '=':
	  tok = *expr++;
	  break;

	case '!':
	  /* Get != */
	  expr++;
	  assert (*expr == '=');
	  tok = NE;
	  break;

	case '<':
	  /* Get <, <<, <= */
	  if (expr[1] == '=')
	    expr += 2, tok = LE;
	  else if (expr[1] == '<')
	    expr += 2, tok = LSH;
	  else
	    expr++, tok = '<';
	  break;

	case '>':
	  /* Get >, >>, >>>, >= */
	  if (expr[1] == '=')
	    expr += 2, tok = GE;
	  else if (expr[1] == '>' && expr[2] == '>')
	    expr += 3, tok = RSHU;
	  else if (expr[1] == '>')
	    expr += 2, tok = RSH;
	  else
	    expr++, tok = '>';
	  break;

	default:
	  abort ();
	}

      assert (sp >= 2);

      /* Constant folding.  */
      if (stack[sp - 1].kind == IMM && stack[sp - 2].kind == IMM)
	{
	  stack[sp - 2].imm =
	    fold (stack[sp - 2].imm, stack[sp - 1].imm, tok);
	  sp--;
	  continue;
	}

      /* If possible, ensure that the constant is the RHS, possibly
	 by changing TOK (if it is a comparison).  */
      if (stack[sp - 2].kind == IMM)
	{
	  int swapped_operation = swap_op (tok);
          if (swapped_operation)
	    {
	      tok = swapped_operation;
	      stack[sp - 2].kind = stack[sp - 1].kind;
	      stack[sp - 1].kind = IMM;
	      stack[sp - 1].imm = stack[sp - 2].imm;

	      /* Ensure CURR_TOS is correct.  */
	      if (curr_tos == sp - 1)
	        curr_tos = sp - 2;
	    }
        }

      /* Get the second argument into a register, if not an immediate.
         Also decide which argument will be prepared into JIT_R0 and
         which will be prepared into JIT_V0.  */
      with_imm = 0;
      src1 = JIT_R0;
      src2 = JIT_V0;
      switch (stack[sp - 1].kind)
	{
	case IMM:
	  /* RHS is an immediate, use an immediate instruction.  */
	  with_imm = 1;
	  imm = stack[sp - 1].imm;
	  break;

	case EXPR:
	  /* RHS is an expression, check if it is already in JIT_R0.  */
	  if (curr_tos == sp - 1)
	    {
	      /* Invert the two sources.  */
	      src1 = JIT_V0;
	      src2 = JIT_R0;
	    }
	  else
	    popr (JIT_V0, &spill_sp);

	  curr_tos = -1;
	  break;

	case ARG:
	  jit_getarg_i (JIT_V0, ofs);
	  break;
	}

      /* Get the first argument into a register indicated by SRC1.  */
      switch (stack[sp - 2].kind)
	{
	case IMM:
	  /* LHS is an immediate, check if we must spill the top of stack.  */
	  if (curr_tos != -1)
	    {
	      pushr (JIT_R0, &spill_sp);
	      curr_tos = -1;
	    }

	  jit_movi_i (src1, stack[sp - 2].imm);
	  break;

	case EXPR:
	  /* LHS is an expression, check if it is already in JIT_R0.  */
	  if (curr_tos != sp - 2)
	    {
	      popr (src1, &spill_sp);
	      curr_tos = -1;
	    }
	  else
	    assert (src1 == JIT_R0);
	  break;

	case ARG:
	  if (curr_tos != -1)
	    {
	      pushr (JIT_R0, &spill_sp);
	      curr_tos = -1;
	    }

	  jit_getarg_i (src1, ofs);
	  break;
	}

      /* Set up the new stack entry, which is cached in R0.  */
      sp -= 2;
      curr_tos = sp;
      stack[sp++].kind = EXPR;

      /* Perform the computation.  */
      if (with_imm)
	gen_reg_imm (src1, imm, tok);
      else
	gen_reg_reg (src1, src2, tok);
    }

  assert (sp == 1);
  switch (stack[0].kind)
    {
    case IMM:
      jit_movi_i (JIT_RET, stack[0].imm);
      break;

    case EXPR:
      assert (curr_tos == 0);
      jit_movr_i (JIT_RET, JIT_R0);
      break;

    case ARG:
      jit_getarg_i (JIT_V0, ofs);
      break;
    }

  jit_ret ();
  jit_flush_code ((char *) fn, jit_get_ip ().ptr);

#ifdef LIGHTNING_DISASSEMBLE
  disassemble (stderr, (char *) fn, jit_get_ip ().ptr);
#endif
  return fn;
}


int
main ()
{
  pifi c2f, f2c;
  int i;

  jit_set_ip (codeBuffer);
  c2f = compile_rpn ("32 x 9 * 5 / +");
  f2c = compile_rpn ("5 x 32_ + * 9 /");

#ifndef LIGHTNING_CROSS
  printf ("\nC:");
  for (i = 0; i <= 100; i += 10)
    printf ("%3d ", i);
  printf ("\nF:");
  for (i = 0; i <= 100; i += 10)
    printf ("%3d ", c2f (i));
  printf ("\n");

  printf ("\nF:");
  for (i = 32; i <= 212; i += 10)
    printf ("%3d ", i);
  printf ("\nC:");
  for (i = 32; i <= 212; i += 10)
    printf ("%3d ", f2c (i));
  printf ("\n");
#endif
  return 0;
}
