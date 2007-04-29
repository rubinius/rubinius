/* 
This software was developed as part of a project at MIT.
 
 Copyright (c) 2005-2006 Russ Cox,
 Massachusetts Institute of Technology
 
 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
												 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:
 
 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
 
#define	setcontext(u)	_setmcontext(&(u)->mc)
#define	getcontext(u)	_getmcontext(&(u)->mc)
typedef struct mcontext mcontext_t;
typedef struct ucontext ucontext_t;
struct mcontext
{
	ulong	pc;		/* lr */
	ulong	cr;		/* mfcr */
	ulong	ctr;		/* mfcr */
	ulong	xer;		/* mfcr */
	ulong	sp;		/* callee saved: r1 */
	ulong	toc;		/* callee saved: r2 */
	ulong	r3;		/* first arg to function, return register: r3 */
	ulong	gpr[19];	/* callee saved: r13-r31 */
/*
// XXX: currently do not save vector registers or floating-point state
//	ulong	pad;
//	uvlong	fpr[18];	/ * callee saved: f14-f31 * /
//	ulong	vr[4*12];	/ * callee saved: v20-v31, 256-bits each * /
*/
};

struct ucontext
{
	struct {
		void *ss_sp;
		uint ss_size;
	} uc_stack;
	sigset_t uc_sigmask;
	mcontext_t mc;
};

void makecontext(ucontext_t*, void(*)(void), int, ...);
int swapcontext(ucontext_t*, ucontext_t*);
int _getmcontext(mcontext_t*);
void _setmcontext(mcontext_t*);

