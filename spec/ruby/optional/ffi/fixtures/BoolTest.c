/*
 * Copyright (c) 2009 Aman Gupta. All rights reserved.
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

#include <stdbool.h>

bool
bool_return_true()
{
    return true;
}

bool
bool_return_false()
{
    return false;
}

bool
bool_return_val(bool value)
{
    return value;
}

bool
bool_reverse_val(bool value)
{
    return value ? false : true;
}
