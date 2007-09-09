/* $Id: md5ossl.c 11708 2007-02-12 23:01:19Z shyouhei $ */

#include "md5ossl.h"

void
MD5_Finish(MD5_CTX *pctx, unsigned char *digest)
{
    MD5_Final(digest, pctx);
}
