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
#include "ptr_array.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define array_good_size(max_size) ((max_size + 7L) & ~7L)
ptr_array ptr_array_new(size_t max_size)
{
	size_t good_size = array_good_size(max_size);
	assert(good_size >= max_size);

	ptr_array retval = (ptr_array)malloc(sizeof(struct ptr_array_t));
	if (retval == NULL)
		return NULL;

	retval->length = 0;
	retval->size = good_size;
	if (good_size != 0)
	{
		retval->array = malloc(sizeof(void*)*good_size);
		if (retval->array == NULL)
		{
			free(retval);
			return NULL;
		}
	}
	else
	{
		retval->array = NULL;
	}
	return retval;
}

void* ptr_array_remove_ordered(ptr_array self, const void *obj)
{
	size_t i;
	assert (self != NULL);

	for (i = 0; i < self->length; ++i)
		if (self->array[i] == obj)
			return ptr_array_remove_index_ordered(self, i);

	return NULL;
}

void *ptr_array_remove_fast(ptr_array self, const void *obj)
{
	size_t i;
	assert (self != NULL);

	for (i = 0; i < self->length; ++i)
		if (self->array[i] == obj)
			return ptr_array_remove_index_fast(self, i);

	return NULL;
}

void * ptr_array_get_index(const ptr_array self, size_t index)
{
	assert (self != NULL);
	assert (index < self->length);
	return (void*)self->array[index];
}

void * ptr_array_set_index(ptr_array self, size_t index, const void *value)
{
	const void * oldval;
	assert( self != NULL);
	assert(index < self->length);

	oldval = self->array[index];
	self->array[index] = value;
	return (void*)oldval;
}

void* ptr_array_remove_index_ordered(ptr_array self, size_t index)
{
	const void *retval;
	size_t i;
	assert (self != NULL);
	assert (index < self->length);

	retval = self->array[index];
	for (i = index + 1; i < self->length; ++i)
		self->array[i - 1] = self->array[i];

	self->length--;
#ifdef CLEANUP_REFERENCES
	self->array[self->length] = NULL;
#endif
	return (void*)retval;
}

void* ptr_array_remove_index_fast(ptr_array self, size_t index)
{
	const void *retval;
	assert (self != NULL);
	assert (index < self->length);

	retval = self->array[index];
	self->length--;
	self->array[index] = self->array[self->length];

#ifdef CLEANUP_REFERENCES
	self->array[self->length] = NULL;
#endif
	return (void*)retval;
}

int _ptr_array_growing_append(ptr_array self, const void *value)
{
	size_t good_size = array_good_size(self->size + 1);
	void * new_array = realloc(self->array, good_size*sizeof(void*));
	if (new_array == NULL)
		return 0;

	self->size = good_size;
	self->array = new_array;
	self->array[self->length] = value;
	self->length++;
	return 1;
}

void ptr_array_free(ptr_array self)
{
	if (self != NULL)
	{
		free(self->array);
		free(self);
	}
}

int ptr_array_contains(const ptr_array self, const void *value)
{
	int i;
	assert (self != NULL);
	for (i = 0; i < self->length; ++i)
		if (self->array[i] == value)
			return 1;

	return 0;
}

void ptr_array_clear(ptr_array self)
{
#ifdef CLEANUP_REFERENCES
	memset(self->array, 0, self->size * sizeof(void*));
#endif
	self->length = 0;
}
