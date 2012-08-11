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

#include <string.h>

int 
string_equals(const char* s1, const char* s2)
{
    return strcmp(s1, s2) == 0;
}

void 
string_set(char* s1, const char* s2)
{
    strcpy(s1, s2);
}
void
string_concat(char* dst, const char* src)
{
    strcat(dst, src);
}
void
string_dummy(char* dummy)
{
}
const char*
string_null(void)
{
    return NULL;
}

