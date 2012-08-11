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

int test_untagged_enum(int val) {
    return val;
}

int test_untagged_typedef_enum(int val) {
    return val;
}

typedef enum {c1, c2, c3, c4} enum_type1;
enum_type1 test_tagged_typedef_enum1(enum_type1 val) {
    return val;
}

typedef enum {c5 = 42, c6, c7, c8} enum_type2;
enum_type2 test_tagged_typedef_enum2(enum_type2 val) {
    return val;
}

typedef enum {c9 = 42, c10, c11 = 4242, c12} enum_type3;
enum_type3 test_tagged_typedef_enum3(enum_type3 val) {
    return val;
}

typedef enum {c13 = 42, c14 = 4242, c15 = 424242, c16 = 42424242} enum_type4;
enum_type4 test_tagged_typedef_enum4(enum_type4 val) {
    return val;
}

