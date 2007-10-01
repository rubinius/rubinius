/* Copyright (C) 2002, 2004 Christopher Clark <firstname.lastname@cl.cam.ac.uk> */

#include "hashtable.h"
#include "hashtable_itr.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h> /* for memcmp */

static const int ITEM_COUNT = 4000;

typedef unsigned int uint32_t;
typedef unsigned short uint16_t;

/*****************************************************************************/
struct key
{
    uint32_t one_ip; uint32_t two_ip; uint16_t one_port; uint16_t two_port;
};

struct value
{
    char *id;
};

DEFINE_HASHTABLE_INSERT(insert_some, struct key, struct value);
DEFINE_HASHTABLE_SEARCH(search_some, struct key, struct value);
DEFINE_HASHTABLE_REMOVE(remove_some, struct key, struct value);
DEFINE_HASHTABLE_ITERATOR_SEARCH(search_itr_some, struct key);


/*****************************************************************************/
static unsigned int
hashfromkey(void *ky)
{
    struct key *k = (struct key *)ky;
    return (((k->one_ip << 17) | (k->one_ip >> 15)) ^ k->two_ip) +
            (k->one_port * 17) + (k->two_port * 13 * 29);
}

static int
equalkeys(void *k1, void *k2)
{
    return (0 == memcmp(k1,k2,sizeof(struct key)));
}

/*****************************************************************************/
int
main(int argc, char **argv)
{
    struct key *k, *kk;
    struct value *v, *found;
    struct hashtable *h;
    struct hashtable_itr *itr;
    int i;

    h = create_hashtable(16, hashfromkey, equalkeys);
    if (NULL == h) exit(-1); /*oom*/


/*****************************************************************************/
/* Insertion */
    for (i = 0; i < ITEM_COUNT; i++)
    {
        k = (struct key *)malloc(sizeof(struct key));
        if (NULL == k) {
            printf("ran out of memory allocating a key\n");
            return 1;
        }
        k->one_ip = 0xcfccee40 + i;
        k->two_ip = 0xcf0cee67 - (5 * i);
        k->one_port = 22 + (7 * i);
        k->two_port = 5522 - (3 * i);
        
        v = (struct value *)malloc(sizeof(struct value));
        v->id = "a value";
        
        if (!insert_some(h,k,v)) exit(-1); /*oom*/
    }
    printf("After insertion, hashtable contains %u items.\n",
            hashtable_count(h));

/*****************************************************************************/
/* Hashtable search */
    k = (struct key *)malloc(sizeof(struct key));
    if (NULL == k) {
        printf("ran out of memory allocating a key\n");
        return 1;
    }
    
    for (i = 0; i < ITEM_COUNT; i++)
    {
        k->one_ip = 0xcfccee40 + i;
        k->two_ip = 0xcf0cee67 - (5 * i);
        k->one_port = 22 + (7 * i);
        k->two_port = 5522 - (3 * i);
        
        if (NULL == (found = search_some(h,k))) {
            printf("BUG: key not found\n");
        }
    }

/*****************************************************************************/
/* Hashtable iteration */
    /* Iterator constructor only returns a valid iterator if
     * the hashtable is not empty */
    itr = hashtable_iterator(h);
    i = 0;
    if (hashtable_count(h) > 0)
    {
        do {
            kk = hashtable_iterator_key(itr);
            v = hashtable_iterator_value(itr);
            /* here (kk,v) are a valid (key, value) pair */
            /* We could call 'hashtable_remove(h,kk)' - and this operation
             * 'free's kk. However, the iterator is then broken.
             * This is why hashtable_iterator_remove exists - see below.
             */
            i++;

        } while (hashtable_iterator_advance(itr));
    }
    printf("Iterated through %u entries.\n", i);

/*****************************************************************************/
/* Hashtable iterator search */

    /* Try the search some method */
    for (i = 0; i < ITEM_COUNT; i++)
    {
        k->one_ip = 0xcfccee40 + i;
        k->two_ip = 0xcf0cee67 - (5 * i);
        k->one_port = 22 + (7 * i);
        k->two_port = 5522 - (3 * i);
        
        if (0 == search_itr_some(itr,h,k)) {
            printf("BUG: key not found searching with iterator");
        }
    }

/*****************************************************************************/
/* Hashtable removal */

    for (i = 0; i < ITEM_COUNT; i++)
    {
        k->one_ip = 0xcfccee40 + i;
        k->two_ip = 0xcf0cee67 - (5 * i);
        k->one_port = 22 + (7 * i);
        k->two_port = 5522 - (3 * i);
        
        if (NULL == (found = remove_some(h,k))) {
            printf("BUG: key not found for removal\n");
        }
    }
    printf("After removal, hashtable contains %u items.\n",
            hashtable_count(h));

/*****************************************************************************/
/* Hashtable destroy and create */

    hashtable_destroy(h, 1);
    h = NULL;
    free(k);

    h = create_hashtable(160, hashfromkey, equalkeys);
    if (NULL == h) {
        printf("out of memory allocating second hashtable\n");
        return 1;
    }

/*****************************************************************************/
/* Hashtable insertion */

    for (i = 0; i < ITEM_COUNT; i++)
    {
        k = (struct key *)malloc(sizeof(struct key));
        k->one_ip = 0xcfccee40 + i;
        k->two_ip = 0xcf0cee67 - (5 * i);
        k->one_port = 22 + (7 * i);
        k->two_port = 5522 - (3 * i);
        
        v = (struct value *)malloc(sizeof(struct value));
        v->id = "a value";
        
        if (!insert_some(h,k,v))
        {
            printf("out of memory inserting into second hashtable\n");
            return 1;
        }
    }
    printf("After insertion, hashtable contains %u items.\n",
            hashtable_count(h));

/*****************************************************************************/
/* Hashtable iterator search and iterator remove */

    k = (struct key *)malloc(sizeof(struct key));
    if (NULL == k) {
        printf("ran out of memory allocating a key\n");
        return 1;
    }
    
    for (i = ITEM_COUNT - 1; i >= 0; i = i - 7)
    {
        k->one_ip = 0xcfccee40 + i;
        k->two_ip = 0xcf0cee67 - (5 * i);
        k->one_port = 22 + (7 * i);
        k->two_port = 5522 - (3 * i);
        
        if (0 == search_itr_some(itr, h, k)) {
            printf("BUG: key %u not found for search preremoval using iterator\n", i);
            return 1;
        }
        if (0 == hashtable_iterator_remove(itr)) {
            printf("BUG: key not found for removal using iterator\n");
            return 1;
        }
    }
    free(itr);

/*****************************************************************************/
/* Hashtable iterator remove and advance */

    for (itr = hashtable_iterator(h);
         hashtable_iterator_remove(itr) != 0; ) {
        ;
    }
    free(itr);
    printf("After removal, hashtable contains %u items.\n",
            hashtable_count(h));

/*****************************************************************************/
/* Hashtable destroy */

    hashtable_destroy(h, 1);
    free(k);
    return 0;
}

/*
 * Copyright (c) 2002, 2004, Christopher Clark
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 
 * * Neither the name of the original author; nor the names of any contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 * 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
