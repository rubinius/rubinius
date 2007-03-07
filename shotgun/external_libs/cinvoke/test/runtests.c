/*
C/Invoke Source Code File

Copyright (c) 2006 Will Weisser

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.
   3. The name of the author may not be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdio.h>
#include <cinvoke.h>

void *getep(CInvContext *ctx, CInvLibrary *lib, const char *name) {
	void *ep = cinv_library_load_entrypoint(ctx, lib, name);
	if (!ep)
		fprintf(stderr, "%s cinv_library_load_entrypoint: %s\n", name,
			cinv_context_geterrormsg(ctx));
	return ep;
}

CInvFunction *createfunc(CInvContext *ctx, const char *name,
	const char *retsig, const char *paramsig) {
	CInvFunction *f = cinv_function_create(ctx, CINV_CC_DEFAULT, retsig,
		paramsig);
	if (!f)
		fprintf(stderr, "%s cinv_function_create: %s\n", name,
			cinv_context_geterrormsg(ctx));
	return f;
}

int test1(CInvContext *ctx, CInvLibrary *lib, void *ep,
	CInvFunction *f) {
	if (!cinv_function_invoke(ctx, f, ep, NULL, NULL)) {
		fprintf(stderr, "invoke failed: %s\n", cinv_context_geterrormsg(ctx));
		return 0;
	}
	return 1;
}

int test2(CInvContext *ctx, CInvLibrary *lib, void *ep,
	CInvFunction *f) {
	int retval;
	int arg1 = 22;
	void *args[1] = { &arg1 };

	if (!cinv_function_invoke(ctx, f, ep, &retval, args)) {
		fprintf(stderr, "invoke failed: %s\n", cinv_context_geterrormsg(ctx));
		return 0;
	}

	if (retval != 33) {
		printf("retval = %d (expected 33)\n", retval);
		return 0;
	}
	return 1;
}

int test3(CInvContext *ctx, CInvLibrary *lib, void *ep,
	CInvFunction *f) {
	void *retval;

	if (!cinv_function_invoke(ctx, f, ep, &retval, NULL)) {
		fprintf(stderr, "invoke failed: %s\n", cinv_context_geterrormsg(ctx));
		return 0;
	}

	if (retval != (void *)0xFFFFFF) {
		printf("retval = %p (expected 0xffffff)\n", retval);
		return 0;
	}
	return 1;
}

int test4(CInvContext *ctx, CInvLibrary *lib, void *ep,
	CInvFunction *f) {
	float retval;

	if (!cinv_function_invoke(ctx, f, ep, &retval, NULL)) {
		fprintf(stderr, "invoke failed: %s\n", cinv_context_geterrormsg(ctx));
		return 0;
	}

	if (retval < 1 || retval > 2) {
		printf("retval = %g\n", retval);
		return 0;
	}
	return 1;
}

int test5(CInvContext *ctx, CInvLibrary *lib, void *ep,
	CInvFunction *f) {
	double retval;

	if (!cinv_function_invoke(ctx, f, ep, &retval, NULL)) {
		fprintf(stderr, "invoke failed: %s\n", cinv_context_geterrormsg(ctx));
		return 0;
	}

	if (retval < 1 || retval > 2) {
		printf("retval = %g\n", retval);
		return 0;
	}
	return 1;
}

int test6(CInvContext *ctx, CInvLibrary *lib, void *ep,
	CInvFunction *f) {
	long long int retval;

	int arg1 = 11;
	long long int arg2 = 22;
	float arg3 = 33.5f;
	double arg4 = 44.5;
	char arg5 = '5';
	
	void *args[5] = { &arg1, &arg2, &arg3, &arg4, &arg5 };

	if (!cinv_function_invoke(ctx, f, ep, &retval, args)) {
		fprintf(stderr, "invoke failed: %s\n", cinv_context_geterrormsg(ctx));
		return 0;
	}
	
	if (retval != -5) {
		printf("retval = %llX\n", retval);
		return 0;
	}
	return 1;
}

struct ts {
	double d1;
	char c2;
};
struct ts2 {
	char c1;
	int i1;
	short s1;
	struct ts ts1;
};

int test7(CInvContext *ctx) {
	void *inst;
	struct ts2 *rts2;
	CInvStructure *ts = cinv_structure_create(ctx), *ts2;

	cinv_structure_addmember_value(ctx, ts, "d1", CINV_T_DOUBLE);
	cinv_structure_addmember_value(ctx, ts, "c2", CINV_T_CHAR);
	cinv_structure_finish(ctx, ts);
	
	ts2 = cinv_structure_create(ctx);
	cinv_structure_addmember_value(ctx, ts2, "c1", CINV_T_CHAR);
	cinv_structure_addmember_value(ctx, ts2, "i1", CINV_T_INT);
	cinv_structure_addmember_value(ctx, ts2, "s1", CINV_T_SHORT);
	cinv_structure_addmember_struct(ctx, ts2, "ts1", ts);
	cinv_structure_finish(ctx, ts2);
	
	inst = cinv_structure_create_instance(ctx, ts2);
	{
		char c1 = 'A';
		int i1 = 55;
		short s1 = 44;
		double d1 = 66.0;
		char c2 = 'B';
		void *tsinst;
		
		cinv_structure_instance_setvalue(ctx, ts2, inst, "c1", &c1);
		cinv_structure_instance_setvalue(ctx, ts2, inst, "i1", &i1);
		cinv_structure_instance_setvalue(ctx, ts2, inst, "s1", &s1);

		tsinst = cinv_structure_create_instance(ctx, ts);
		cinv_structure_instance_setvalue(ctx, ts, tsinst, "d1", &d1);
		cinv_structure_instance_setvalue(ctx, ts, tsinst, "c2", &c2);

		cinv_structure_instance_setvalue(ctx, ts2, inst, "ts1", tsinst);
		cinv_structure_delete_instance(ctx, tsinst);
	}

	rts2 = inst;
	
	if (rts2->c1 != 'A' ||
		rts2->i1 != 55 ||
		rts2->s1 != 44 ||
		!(rts2->ts1.d1 > 65 && rts2->ts1.d1 < 67) ||
		rts2->ts1.c2 != 'B') {
		printf("struct = %c %d %d %f %c\n", rts2->c1, rts2->i1, rts2->s1,
			rts2->ts1.d1, rts2->ts1.c2);
		return 0;
	}

	printf("test7\n");

	return 1;
}

int test8(CInvContext *ctx) {
	struct ts2 rts2;
	void *inst;

	CInvStructure *ts = cinv_structure_create(ctx), *ts2;
	cinv_structure_addmember_value(ctx, ts, "d1", CINV_T_DOUBLE);
	cinv_structure_addmember_value(ctx, ts, "c2", CINV_T_CHAR);
	cinv_structure_finish(ctx, ts);
	
	ts2 = cinv_structure_create(ctx);
	cinv_structure_addmember_value(ctx, ts2, "c1", CINV_T_CHAR);
	cinv_structure_addmember_value(ctx, ts2, "i1", CINV_T_INT);
	cinv_structure_addmember_value(ctx, ts2, "s1", CINV_T_SHORT);
	cinv_structure_addmember_struct(ctx, ts2, "ts1", ts);
	cinv_structure_finish(ctx, ts2);

	rts2.c1 = 'A';
	rts2.i1 = 55;
	rts2.s1 = 44;
	rts2.ts1.d1 = 66.0;
	rts2.ts1.c2 = 'B';

	inst = &rts2;
	
	{
		char c1 = 
			*(char *)cinv_structure_instance_getvalue(ctx, ts2, inst, "c1");
		int i1 = 
			*(int *)cinv_structure_instance_getvalue(ctx, ts2, inst, "i1");
		short s1 =
			*(short *)cinv_structure_instance_getvalue(ctx, ts2, inst, "s1");
		void *tsinst = cinv_structure_instance_getvalue(ctx, ts2, inst, "ts1");
		double d1 =
			*(double *)cinv_structure_instance_getvalue(ctx, ts, tsinst, "d1");
		char c2 =
			*(char *)cinv_structure_instance_getvalue(ctx, ts, tsinst, "c2");

		if (c1 != 'A' ||
			i1 != 55 ||
			s1 != 44 ||
			!(d1 > 65 && d1 < 67) ||
			c2 != 'B') {
			printf("struct = %c %d %d %f %c\n", c1, i1, s1,
				d1, c2);
			return 0;
			}
	}

	printf("test8\n");

	return 1;
}

void cbfunc(CInvFunction *f, void *parameters[], void *returnout,
	void *userdata) {
	printf("userdata=%p (want 0xAA)\n", userdata);
	printf("args=%d %c %d  (want 22 A 44)\n", *(int*)parameters[0], *(char*)parameters[1],
		*(int*)parameters[2]);
	*(float *)returnout = 11.0f;
}

int test9(CInvContext *ctx, CInvLibrary *lib, void *ep,
	CInvFunction *f) {
	CInvFunction *proto = cinv_function_create(ctx, CINV_CC_DEFAULT, "f",
		"ici");
	CInvCallback *cb = cinv_callback_create(ctx, proto, (void *)0xAA, cbfunc);

	void *e = cinv_callback_getentrypoint(ctx, cb);

	void *args[1] = { &e };

	if (!cinv_function_invoke(ctx, f, ep, NULL, args)) {
		fprintf(stderr, "invoke failed: %s\n", cinv_context_geterrormsg(ctx));
		return 0;
	}

	cinv_callback_delete(ctx, cb);
	return 1;
}

int test10(CInvContext *ctx, CInvLibrary *lib, void *ep,
	CInvFunction *f) {
	float retval;

	int arg1 = 111;
	short arg2 = 222;
	char arg3 = '3';
	long arg4 = 444;
	long arg5 = 555;
	int arg6 = 666;
	int arg7 = 777;
	long arg8 = 888;
	short arg9 = 999;
	
	void *args[9] = { &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7, &arg8, &arg9 };

	if (!cinv_function_invoke(ctx, f, ep, &retval, args)) {
		fprintf(stderr, "invoke failed: %s\n", cinv_context_geterrormsg(ctx));
		return 0;
	}
	
	if (retval < 0 || retval > 1) {
		printf("retval = %f\n", retval);
		return 0;
	}
	return 1;
}

void cbfunc2(CInvFunction *f, void *parameters[], void *returnout,
	void *userdata) {
#ifdef _WIN32
	char buf[20];
	_i64toa(*(long long *)parameters[7], buf, 16);
	printf("args=%d %d %ld %ld %d %d %x %s"
	" (want 1 2 3 4 5 6 77777777 88888888888888)\n",
		*(int *)parameters[0],
		*(int *)parameters[1],
		*(long *)parameters[2],
		*(long *)parameters[3],
		*(int *)parameters[4],
		*(int *)parameters[5],
		*(int *)parameters[6],
		buf);
#else
	printf("args=%d %d %ld %ld %d %d %x %llx"
	" (want 1 2 3 4 5 6 77777777 88888888888888)\n",
		*(int *)parameters[0],
		*(int *)parameters[1],
		*(long *)parameters[2],
		*(long *)parameters[3],
		*(int *)parameters[4],
		*(int *)parameters[5],
		*(int *)parameters[6],
		*(long long *)parameters[7]);
#endif
	printf("userdata=%p (want 0xBB)\n", userdata);
	*(int *)returnout = 1;
}

int test11(CInvContext *ctx, CInvLibrary *lib, void *ep,
	CInvFunction *f) {
	CInvFunction *proto = cinv_function_create(ctx, CINV_CC_DEFAULT, "i",
		"iilliiie");
	CInvCallback *cb = cinv_callback_create(ctx, proto, (void *)0xBB, cbfunc2);

	void *e = cinv_callback_getentrypoint(ctx, cb);

	void *args[1] = { &e };

	if (!cinv_function_invoke(ctx, f, ep, NULL, args)) {
		fprintf(stderr, "invoke failed: %s\n", cinv_context_geterrormsg(ctx));
		return 0;
	}

	cinv_callback_delete(ctx, cb);
	return 1;
}

#define UPDATE0(test) \
	if (test(ctx)) \
		succeeded++; \
	else \
		failed++;

#define UPDATE(test, retsig, paramsig) \
	tmp = 1; \
	ep = getep(ctx, lib, #test); \
	if (!ep) { tmp = 0; } \
	else { \
		f = createfunc(ctx, #test, retsig, paramsig); \
		if (!f) { tmp = 0; } \
		else { \
			tmp = test(ctx, lib, ep, f); \
			cinv_function_delete(ctx, f); \
		} \
	} \
	if (tmp) \
		succeeded++; \
	else \
		failed++;

#ifdef _WIN32
#define LIBSUFFIX "dll"
#else
#ifdef __APPLE_CC__
#define LIBSUFFIX "dylib"
#else
#define LIBSUFFIX "so"
#endif
#endif

int main(int argc, char **argv) {
	int tmp;
	void *ep;
	CInvFunction *f;
	int succeeded = 0, failed = 0;
	CInvLibrary *lib;
	float pct;
	CInvContext *ctx = cinv_context_create();
	if (ctx == NULL) {
		fprintf(stderr, "Cannot create context\n");
		return 1;
	}

	lib = cinv_library_create(ctx, "./lib." LIBSUFFIX);
	if (!lib) {
		fprintf(stderr, "Cannot load library: %s\n",
			cinv_context_geterrormsg(ctx));
		return 1;
	}

	UPDATE(test1, "", "");
	UPDATE(test2, "i", "i");
	UPDATE(test3, "p", "");
	UPDATE(test4, "f", "");
	UPDATE(test5, "d", "");
	UPDATE(test6, "e", "iefdc");
	UPDATE0(test7);
	UPDATE0(test8);
	UPDATE(test9, "", "p");
	UPDATE(test10, "f", "isclliils");
	UPDATE(test11, "", "p");

	if (!cinv_library_delete(ctx, lib)) {
		fprintf(stderr, "Error deleting library: %s\n",
			cinv_context_geterrormsg(ctx));
		return 1;
	}
	if (!cinv_context_delete(ctx)) {
		fprintf(stderr, "Error deleting context: %s\n",
			cinv_context_geterrormsg(ctx));
		return 1;
	}

	pct = (float)succeeded/(succeeded + failed) * 100;
	printf("Tests complete, %d succeeded, %d failed (%.2f%% succeeded)\n",
		succeeded, failed, pct);

#ifdef _WIN32
	{
	char buf[3];
	fgets(buf, 3, stdin);
	}
#endif
	return 0;
}
