/*
 * $Id: ruby_missing.h 12496 2007-06-08 15:02:04Z technorama $
 * 'OpenSSL for Ruby' project
 * Copyright (C) 2001-2003  Michal Rokos <m.rokos@sh.cvut.cz>
 * All rights reserved.
 */
/*
 * This program is licenced under the same licence as Ruby.
 * (See the file 'LICENCE'.)
 */
#if !defined(_OSSL_RUBY_MISSING_H_)
#define _OSSL_RUBY_MISSING_H_

#define rb_define_copy_func(klass, func) \
	rb_define_method(klass, "initialize_copy", func, 1)


#ifndef HAVE_RB_STR_SET_LEN
#ifndef rb_str_set_len

/* these methods should probably be backported to 1.8 */
#define rb_str_set_len(str, length) do {	\
	RSTRING(str)->ptr[length] = 0;		\
	RSTRING(str)->len = length;		\
} while(0)

#endif
#endif /* ! HAVE_RB_STR_SET_LEN */

#ifndef HAVE_RB_BLOCK_CALL
/* the openssl module doesn't use arg[3-4] and arg2 is always rb_each */
#define rb_block_call(arg1, arg2, arg3, arg4, arg5, arg6) rb_iterate(rb_each, arg1, arg5, arg6)
#endif /* ! HAVE_RB_BLOCK_CALL */

#endif /* _OSSL_RUBY_MISSING_H_ */
