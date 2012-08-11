/* 
 * Copyright (C) 2007 Wayne Meissner
 * 
 * All rights reserved.
 *
 * This file is part of ruby-ffi.
 *
 * This code is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * version 3 for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with this work.  If not, see <http://www.gnu.org/licenses/>.
 */


#define MEMSET(buf, value, size) do { \
    int i; for (i = 0; i < size; ++i) buf[i] = value; \
} while(0)
#define MEMCPY(dst, src, size) do { \
    int i; for (i = 0; i < size; ++i) dst[i] = src[i]; \
} while(0)

#define FILL(JTYPE, CTYPE) \
void fill##JTYPE##Buffer(CTYPE* buf, CTYPE value, int size) { MEMSET(buf, value, size); }

#define COPY(JTYPE, CTYPE) \
void copy##JTYPE##Buffer(CTYPE* dst, CTYPE* src, int size) { MEMCPY(dst, src, size); }

#define FUNC(JTYPE, CTYPE) \
    FILL(JTYPE, CTYPE); \
    COPY(JTYPE, CTYPE)
            
FUNC(Byte, char);
FUNC(Short, short);
FUNC(Int, int);
FUNC(Long, long long);
FUNC(Float, float);
FUNC(Double, double);

