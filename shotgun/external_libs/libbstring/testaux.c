/*
 * This source file is part of the bstring string library.  This code was
 * written by Paul Hsieh in 2002-2006, and is covered by the BSD open source 
 * license. Refer to the accompanying documentation for details on usage and 
 * license.
 */

/*
 * testaux.c
 *
 * This file is the C unit test for the bstraux module of Bstrlib.
 */

#include <stdio.h>
#include "bstrlib.h"
#include "bstraux.h"

static int tWrite (const void * buf, size_t elsize, size_t nelem, void * parm) {
bstring b = (bstring) parm;
size_t i;

	if (NULL == b || NULL == buf || 0 == elsize || 0 == nelem)
		return -__LINE__;

	for (i=0; i < nelem; i++) {
		if (0 > bcatblk (b, buf, elsize)) break;
		buf = (const void *) (elsize + (const char *) buf);
	}
	return (int) i;	
}

int test0 (void) {
struct bwriteStream * ws;
bstring s;
int ret = 0;

	printf ("TEST: struct bwriteStream functions.\n");

	ws = bwsOpen ((bNwrite) tWrite, (s = bfromcstr ("")));
	bwsBuffLength (ws, 8);
	ret += 8 != bwsBuffLength (ws, 0);
	bwsWriteBlk (ws, bsStaticBlkParms ("Hello "));
	ret += 0 == biseqcstr (s, "");
	bwsWriteBlk (ws, bsStaticBlkParms ("World\n"));
	ret += 0 == biseqcstr (s, "Hello Wo");
	ret += s != bwsClose (ws);
	ret += 0 == biseqcstr (s, "Hello World\n");

	printf ("\t# failures: %d\n", ret);

	return ret;
}

int test1 (void) {
struct tagbstring t = bsStatic ("Hello world");
bstring b, c, d;
int ret = 0;

	printf ("TEST: bTail and bHead functions.\n");
	b = bTail (&t, 5);
	c = bHead (&t, 5);
	ret += 0 >= biseqcstr (b, "world");
	ret += 0 >= biseqcstr (c, "Hello");
	bdestroy (b);
	bdestroy (c);

	b = bTail (&t, 0);
	c = bHead (&t, 0);
	ret += 0 >= biseqcstr (b, "");
	ret += 0 >= biseqcstr (c, "");
	bdestroy (b);
	bdestroy (c);

	d = bstrcpy (&t);
	b = bTail (d, 5);
	c = bHead (d, 5);
	ret += 0 >= biseqcstr (b, "world");
	ret += 0 >= biseqcstr (c, "Hello");
	bdestroy (b);
	bdestroy (c);
	bdestroy (d);

	printf ("\t# failures: %d\n", ret);

	return ret;
}

int test2 (void) {
struct tagbstring t = bsStatic ("Hello world");
bstring b;
int ret = 0, reto;

	printf ("TEST: bSetChar function.\n");
	ret += 0 <= bSetChar (&t, 4, ',');
	ret += 0 >  bSetChar (b = bstrcpy (&t), 4, ',');
	ret += 0 >= biseqcstr (b, "Hell, world");
	ret += 0 <= bSetChar (b, -1, 'x');
	b->slen = 2;
	ret += 0 >  bSetChar (b, 1, 'i');
	ret += 0 >= biseqcstr (b, "Hi");
	ret += 0 >  bSetChar (b, 2, 's');
	ret += 0 >= biseqcstr (b, "His");
	ret += 0 >  bSetChar (b, 1, '\0');
	ret += blength (b) != 3;
	ret += bchare (b, 0, '?') != 'H';
	ret += bchare (b, 1, '?') != '\0';
	ret += bchare (b, 2, '?') != 's';
	bdestroy (b);

	printf ("\t# failures: %d\n", ret);

	reto = ret;
	ret = 0;

	printf ("TEST: bSetCstrChar function.\n");
	ret += 0 <= bSetCstrChar (&t, 4, ',');
	ret += 0 >  bSetCstrChar (b = bstrcpy (&t), 4, ',');
	ret += 0 >= biseqcstr (b, "Hell, world");
	ret += 0 <= bSetCstrChar (b, -1, 'x');
	b->slen = 2;
	ret += 0 >  bSetCstrChar (b, 1, 'i');
	ret += 0 >= biseqcstr (b, "Hi");
	ret += 0 >  bSetCstrChar (b, 2, 's');
	ret += 0 >= biseqcstr (b, "His");
	ret += 0 >  bSetCstrChar (b, 1, '\0');
	ret += blength (b) != 1;
	ret += bchare (b, 0, '?') != 'H';
	bdestroy (b);

	printf ("\t# failures: %d\n", ret);

	return reto + ret;
}

int test3 (void) {
struct tagbstring t = bsStatic ("Hello world");
bstring b;
int ret = 0;

	printf ("TEST: bFill function.\n");
	ret += 0 <= bFill (&t, 'x', 7);
	ret += 0 >  bFill (b = bstrcpy (&t), 'x', 7);
	ret += 0 >= biseqcstr (b, "xxxxxxx");
	ret += 0 <= bFill (b, 'x', -1);
	ret += 0 >  bFill (b, 'x', 0);
	ret += 0 >= biseqcstr (b, "");
	bdestroy (b);

	printf ("\t# failures: %d\n", ret);

	return ret;
}

int test4 (void) {
struct tagbstring t = bsStatic ("foo");
bstring b;
int ret = 0;

	printf ("TEST: bReplicate function.\n");
	ret += 0 <= bReplicate (&t, 4);
	ret += 0 <= bReplicate (b = bstrcpy (&t), -1);
	ret += 0 >  bReplicate (b, 4);
	ret += 0 >= biseqcstr (b, "foofoofoofoo");
	ret += 0 >  bReplicate (b, 0);
	ret += 0 >= biseqcstr (b, "");
	bdestroy (b);

	printf ("\t# failures: %d\n", ret);

	return ret;
}

int test5 (void) {
struct tagbstring t = bsStatic ("Hello world");
bstring b;
int ret = 0;

	printf ("TEST: bReverse function.\n");
	ret += 0 <= bReverse (&t);
	ret += 0 >  bReverse (b = bstrcpy (&t));
	ret += 0 >= biseqcstr (b, "dlrow olleH");
	b->slen = 0;
	ret += 0 >  bReverse (b);
	ret += 0 >= biseqcstr (b, "");
	bdestroy (b);

	printf ("\t# failures: %d\n", ret);

	return ret;
}

int test6 (void) {
struct tagbstring t = bsStatic ("Hello world");
bstring b;
int ret = 0;

	printf ("TEST: bInsertChrs function.\n");
	ret += 0 <= bInsertChrs (&t, 6, 4, 'x', '?');
	ret += 0 >  bInsertChrs (b = bstrcpy (&t), 6, 4, 'x', '?');
	ret += 0 >= biseqcstr (b, "Hello xxxxworld");
	bdestroy (b);

	printf ("\t# failures: %d\n", ret);

	return ret;
}

int test7 (void) {
struct tagbstring t = bsStatic ("  i am  ");
bstring b;
int ret = 0;

	printf ("TEST: bJustify functions.\n");
	ret += 0 <= bJustifyLeft (&t, ' ');
	ret += 0 <= bJustifyRight (&t, 8, ' ');
	ret += 0 <= bJustifyMargin (&t, 8, ' ');
	ret += 0 <= bJustifyCenter (&t, 8, ' ');
	ret += 0 >  bJustifyLeft (b = bstrcpy (&t), ' ');
	ret += 0 >= biseqcstr (b, "i am");
	ret += 0 >  bJustifyRight (b, 8, ' ');
	ret += 0 >= biseqcstr (b, "    i am");
	ret += 0 >  bJustifyMargin (b, 8, ' ');
	ret += 0 >= biseqcstr (b, "i     am");
	ret += 0 >  bJustifyCenter (b, 8, ' ');
	ret += 0 >= biseqcstr (b, "  i am");
	bdestroy (b);

	printf ("\t# failures: %d\n", ret);

	return ret;
}

int test8 (void) {
struct tagbstring t = bsStatic ("Hello world");
bstring b;
char * c;
int ret = 0;

	printf ("TEST: NetStr functions.\n");
	c = bStr2NetStr (&t);
	ret += 0 != strcmp (c, "11:Hello world,");
	b = bNetStr2Bstr (c);
	ret += 0 >= biseq (b, &t);
	bdestroy (b);
	bcstrfree (c);

	printf ("\t# failures: %d\n", ret);

	return ret;
}

int test9 (void) {
struct tagbstring t = bsStatic ("Hello world");
bstring b, c;
int err, ret = 0;

	printf ("TEST: Base 64 codec.\n");

	b = bBase64Encode (&t);
	ret += 0 >= biseqcstr (b, "SGVsbG8gd29ybGQ=");
	c = bBase64DecodeEx (b, &err);
	ret += 0 != err;
	ret += 0 >= biseq (c, &t);
	bdestroy (b);
	bdestroy (c);

	printf ("\t# failures: %d\n", ret);

	return ret;
}

int test10 (void) {
struct tagbstring t = bsStatic ("Hello world");
bstring b, c;
int err, ret = 0;

	printf ("TEST: UU codec.\n");

	b = bUuEncode (&t);
	ret += 0 >= biseqcstr (b, "+2&5L;&\\@=V]R;&0`\r\n");
	c = bUuDecodeEx (b, &err);
	ret += 0 != err;
	ret += 0 >= biseq (c, &t);
	bdestroy (b);
	bdestroy (c);

	printf ("\t# failures: %d\n", ret);

	return ret;
}

int test11 (void) {
struct tagbstring t = bsStatic ("Hello world");
unsigned char Ytstr[] = {0x72, 0x8f, 0x96, 0x96, 0x99, 0x4a, 0xa1, 0x99, 0x9c, 0x96, 0x8e};
bstring b, c;
int ret = 0;

	printf ("TEST: Y codec.\n");

	b = bYEncode (&t);
	ret += 11 != b->slen;
	ret += 0 >= bisstemeqblk (b, Ytstr, 11);
	c = bYDecode (b);
	ret += 0 >= biseq (c, &t);
	bdestroy (b);
	bdestroy (c);

	printf ("\t# failures: %d\n", ret);

	return ret;
}

int test12 (void) {
struct tagbstring t = bsStatic ("Hello world");
struct bStream * s;
bstring b;
int ret = 0;

	printf ("TEST: bsFromBstr.\n");

	ret = bsread (b = bfromcstr (""), s = bsFromBstr (&t), 6);
	ret += 1 != biseqcstr (b, "Hello ");
	if (b) b->slen = 0;
	ret = bsread (b, s, 6);
	ret += 1 != biseqcstr (b, "world");

	bdestroy (b);
	bsclose (s);

	printf ("\t# failures: %d\n", ret);

	return ret;
}


int main () {
int ret = 0;

	printf ("Direct case testing of bstraux functions\n");

	ret += test0 ();
	ret += test1 ();
	ret += test2 ();
	ret += test3 ();
	ret += test4 ();
	ret += test5 ();
	ret += test6 ();
	ret += test7 ();
	ret += test8 ();
	ret += test9 ();
	ret += test10 ();
	ret += test11 ();
	ret += test12 ();

	printf ("# test failures: %d\n", ret);

	return 0;
}
