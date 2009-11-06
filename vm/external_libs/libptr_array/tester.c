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
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "ptr_array.h"

int main(int argc, char ** argv);

void test_new()
{
	ptr_array p1 = ptr_array_new(10);
	assert(p1 != NULL);
	assert(p1->size >= 10);
	assert(p1->array != NULL);
	assert(p1->length == 0);
	ptr_array_free(p1);
}

void test_new_oom()
{
	fprintf(stderr, "Your malloc implementation may report an (ignorable) "
			"error for this test\n");
	ptr_array p1 = ptr_array_new(1000000000);
	assert (p1 == NULL);
}

void test_free()
{
	ptr_array p1 = ptr_array_new(1);
	ptr_array_free(p1);
	ptr_array p2 = NULL;
	ptr_array_free(p2);
}

void test_new_zero()
{
	ptr_array p1 = ptr_array_new(0);
	assert(p1 != NULL);
	assert(p1->size == 0);
	assert(p1->array == NULL);
	assert(p1->length == 0);
	ptr_array_free(p1);
}

static ptr_array setup_data()
{
	ptr_array p1 = ptr_array_new(4);
	ptr_array_append(p1, &malloc);
	ptr_array_append(p1, &realloc);
	ptr_array_append(p1, &free);
	ptr_array_append(p1, &main);
	return p1;
}

void test_remove_ordered()
{
	ptr_array p1 = setup_data();
	void * value = 
	ptr_array_remove_ordered(p1, &realloc);
	assert(value == &realloc);
	assert(ptr_array_get_index(p1, 0) == &malloc);
	assert(ptr_array_get_index(p1, 1) == &free);
	assert(ptr_array_get_index(p1, 2) == &main);
	assert(ptr_array_length(p1) == 3);

	value =
	ptr_array_remove_ordered(p1, &malloc);
	assert(value == &malloc);
	assert(ptr_array_get_index(p1, 0) == &free);
	assert(ptr_array_get_index(p1, 1) == &main);
	assert(ptr_array_length(p1) == 2);

	value =
	ptr_array_remove_ordered(p1, &main);
	assert(value == &main);
	assert(ptr_array_get_index(p1, 0) == &free);
	assert(ptr_array_length(p1) == 1);

	value =
	ptr_array_remove_ordered(p1, &free);
	assert (value == &free);
	assert(ptr_array_length(p1) == 0);
}

void test_remove_ordered_nonexistant()
{
	ptr_array p1 = setup_data();
	void * value = ptr_array_remove_ordered(p1, "some random data");
	assert(value == NULL);
}

void test_remove_index_ordered()
{
	ptr_array p1 = setup_data();
	void * value = 
	ptr_array_remove_index_ordered(p1, 1);
	assert(value == &realloc);
	assert(ptr_array_get_index(p1, 0) == &malloc);
	assert(ptr_array_get_index(p1, 1) == &free);
	assert(ptr_array_get_index(p1, 2) == &main);
	assert(ptr_array_length(p1) == 3);

	value =
	ptr_array_remove_index_ordered(p1, 0);
	assert(value == &malloc);
	assert(ptr_array_get_index(p1, 0) == &free);
	assert(ptr_array_get_index(p1, 1) == &main);
	assert(ptr_array_length(p1) == 2);

	value =
	ptr_array_remove_index_ordered(p1, 1);
	assert(value == &main);
	assert(ptr_array_get_index(p1, 0) == &free);
	assert(ptr_array_length(p1) == 1);

	value =
	ptr_array_remove_index_ordered(p1, 0);
	assert (value == &free);
	assert(ptr_array_length(p1) == 0);
}
void test_remove_fast()
{
	ptr_array p1 = setup_data();
	void * value = 
	ptr_array_remove_fast(p1, &realloc);
	assert(value == &realloc);
	assert(ptr_array_contains(p1, &malloc));
	assert(ptr_array_contains(p1, &free));
	assert(ptr_array_contains(p1, &main));
	assert(ptr_array_length(p1) == 3);

	value =
	ptr_array_remove_fast(p1, &malloc);
	assert(value == &malloc);
	assert(ptr_array_contains(p1, &free));
	assert(ptr_array_contains(p1, &main));
	assert(ptr_array_length(p1) == 2);

	value =
	ptr_array_remove_fast(p1, &main);
	assert(value == &main);
	assert(ptr_array_contains(p1, &free));
	assert(ptr_array_length(p1) == 1);

	value =
	ptr_array_remove_fast(p1, &free);
	assert (value == &free);
	assert(ptr_array_length(p1) == 0);
}

void test_remove_fast_nonexistant()
{
	ptr_array p1 = setup_data();
	void * value = ptr_array_remove_fast(p1, "some random data");
	assert(value == NULL);
}

void test_remove_index_fast()
{
	ptr_array p1 = setup_data();
	void * value = 
	ptr_array_remove_index_fast(p1, 1);
	assert(value == &realloc);
	assert(ptr_array_contains(p1, &malloc));
	assert(ptr_array_contains(p1, &free));
	assert(ptr_array_contains(p1, &main));
	assert(ptr_array_length(p1) == 3);

	value =
	ptr_array_remove_index_fast(p1, 0);
	assert(value == &malloc);
	assert(ptr_array_contains(p1, &free));
	assert(ptr_array_contains(p1, &main));
	assert(ptr_array_length(p1) == 2);

	value =
	ptr_array_remove_index_fast(p1, 1);
	assert(value == &main);
	assert(ptr_array_contains(p1, &free));
	assert(ptr_array_length(p1) == 1);

	value =
	ptr_array_remove_index_fast(p1, 0);
	assert (value == &free);
	assert(ptr_array_length(p1) == 0);
}

void test_get_set_index()
{
	ptr_array p1 = setup_data();
	assert(ptr_array_get_index(p1, 0) == &malloc);
	ptr_array_set_index(p1, 0, &realloc);
	assert(ptr_array_get_index(p1, 0) == &realloc);
}

void test_array_contains()
{
	ptr_array p1 = setup_data();
	assert(ptr_array_contains(p1, &malloc));
	assert(!ptr_array_contains(p1, &test_array_contains));
}

void test_array_length()
{
	ptr_array p1 = setup_data();
	assert(ptr_array_length(p1) == 4);
	ptr_array_remove_index_fast(p1, 0);
	assert(ptr_array_length(p1) == 3);
}

void test_array_append()
{
	int i;
	ptr_array p1 = ptr_array_new(0);
	for (i = 0; i < 1000; i++)
		ptr_array_append(p1, &test_array_append);
	assert(ptr_array_length(p1) == 1000);

	for (i = 0; i < 1000; i++)
		assert(p1->array[i] == &test_array_append);
}

void test_array_clear()
{
	ptr_array p1 = setup_data();
	ptr_array_clear(p1);
	assert(ptr_array_length(p1) == 0);
	ptr_array_append(p1, 0);
	assert(ptr_array_length(p1) == 1);
}
int main (int argc, char **argv)
{
	test_new();
	test_new_oom();
	test_new_zero();

	test_remove_ordered();
	test_remove_ordered_nonexistant();
	test_remove_index_ordered();

	test_remove_fast();
	test_remove_fast_nonexistant();
	test_remove_index_fast();

	test_get_set_index();
	test_array_contains();
	test_array_length();
	
	test_array_append();
	test_array_clear();
	return 0;
}
