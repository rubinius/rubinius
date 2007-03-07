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
#include <stdlib.h>

#ifdef _WIN32
#define CDECL __cdecl
#define STDCALL _stdcall
#define DLLEXPORT __declspec(dllexport)
#else
#define CDECL
#define STDCALL
#define DLLEXPORT
#endif

DLLEXPORT void test1() {
	printf("test1\n");
}

DLLEXPORT int test2(int p) {
	printf("test2: %d (want 22)\n", p);
	return 33;
}
DLLEXPORT void * test3() {
	printf("test3\n");
	return (void*)0xFFFFFF;
}
DLLEXPORT float test4() {
	printf("test4\n");
	return 1.1000000000000124422f;
}
DLLEXPORT double test5() {
	printf("test5\n");
	return 1.100000000000000000004093840394830948304983049230920124422;
}
DLLEXPORT long long int test6(int a0, long long int a1, float a2, double a3, char a4) {
#ifdef _WIN32
	char buf[20];
	_i64toa(a1, buf, 10);
	printf("test6: %d %s %f %f %c (want 11 22 %f %f 5)\n", a0, buf, a2, a3, a4,
		33.5, 44.5);
#else
	printf("test6: %d %lld %f %f %c (want 11 22 %f %f 5)\n", a0, a1, a2, a3, a4,
		33.5, 44.5);
#endif
	return -5;
}
DLLEXPORT void test9(float (CDECL *f)(int, char, int)) {
	printf("test9: %f (want 11.0)\n", f(22, 'A', 44));
}
DLLEXPORT float test10(int i1, short i2, char i3, long i4, long i5,
	int i6, int i7, long i8, short i9) {
	printf("test10: %d %d %c %ld %ld %d %d %ld %d (want 111 222 3 444 555 666 777 888 999\n", i1, i2, i3, i4, i5, i6, i7, i8, i9);
	return 0.7f;
}
DLLEXPORT void test11(int (CDECL *f)(int, int, long, long, int, int,
int, long long)) {
	printf("test11: %d (want 1)\n", f(1, 2, 3, 4, 5, 6, 0x77777777, 0x88888888888888LL));
}
