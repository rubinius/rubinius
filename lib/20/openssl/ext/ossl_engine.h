/*
 * $Id: ossl_engine.h 25189 2009-10-02 12:04:37Z akr $
 * 'OpenSSL for Ruby' project
 * Copyright (C) 2003  Michal Rokos <m.rokos@sh.cvut.cz>
 * Copyright (C) 2003  GOTOU Yuuzou <gotoyuzo@notwork.org>
 * All rights reserved.
 */
/*
 * This program is licenced under the same licence as Ruby.
 * (See the file 'LICENCE'.)
 */
#if !defined(OSSL_ENGINE_H)
#define OSSL_ENGINE_H

extern VALUE cEngine;
extern VALUE eEngineError;

void Init_ossl_engine(void);

#endif /* OSSL_ENGINE_H */
