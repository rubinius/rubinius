/* $Id: rmd160ossl.c 11131 2006-10-11 12:43:58Z knu $ */

#include "defs.h"
#include "rmd160ossl.h"

void RMD160_Finish(RMD160_CTX *ctx, char *buf) {
	RIPEMD160_Final(buf, ctx);
}
