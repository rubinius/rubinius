/*
 * $Id$
 * Copyright (C) 2007 Technorama Ltd. <oss-ruby@technorama.net>
 */
#include "ossl.h"

VALUE mPKCS5;
VALUE ePKCS5;

/*
 * call-seq:
 *    PKCS5.pbkdf2_hmac(pass, salt, iter, keylen, digest) => string
 *
 * === Parameters
 * * +pass+ - string
 * * +salt+ - string
 * * +iter+ - integer - should be greater than 1000.  2000 is better.
 * * +keylen+ - integer
 * * +digest+ - a string or OpenSSL::Digest object.
 *
 * Available in OpenSSL 0.9.9?.
 *
 * Digests other than SHA1 may not be supported by other cryptography libraries.
 */
static VALUE
ossl_pkcs5_pbkdf2_hmac(VALUE self, VALUE pass, VALUE salt, VALUE iter, VALUE keylen, VALUE digest)
{
#ifdef HAVE_PKCS5_PBKDF2_HMAC
    VALUE str;
    const EVP_MD *md;
    int len = NUM2INT(keylen);
    unsigned char* salt_p;
    unsigned char* str_p;

    StringValue(pass);
    StringValue(salt);
    md = GetDigestPtr(digest);
    str = rb_str_new(0, len);
    salt_p = (unsigned char*)RSTRING_PTR(salt);
    str_p = (unsigned char*)RSTRING_PTR(str);

    if (PKCS5_PBKDF2_HMAC(RSTRING_PTR(pass), RSTRING_LEN(pass), salt_p, RSTRING_LEN(salt), NUM2INT(iter), md, len, str_p) != 1)
        ossl_raise(ePKCS5, "PKCS5_PBKDF2_HMAC");

    return str;
#else
    rb_notimplement();
#endif
}


/*
 * call-seq:
 *    PKCS5.pbkdf2_hmac_sha1(pass, salt, iter, keylen) => string
 *
 * === Parameters
 * * +pass+ - string
 * * +salt+ - string
 * * +iter+ - integer - should be greater than 1000.  2000 is better.
 * * +keylen+ - integer
 *
 * This method is available almost any version OpenSSL.
 *
 * Conforms to rfc2898.
 */
static VALUE
ossl_pkcs5_pbkdf2_hmac_sha1(VALUE self, VALUE pass, VALUE salt, VALUE iter, VALUE keylen)
{
#ifdef HAVE_PKCS5_PBKDF2_HMAC_SHA1
    VALUE str;
    int len = NUM2INT(keylen);

    StringValue(pass);
    StringValue(salt);

    str = rb_str_new(0, len);

    if (PKCS5_PBKDF2_HMAC_SHA1(RSTRING_PTR(pass), RSTRING_LEN(pass), RSTRING_PTR(salt), RSTRING_LEN(salt), NUM2INT(iter), len, RSTRING_PTR(str)) != 1)
        ossl_raise(ePKCS5, "PKCS5_PBKDF2_HMAC_SHA1");

    return str;
#else
    rb_notimplement();
#endif
}

void
Init_ossl_pkcs5()
{
    /*
     * Password-based Encryption
     *
     */
    mPKCS5 = rb_define_module_under(mOSSL, "PKCS5");
    ePKCS5 = rb_define_class_under(mPKCS5, "PKCS5Error", eOSSLError);

    rb_define_module_function(mPKCS5, "pbkdf2_hmac", ossl_pkcs5_pbkdf2_hmac, 5);
    rb_define_module_function(mPKCS5, "pbkdf2_hmac_sha1", ossl_pkcs5_pbkdf2_hmac_sha1, 4);
}
