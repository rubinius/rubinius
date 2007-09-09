/* $Id: rmd160ossl.c 11908 2007-02-27 11:51:55Z knu $ */

#include "defs.h"
#include "rmd160ossl.h"

void RMD160_Finish(RMD160_CTX *ctx, char *buf) {
	RIPEMD160_Final((unsigned char *)buf, ctx);
}
