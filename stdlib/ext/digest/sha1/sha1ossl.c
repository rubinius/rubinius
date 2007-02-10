/* $Id: sha1ossl.c 11131 2006-10-11 12:43:58Z knu $ */

#include "defs.h"
#include "sha1ossl.h"

void
SHA1_Finish(SHA1_CTX *ctx, char *buf)
{
	SHA1_Final(buf, ctx);
}
