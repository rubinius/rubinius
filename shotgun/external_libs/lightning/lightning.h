/******************************** -*- C -*- ****************************
 *
 *	lightning main include file
 *
 ***********************************************************************/


/***********************************************************************
 *
 * Copyright 2000 Free Software Foundation, Inc.
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



#ifndef __lightning_h
#define __lightning_h

#ifdef __cplusplus
extern "C" {
#endif

#include <lightning/asm-common.h>

#ifndef LIGHTNING_DEBUG
#include <lightning/asm.h>
#endif

#include <lightning/core.h>
#include <lightning/core-common.h>
#include <lightning/funcs-common.h>
#include <lightning/funcs.h>
#include <lightning/fp.h>
#include <lightning/fp-common.h>

#ifndef JIT_R0
#error GNU lightning does not support the current target
#endif

#ifdef __cplusplus
}
#endif

#endif /* __lightning_h */
