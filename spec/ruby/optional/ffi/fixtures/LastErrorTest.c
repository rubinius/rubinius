/*
 * Copyright (c) 2008 Wayne Meissner. All rights reserved.
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

#if defined(_WIN32) || defined(__WIN32__)
# include <windows.h>
#else
# include <errno.h>
#endif

int setLastError(int error) {
#if defined(_WIN32) || defined(__WIN32__)
    SetLastError(error);
#else
    errno = error;
#endif
    return -1;
}

