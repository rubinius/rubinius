#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#if defined (BSTRLIB_CAN_USE_IOSTREAM)
#include <iostream>
/* #include <string> */
#endif
#include "bstrwrap.h"
#include "bstraux.h"
#include "bsafe.h"

static struct tagbstring x = bsStatic ("test");

void cTest (void) {
static struct tagbstring a = bsStatic ("this is lower case");
static struct tagbstring vowels = bsStatic ("thi");
struct tagbstring t;
bstring b, u;
struct bstrList * sl;
int l;

	printf ("\nC test\n");
	b = bfromcstr ("");
	bAssign (b, &a);
	bUppercase (b);
	printf ("- <%s>\n", b->data);
	printf ("- caselessCmp (%s,%s) -> %d\n", b->data, a.data, bCaselessCmp (b, &a));
	bTrunc (b, 7);
	printf ("- <%s>\n", b->data);
	b = bTail (b, 2);
	printf ("- <%s>\n", b->data);
	bFill (b, 'x', 0);
	printf ("- <%s>\n", b->data);
	bFill (b, 'x', 16);
	printf ("- <%s>\n", b->data);
	bSubs (b, 2, 4, &a, '?');
	printf ("- <%s>\n", b->data);
	bReplicate (b, 3);
	printf ("- <%s>\n", b->data);

	printf ("Vowel split test\n");

	cstr2tbstr (t,"aeiou");
	sl = bsplits (b, &t);
	if (sl) {
		int i;
		for (i=0; i < sl->qty; i++) {
			printf ("-\t<%s>\n", sl->entry[i]->data);
		}
		u = bjoin (sl, NULL);

		if (u) printf ("Rejoined: <%s>\n", u->data);
		bdestroy (u);

		bstrListDestroy (sl);
	} else {
		printf ("!!!!! Unable to bsplit! [%d]\n", __LINE__);
	}

	printf ("bsplits(\"%s\", \"x!\") split test\n", b->data);

	cstr2tbstr (t,"x!");
	sl = bsplits (b, &t);
	if (sl) {
		int i;
		bstring w;
		struct tagbstring xx = bsStatic ("x");

		for (i=0; i < sl->qty; i++) {
			printf ("-\t<%s>\n", sl->entry[i]->data);
		}
		w = bjoin (sl, &xx);
		if (w) printf ("Rejoined: <%s>\n", w->data);

		bstrListDestroy (sl);
	} else {
		printf ("!!!!! Unable to bsplit! [%d]\n", __LINE__);
	}

	bReplicate (b, 0);
	printf ("- bReplicate (*, 0) -> <%s>\n", b->data);

	bCatCstr (b, "x");
	bReplicate (b, 1);
	printf ("- bReplicate (\"x\", 1) -> <%s>\n", b->data);

	b->slen = 1;
	bReplicate (b, 3);
	printf ("- bReplicate (\"x\", 3) -> <%s>\n", b->data);

	bReplicate (b, 0);
	printf ("- bReplicate (*, 0) -> <%s>\n", b->data);

	bAssign (b, &a);
	b = bTail (b, 0);
	printf ("- <%s>\n", b->data);
	bSubs (b, 6, 4, &a, '?');
	printf ("- <%s>\n", b->data);
	bReverse (b);
	printf ("- <%s>\n", b->data);
	printf ("- binchr (<%s>, 0, <%s>) = %d\n", b->data, vowels.data, binchr (b, 0, &vowels));
	l = b->slen;
	binsert (b, 40, b, '*');
	printf ("- binsert (b, 40, b, '*') -> <<%s>>\n", b->data);

	bdelete (b, l, b->slen);
	printf ("- bdelete (b, %d, *) -> <<%s>>\n", l, b->data);

	binsert (b, 40, b, '*');
	printf ("- binsert (b, 40, b, '*') -> <<%s>>\n", b->data);

	bdelete (b, 40, l);
	printf ("- bdelete (b, 40, %d) -> <<%s>>\n", l, b->data);

	bdelete (b, l, b->slen);
	printf ("- bdelete (b, %d, *) -> <<%s>>\n", l, b->data);

	binsertch (b, 4, 5, '*');
	printf ("- binsertch (b, 4, 5, '*') -> <<%s>>\n", b->data);

	/* An illegal operations */

	printf ("- bconcat (static<'%s'>, static<'%s'>) -> ", a.data, vowels.data);
	l = bconcat (&a, &vowels);
	printf ("%d\n", l);

	printf ("- bconcat (static<'%s'>, static<'%s'>) -> ", vowels.data, a.data);
	l = bconcat (&vowels, &a);
	printf ("%d\n", l);

	printf ("- bconcat (static<'%s'>, <'%s'>) -> ", vowels.data, b->data);
	l = bconcat (&vowels, b);
	printf ("%d\n", l);

	bwriteprotect (*b);
	printf ("- bconcat (protected<'%s'>, static<'%s'>) -> ", b->data, vowels.data);
	l = bconcat (b, &vowels);
	printf ("%d\n", l);
}

void cppTest (void) {
FILE * fp = NULL;
CBString q, r;

	printf ("\nC++ test\n");

	fp = fopen ("..\\license.txt", "r");
	if (fp == NULL) fp = fopen ("license.txt", "r");
	if (NULL != fp) {
		while (q.gets ((bNgetc)fgetc, fp, '\n') >= 0) {
			printf ("- %s", (const char *)q);
		}

		try {
			int c;

			fseek (fp, 0, SEEK_SET);

			struct CBStream s ((bNread) fread, fp);

			printf ("bsStream buffer size = %d\n", s.buffLengthGet ());
			c = 0;
			while (!s.eof ()) {
				q = s.readLine ('\n');
				printf ("* %s", (const char *) q);
				c++;
				if ((c % 10) == 9) {
					r.format ("-- Line: %d ------------------------------------------------------------------\n", c + 1);
					s.unread (r);
				}
			}
		}

		catch (struct CBStringException err) {
			printf ("Exception thrown [%d]: %s\n", __LINE__, err.what());
		}

		fseek (fp, 0, SEEK_SET);

		q.read ((bNread)fread, fp);
		printf ("<%s>\n", (const char *)q);

		fclose (fp);

#if defined(BSTRLIB_CAN_USE_STL)
		{
			CBStringList cv;
			cv.split (q, '\n');
			CBString c(cv);
			for (size_t i=0; i < cv.size(); i++) {
				printf ("+ <%s>\n", (const char *)cv.at (i));
			}
			printf ("join constructor: <%s>\n", (const char *)c);
		}
#endif
	}

	printf ("x.mlen = %d\n", x.mlen);
	printf ("x.slen = %d\n", x.slen);
	printf ("x.data = %s\n", x.data);

	q.format ("%s[%d]\n", __FILE__, __LINE__);

	q = __FILE__;
	q.formata ("[%d]", __LINE__);

	printf ("q = <%s>\n", (const char *)q);

	r = x;

	printf ("q == q           = %d\n", q == q);
	printf ("q != q           = %d\n", q != q);
	printf ("q < q            = %d\n", q < q);
	printf ("q <= q           = %d\n", q <= q);
	printf ("q > q            = %d\n", q > q);
	printf ("q >= q           = %d\n", q >= q);

	printf ("q == r           = %d\n", q == r);
	printf ("q != r           = %d\n", q != r);
	printf ("q <  r           = %d\n", q <  r);
	printf ("q <= r           = %d\n", q <= r);
	printf ("q >  r           = %d\n", q >  r);
	printf ("q >= r           = %d\n", q >= r);

	printf ("q.find (r, 0)    = %d\n", q.find (r, 0));
	printf ("q.find (r, 1)    = %d\n", q.find (r, 1));
	printf ("q.find (r, 2)    = %d\n", q.find (r, 2));
	printf ("q.find (r, 3)    = %d\n", q.find (r, 3));
	printf ("q.find (r, 4)    = %d\n", q.find (r, 4));
	printf ("q.find (\"pp\", 0) = %d\n", q.find ("pp", 0));
	printf ("q.find (\"pp\", 1) = %d\n", q.find ("pp", 1));
	printf ("q.find (\"pp\", 2) = %d\n", q.find ("pp", 2));

	printf ("q.reversefind (r, %d)    = %d\n", q.length()-1, q.reversefind (r, q.length()-1));
	printf ("q.reversefind (\"pp\", %d) = %d\n", q.length()-1, q.reversefind ("pp", q.length()-1));

	printf ("\n\nq = <%s>\nr = <%s>\nx = <%s>\n", (const char *)q, (const char *)r, x.data);

	for (int i=0; i < q.length(); i++) {
		printf ("(%c)", (char) q[i]);
	}
	printf ("\n");

	q.fill (20, 'y');
	r.fill (20, 'x');
	q += r;
	q.insert (10, q);
	puts ((const char *)q);

	q += q;

	puts ((const char *)q);

	q = "abc";
	q.repeat (((80 - 1) / q.length()) + 1);
	q.trunc (80);
	puts ((const char *)q);

	q = "   <-- lws rws-->   ";
	printf ("|%s|\n", (const char *)q);
	r = q;
	r.rtrim ();
	printf ("|%s|\n", (const char *)r);
	r = q;
	r.ltrim ();
	printf ("|%s|\n", (const char *)r);
	r = q;
	r.trim ();
	printf ("|%s|\n", (const char *)r);

	q = "a   <-- lws rws-->   ";
	printf ("|%s|\n", (const char *)q);
	r = q;
	r.rtrim ();
	printf ("|%s|\n", (const char *)r);
	r = q;
	r.ltrim ();
	printf ("|%s|\n", (const char *)r);
	r = q;
	r.trim ();
	printf ("|%s|\n", (const char *)r);

	q = "   <-- lws rws-->   b";
	printf ("|%s|\n", (const char *)q);
	r = q;
	r.rtrim ();
	printf ("|%s|\n", (const char *)r);
	r = q;
	r.ltrim ();
	printf ("|%s|\n", (const char *)r);
	r = q;
	r.trim ();
	printf ("|%s|\n", (const char *)r);

#if defined(BSTRLIB_CAN_USE_STL)
	{
		CBStringList cv;
		cv.split (q, CBString(" a"));
		for (size_t i=0; i < cv.size(); i++) {
			printf ("+ <%s>\n", (const char *)cv.at (i));
		}
	}
#endif

	q.trunc(0);
	q = q + 'c';
	printf ("<%s>\n", (const char *) q);
	q = q + 'a';
	printf ("<%s>\n", (const char *) q);
	q = q + 'r';
	printf ("<%s>\n", (const char *) q);
	q.formata ("%g", 3.4);
	printf ("<%s>\n", (const char *) q);
	q.formata ("%g", 1.5);
	printf ("<%s>\n", (const char *) q);

	q.trunc(0);
	q.repeat (2);
	printf ("<%s>\n", (const char *) q);

#ifdef BSTRLIB_THROWS_EXCEPTIONS
	try {
		q.trunc (-1);
	}
	catch (struct CBStringException err) {
		printf ("Exception thrown: %s\n", err.what());
	}

	try {
		putchar (q[-1]);
	}
	catch (struct CBStringException err) {
		printf ("Exception thrown: %s\n", err.what());
	}

	try {
		putchar (q[10000000]);
	}
	catch (struct CBStringException err) {
		printf ("Exception thrown: %s\n", err.what());
	}
#endif

	q = "the";
	q *= 4;
	printf ("<%s>\n", (const char *) q);
	q = 4 * CBString ("the");
	printf ("<%s>\n", (const char *) q);
	q = CBString ("the") * 4;
	printf ("<%s>\n", (const char *) q);
}

void test3 (void) {
FILE * fp;
	/* fp = fopen ("..\\adentrap.csv", "rb");
	if (fp == NULL) fp = fopen ("adentrap.csv", "rb"); */

	fp = fopen ("..\\license.txt", "rb");
	if (fp == NULL) fp = fopen ("license.txt", "rb");

	if (fp) {
		struct bStream * s;
		bstring r = bfromcstr ("");
		struct tagbstring xx = bsStatic ("\r\n");

		s = bsopen ((bNread) fread, fp);
		while (0 == bsreadlns (r, s, &xx)) {
		/* while (0 == bsreadln (r, s, '\n')) { */
			printf ("<<%s>>\n", r->data);
		}
		bsclose (s);
		fclose (fp);
	}
}

void test4 (void) {
struct tagbstring f0 = bsStatic ("a"), f1 = bsStatic ("ab"), f2 = bsStatic ("b");
struct tagbstring r0 = bsStatic ("x"), r1 = bsStatic ("yz"), r2 = bsStatic ("");
struct tagbstring t;
bstring b;

	printf ("\nReplaceAll test\n");

	b = bfromcstr ("ababababababab");
	bReplaceAll (b, &f0, &r0, 0);
	printf ("<%s>\n", b->data);

	bReplaceAll (b, &r0, &f0, 0);
	printf ("<%s>\n", b->data);

	bReplaceAll (b, &f1, &r1, 0);
	printf ("<%s>\n", b->data);

	bReplaceAll (b, &r1, &f0, 0);
	printf ("<%s>\n", b->data);

	bReplaceAll (b, &f0, &f1, 0);
	printf ("<%s>\n", b->data);

	bReplaceAll (b, &f0, &r1, 0);
	printf ("<%s>\n", b->data);

	bReplaceAll (b, &f2, &f1, 0);
	printf ("<%s>\n", b->data);

	bReplaceAll (b, &r0, &f0, 0);
	printf ("<%s>\n", b->data);

	bReplaceAll (b, &r2, &f0, 0);
	printf ("<%s>\n", b->data);

	bReplaceAll (b, &r1, &r2, 0);
	printf ("<%s>\n", b->data);

	blk2tbstr (t, b->data, 1);
	bReplaceAll (b, &t, &r0, 0);
	printf ("<%s>\n", b->data);
}

void test5 (void) {
struct tagbstring t0 = bsStatic ("Hello, world.");
bstring b;
char * nStr0;

	b = bNetStr2Bstr (nStr0 = bStr2NetStr (&t0));
	printf ("\nbstring <-> netstring conversion\n");
	printf ("<%s>\n", t0.data);
	printf ("%s\n", nStr0);
	printf ("<%s>\n", b->data);
	printf ("%s\n", bStr2NetStr (b));

	nStr0 = bStr2NetStr (b = bNetStr2Bstr ("0:,"));
	printf ("\nEmpty strings\n");
	printf ("%s\n", nStr0);
	printf ("<%s>\n", b->data);
	printf ("<%s>\n", bNetStr2Bstr (nStr0)->data);

	printf ("\nNULL parameter\n");
	printf ("bNetStr2Bstr(NULL) = %p\n", (void *) bNetStr2Bstr (NULL));
	printf ("bStr2NetStr(NULL)  = %p\n", (void *) bStr2NetStr (NULL));
}

static int cb0 (void * parm, int ofs, int len) {
bstring b = bmidstr ((bstring) parm, ofs, len);
	printf ("<%d:%d:%s>", ofs, len, b->data);
	bdestroy (b);
	return 0;
}

void test6 (void) {
struct tagbstring t0 = bsStatic ("abaabacabaaaabaacaabaaaaaa");
struct tagbstring t1 = bsStatic ("bc");

	printf ("\nbsplit?cb test\n");
	bsplitcb (&t0, 'b', 0, cb0, &t0);
	printf ("\n");
	bsplitscb (&t0, &t1, 0, cb0, &t0);
	printf ("\n");
	bsplitcb (&t0, 'd', 0, cb0, &t0);
	printf ("\n");
}

void test7 (void) {
CBString dbl("-3.7e+10");
CBString flt("-3.7");
CBString i("-7");
CBString ui("7");

	printf ("\nNumeric conversion test\n");

	try {
		CBString xx("animal");
		printf ("%d", (int)xx);
	}
	catch (struct CBStringException err) {
		printf ("Exception correctly thrown [%d]: %s\n", __LINE__, err.what());
	}

	printf ("<%g>\n", (double)dbl);
	printf ("<%f>\n", (float)flt);
	printf ("<%d>\n", (int)i);
	printf ("<%u>\n", (unsigned int)ui);
}

#if 0
#if defined (BSTRLIB_CAN_USE_IOSTREAM)
#include <iostream>
/* #include <string> */
#endif
#endif

#if defined (BSTRLIB_CAN_USE_STL)
#include <string>
#endif

#if 0
using namespace std;

void test8 (void) {
#if defined (BSTRLIB_CAN_USE_IOSTREAM)
	CBString b, c;
/*	std::string b, c; */

/*	std::cin >> b >> c;
	std::cout << "<" << b << ">" << c << std::endl; */

	/* std:: */ cout << "Enter some text" << /* std:: */ endl;

	getline (/* std:: */ cin, b);
	getline (/* std:: */ cin, c);
	
	/* std:: */ cout << b << /* std:: */ endl << c;
#endif

	return;
}
#endif

#include <time.h>

#define TESTSTRING1 ("<sometag name=\"John Doe\" position=\"Executive VP Marketing\"/>")

void test9 (void) {
	int i, c;
	clock_t t0, t1;
	
#if defined (BSTRLIB_CAN_USE_STL)
	t0 = clock();
	for (c=i=0; i < 1000000; i++) {
		std::string b;
		c += b.length();
	}
	t1 = clock() - t0;

	printf ("std::string empty constructor performance: %f\n", (float)t1 / CLOCKS_PER_SEC );

	t0 = clock();
	for (c=i=0; i < 1000000; i++) {
		std::string b (TESTSTRING1);
		c += b.length();
	}
	t1 = clock() - t0;

	printf ("std::string non-empty constructor performance: %f\n", (float)t1 / CLOCKS_PER_SEC );

	t0 = clock();
	{
		std::string b;
		for (c=i=0; i < 1000000; i++) {
			b = TESTSTRING1;
			c += b.length();
		}
	}
	t1 = clock() - t0;

	printf ("std::string char * assignment performance: %f\n", (float)t1 / CLOCKS_PER_SEC );

	t0 = clock();
	{
		std::string b (TESTSTRING1);
		for (c=i=0; i < 1000000; i++) {
			c += b[0];
			c += b[11];
			c += b.c_str()[1];
		}
	}
	t1 = clock() - t0;

	printf ("std::string char extraction performance: %f\n", (float)t1 / CLOCKS_PER_SEC );

	t0 = clock();
	{
		std::string a (TESTSTRING1);
		std::string b;
		for (c=i=0; i < 10000; i++) {
			b += a;
			b += "!!";
		}
	}
	t1 = clock() - t0;

	printf ("std::string concatenation performance: %f\n", (float)t1 / CLOCKS_PER_SEC );

	t0 = clock();
	{
		std::string b ("Dot. 123. Some more data.");
		for (c=i=0; i < 1000000; i++) {
			c = b.find ('.');
			c = b.find ("123");
			/* c = b.find_first_of ("sm"); */
		}
	}
	t1 = clock() - t0;

	printf ("std::string scan performance: %f\n", (float)t1 / CLOCKS_PER_SEC );
#endif
	t0 = clock();
	for (c=i=0; i < 1000000; i++) {
		CBString b;
		c += b.length();
	}
	t1 = clock() - t0;
	printf ("CBString empty constructor performance: %f\n", (float)t1 / CLOCKS_PER_SEC );

	t0 = clock();
	for (c=i=0; i < 1000000; i++) {
		CBString b (TESTSTRING1);
		c += b.length();
	}
	t1 = clock() - t0;
	printf ("CBString non-empty constructor performance: %f\n", (float)t1 / CLOCKS_PER_SEC );

	t0 = clock();
	{
		CBString b;
		for (c=i=0; i < 1000000; i++) {
			b = TESTSTRING1;
			c += b.length();
		}
	}
	t1 = clock() - t0;

	printf ("CBString char * assignment performance: %f\n", (float)t1 / CLOCKS_PER_SEC );

	t0 = clock();
	{
		CBString b (TESTSTRING1);
		for (c=i=0; i < 1000000; i++) {
			c += b[0];
			c += b[11];
			c += ((const char *)b)[1];
		}
	}
	t1 = clock() - t0;

	printf ("CBString char extraction performance: %f\n", (float)t1 / CLOCKS_PER_SEC );

	t0 = clock();
	{
		CBString a (TESTSTRING1);
		CBString b;
		for (c=i=0; i < 10000; i++) {
			b += a;
			b += "!!";
		}
	}
	t1 = clock() - t0;

	printf ("CBString concatenation performance: %f\n", (float)t1 / CLOCKS_PER_SEC );

	t0 = clock();
	{
		CBString b ("Dot. 123. Some more data.");
		for (c=i=0; i < 1000000; i++) {
			c = b.find ('.');
			c = b.find ("123");
			/* c = b.find ("sm"); */
		}
	}
	t1 = clock() - t0;

	printf ("CBString scan performance: %f\n", (float)t1 / CLOCKS_PER_SEC );

	return;
}

void test10 (void) {
	time_t tloc;
	struct tm tod;
	bstring b;

	time (&tloc);
	tod = *localtime (&tloc);
	b = bStrfTime ("%A %B %d, %Y", &tod);
	printf ("<%s>\n", b->data);
	bSecureDestroy (b);

	b = bAscTime (&tod);
	printf ("%s", b->data);
	bSecureDestroy (b);

	b = bCTime (&tloc);
	printf ("%s", b->data);
	bSecureDestroy (b);
}

int xfgetc (void * ctx) {
	return fgetc ((FILE *) ctx);
}

void test11 (void) {
bstring inp;

	printf ("Secure input:\n");
	inp = bSecureInput (24, '\n', xfgetc, stdin);
	printf ("%s", bdatae (inp, "Failed to read secure input\n"));
	bSecureDestroy (inp);
}

int main () {
	cppTest ();
	cTest ();
	test3 ();
	test4 ();
	test5 ();
	test6 ();
	test7 ();
#if 0
	test8 ();
	test9 ();
#endif
	test10 ();
#if 0
	test11 ();
#endif

	return 0;
}
