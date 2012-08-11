/*
 * Copyright (c) 2009 Wayne Meissner. All rights reserved.
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

#ifdef _WIN32
#include <windows.h>
#define sleep(x) Sleep(x)
#endif

#ifndef _WIN32
#include <unistd.h>
#include <pthread.h>
#endif

int testAdd(int a, int b)
{
    return a + b;
};

int testFunctionAdd(int a, int b, int (*f)(int, int))
{
    return f(a, b);
};

void testBlocking(int seconds) {
    sleep(seconds);
};

struct async_data {
    void (*fn)(int);
    int value;
};

static void* asyncThreadCall(void *data)
{
    struct async_data* d = (struct async_data *) data;
    if (d != NULL && d->fn != NULL) {
        (*d->fn)(d->value);
    }

    return NULL;
}

void testAsyncCallback(void (*fn)(int), int value)
{
#ifndef _WIN32
    pthread_t t;
    struct async_data d;
    d.fn = fn;
    d.value = value;
    pthread_create(&t, NULL, asyncThreadCall, &d);
    pthread_join(t, NULL);
#else
    (*fn)(value);
#endif
} 
