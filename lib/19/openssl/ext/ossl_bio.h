/*
 * $Id: ossl_bio.h 25189 2009-10-02 12:04:37Z akr $
 * 'OpenSSL for Ruby' team members
 * Copyright (C) 2003
 * All rights reserved.
 */
/*
 * This program is licenced under the same licence as Ruby.
 * (See the file 'LICENCE'.)
 */
#if !defined(_OSSL_BIO_H_)
#define _OSSL_BIO_H_

BIO *ossl_obj2bio(VALUE);
BIO *ossl_protect_obj2bio(VALUE,int*);
VALUE ossl_membio2str0(BIO*);
VALUE ossl_membio2str(BIO*);
VALUE ossl_protect_membio2str(BIO*,int*);

#endif

