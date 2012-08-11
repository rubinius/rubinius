/*
 * Copyright (c) 2007 Wayne Meissner. All rights reserved.
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

#include <sys/types.h>
#include <sys/param.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef void* ptr;
typedef void* pointer;
#ifdef _WIN32
typedef char* caddr_t;
#endif

#define RET(T) T ptr_ret_##T(void* arg1, int offset) { \
    T tmp; memcpy(&tmp, (caddr_t) arg1 + offset, sizeof(tmp)); return tmp; \
}
#define SET(T) void ptr_set_##T(void* arg1, int offset, T value) { \
    memcpy((caddr_t) arg1 + offset, &value, sizeof(value)); \
}
#define TEST(T) SET(T) RET(T)

TEST(int8_t);
TEST(int16_t);
TEST(int32_t);
TEST(int64_t);
TEST(float);
TEST(double);
TEST(pointer);

void*
ptr_return_array_element(void **ptrArray, int arrayIndex) 
{
    return ptrArray[arrayIndex];
}

void
ptr_set_array_element(void **ptrArray, int arrayIndex, void *value)
{    
    ptrArray[arrayIndex] = value;
}

void*
ptr_malloc(int size) 
{
    return calloc(1, size);
}
void
ptr_free(void* ptr)
{
    free(ptr);
}

void*
ptr_from_address(uintptr_t addr)
{
    return (void *) addr;
}

