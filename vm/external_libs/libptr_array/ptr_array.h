/* **************************************************************************
 * Copyright (c) 2007, David Waite
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are
 *  met:
 * 
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice
 *   this list of conditions and the following disclaimer in the 
 *   documentation and/or other materials provided with the distribution.
 * * The name(s) of contributors may not be used to endorse or promote
 *   products derived from this software without specific prior written
 *   permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ************************************************************************* */
#ifndef _POINTER_ARRAY_
#define _POINTER_ARRAY_
#include <sys/types.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Pointer Array
 * *************
 *
 * This type is meant to provide an automatically growing array of
 * pointers, to be used by various software. It is somewhat equivalent
 * to GPtrArray in GNU glib; however, several pieces of functionality
 * are not implemented as they were not needed for the particular
 * project at hand.
 *
 * Structure:
 * 
 *     The library is built to allow for multiple references to be
 *     able to point at the array, even if the array is enlarged (and
 *     as a side effect, moved around memory) multiple times. Arrays
 *     currently cannot be shrunk - it is recommended that an array be
 *     destroyed and recreated if its space requirements decrease
 *     dramatically.
 */
typedef struct ptr_array_t *ptr_array; 

struct ptr_array_t
{
  const void **array;
  size_t length;
  size_t size;
};

/* Create a new, empty pointer array. The returned value must be freed
 * by a call to ptr_array_free.
 *
 * Arguments: initial_size, the initial number of entries to
 *            support. This is useful to plan out initial usage to 
 *            avoid unnececssary reallocation calls.
 * Result: the pointer array, or NULL if allocation failed for some
 *            reason.
 */
ptr_array ptr_array_new(size_t initial_size);
/* Free an existing pointer array.
 * 
 * Arguments: self, the pointer array to free. Can be NULL. Multiple
 *            calls to free has undefined platform results.
 */
void ptr_array_free(ptr_array self);

/* Remove an entry by value from the array, preserving the order of
 * items following the removal point.
 *
 * Arguments: self, the pointer array.
 *            obj, the object to search for in the array.
 * Result: obj if the value was found, otherwise NULL. Note that if
 *            obj is NULL, success is not possible to determine.
 */
void * ptr_array_remove_ordered(ptr_array self, const void *obj);
/* Remove an entry by value from the array, preferring speed over
 * preserving order.
 *
 * Arguments: self, the pointer array
 *            obj, the object to search for in the array.
 * Result: obj if the value was found, otherwise NULL. Note that if
 *            obj is NULL, success is not possible to determine.
 */
void * ptr_array_remove_fast(ptr_array self, const void *obj);

/* Remove an entry at a certain index from the array, preserving the
 * order of items following the removal point.
 *
 * Arguments: self, the pointer array.
 *            index, the position of the object to remove.
 * Result: the value which was removed, otherwise NULL. Note if the
 *            value removed is NULL, success is not possible to
 *            determine. 
 */
void * ptr_array_remove_index_ordered(ptr_array self, size_t index);

/* Remove an entry at a certain index from the array, preferring speed over
 * preserving order.
 *
 * Arguments: self, the pointer array.
 *            index, the position of the object to remove.
 * Result: the value which was removed, otherwise NULL. Note if the
 *            value removed is NULL, success is not possible to
 *            determine. 
 */
void * ptr_array_remove_index_fast(ptr_array self, size_t index);

/* Retrieve the pointer at a certain index.
 *
 * Arguments: self, the pointer array
 *            index, the position of the object to retrieve.
 * Result: the value which exists at the given index.
 */
void * ptr_array_get_index(const ptr_array self, size_t index);

/* Modify the pointer at a certain index.
 * 
 * Arguments: self, the pointer array
 *            index, the position of the object to overwrite
 *            value, the value to overwrite at the specified index
 * Result: the value which was at the index before.
 */
void * ptr_array_set_index(ptr_array self, size_t index, const void *value);

/* Determine if the pointer array contains a particular value
 * 
 * Arguments: self, the pointer array
 *            value, the value which is being searched for in the
 *            array.
 * Result: Non-zero if found, zero if not found.
 */
int ptr_array_contains(const ptr_array self, const void *value);

/* Determine length (number of pointers) of the array.
 *
 * Arguments: self, the pointer array
 * Result: number of entries within the array.
 */
static inline size_t ptr_array_length(const ptr_array self)
{
  return self->length;
}

/* Internal method for appending a value which causes array growth */
int _ptr_array_growing_append(ptr_array self, const void *value);

/* Append a new value to the end of the array.
 * 
 * Arguments: self, the pointer array
 *            value, the pointer to append at the end
 * Result: Non-zero on success, zero on failure.
 */
static inline int ptr_array_append(ptr_array self, const void *value)
{
	if (self->length == self->size)
		return _ptr_array_growing_append(self, value);

	self->array[self->length] = value;
	self->length++;
        return 1;
}

/* Clear all entries from the array. This does not reduce the
 * array size.
 *
 * Arguments: self, the pointer array
 */
void ptr_array_clear(ptr_array self);

#ifdef __cplusplus
}
#endif

#endif
