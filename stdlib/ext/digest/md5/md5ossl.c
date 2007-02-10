/* $Id: md5ossl.c 11131 2006-10-11 12:43:58Z knu $ */

#include "md5ossl.h"

void
MD5_Finish(MD5_CTX *pctx, unsigned char *digest)
{
    MD5_Final(digest, pctx);
}
