/* This is a public domain general purpose hash table package written by Peter Moore @ UCB. */

/* @(#) st.h 5.1 89/12/14 */

#ifndef ST_H

#define ST_H

#if SIZEOF_LONG == SIZEOF_VOIDP
typedef unsigned long st_data_t;
#elif SIZEOF_LONG_LONG == SIZEOF_VOIDP
typedef unsigned LONG_LONG st_data_t;
#else
# error ---->> st.c requires sizeof(void*) == sizeof(long) to be compiled. <<---
-
#endif
#define ST_DATA_T_DEFINED

typedef struct st_table st_table;

struct st_hash_type {
    int (*compare)();
    int (*hash)();
};

struct st_table {
    struct st_hash_type *type;
    int num_bins;
    int num_entries;
    struct st_table_entry **bins;
};

#define st_is_member(table,key) st_lookup(table,key,(st_data_t *)0)

enum st_retval {ST_CONTINUE = 0, ST_STOP = 1, ST_DELETE, ST_CHECK};

// This is st.c, stuck in here, and with all the functions set to static.

/* This is a public domain general purpose hash table package written by Peter Moore @ UCB. */

/* static	char	sccsid[] = "@(#) st.c 5.1 89/12/14 Crucible"; */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct st_table_entry st_table_entry;

struct st_table_entry {
    unsigned int hash;
    st_data_t key;
    st_data_t record;
    st_table_entry *next;
};

#define ST_DEFAULT_MAX_DENSITY 5
#define ST_DEFAULT_INIT_TABLE_SIZE 11

    /*
     * DEFAULT_MAX_DENSITY is the default for the largest we allow the
     * average number of items per bin before increasing the number of
     * bins
     *
     * DEFAULT_INIT_TABLE_SIZE is the default for the number of bins
     * allocated initially
     *
     */
static int st_internal_numcmp(long, long);
static int st_internal_numhash(long);
static struct st_hash_type type_st_internal_numhash = {
    st_internal_numcmp,
    st_internal_numhash,
};

/* extern int strcmp(const char *, const char *); */
static int st_internal_strhash(const char *);
static struct st_hash_type type_st_internal_strhash = {
    strcmp,
    st_internal_strhash,
};

static void st_internal_rehash(st_table *);

#define st_internal_alloc(type) (type*)xmalloc((unsigned)sizeof(type))
#define st_internal_calloc(n,s) (char*)xcalloc((n),(s))

#define ST_EQUAL(table,x,y) ((x)==(y) || (*table->type->compare)((x),(y)) == 0)

#define st_do_hash(key,table) (unsigned int)(*(table)->type->hash)((key))
#define st_do_hash_bin(key,table) (st_do_hash(key, table)%(table)->num_bins)

/*
 * ST_MINSIZE is the minimum size of a dictionary.
 */

#define ST_MINSIZE 8

/*
Table of prime numbers 2^n+a, 2<=n<=30.
*/
static long st_internal_primes[] = {
	8 + 3,
	16 + 3,
	32 + 5,
	64 + 3,
	128 + 3,
	256 + 27,
	512 + 9,
	1024 + 9,
	2048 + 5,
	4096 + 3,
	8192 + 27,
	16384 + 43,
	32768 + 3,
	65536 + 45,
	131072 + 29,
	262144 + 3,
	524288 + 21,
	1048576 + 7,
	2097152 + 17,
	4194304 + 15,
	8388608 + 9,
	16777216 + 43,
	33554432 + 35,
	67108864 + 15,
	134217728 + 29,
	268435456 + 3,
	536870912 + 11,
	1073741824 + 85,
	0
};

static int
st_internal_new_size(size)
    int size;
{
    int i;

    int newsize;

    for (i = 0, newsize = ST_MINSIZE;
	 i < sizeof(st_internal_primes)/sizeof(st_internal_primes[0]);
	 i++, newsize <<= 1)
    {
	if (newsize > size) return st_internal_primes[i];
    }
    /* Ran out of polynomials */
    return -1;			/* should raise exception */
}

static st_table*
st_init_table_with_size(type, size)
    struct st_hash_type *type;
    int size;
{
    st_table *tbl;

    size = st_internal_new_size(size);	/* round up to prime number */

    tbl = st_internal_alloc(st_table);
    tbl->type = type;
    tbl->num_entries = 0;
    tbl->num_bins = size;
    tbl->bins = (st_table_entry **)st_internal_calloc(size, sizeof(st_table_entry*));

    return tbl;
}

static st_table*
st_init_table(type)
    struct st_hash_type *type;
{
    return st_init_table_with_size(type, 0);
}

static st_table*
st_init_numtable(void)
{
    return st_init_table(&type_st_internal_numhash);
}

static st_table*
st_init_numtable_with_size(size)
    int size;
{
    return st_init_table_with_size(&type_st_internal_numhash, size);
}

static st_table*
st_init_strtable(void)
{
    return st_init_table(&type_st_internal_strhash);
}

static st_table*
st_init_strtable_with_size(size)
    int size;
{
    return st_init_table_with_size(&type_st_internal_strhash, size);
}

static void
st_free_table(table)
    st_table *table;
{
    register st_table_entry *ptr, *next;
    int i;

    for(i = 0; i < table->num_bins; i++) {
	ptr = table->bins[i];
	while (ptr != 0) {
	    next = ptr->next;
	    free(ptr);
	    ptr = next;
	}
    }
    free(table->bins);
    free(table);
}

#define ST_PTR_NOT_EQUAL(table, ptr, hash_val, key) \
((ptr) != 0 && (ptr->hash != (hash_val) || !ST_EQUAL((table), (key), (ptr)->key)))

#define ST_FIND_ENTRY(table, ptr, hash_val, bin_pos) do {\
    bin_pos = hash_val%(table)->num_bins;\
    ptr = (table)->bins[bin_pos];\
    if (ST_PTR_NOT_EQUAL(table, ptr, hash_val, key)) {\
	while (ST_PTR_NOT_EQUAL(table, ptr->next, hash_val, key)) {\
	    ptr = ptr->next;\
	}\
	ptr = ptr->next;\
    }\
} while (0)

static int
st_lookup(table, key, value)
    st_table *table;
    register st_data_t key;
    st_data_t *value;
{
    unsigned int hash_val, bin_pos;
    register st_table_entry *ptr;

    hash_val = st_do_hash(key, table);
    ST_FIND_ENTRY(table, ptr, hash_val, bin_pos);

    if (ptr == 0) {
	return 0;
    }
    else {
	if (value != 0)  *value = ptr->record;
	return 1;
    }
}

#define ST_ADD_DIRECT(table, key, value, hash_val, bin_pos)\
do {\
    st_table_entry *entry;\
    if (table->num_entries/(table->num_bins) > ST_DEFAULT_MAX_DENSITY) {\
	st_internal_rehash(table);\
        bin_pos = hash_val % table->num_bins;\
    }\
    \
    entry = st_internal_alloc(st_table_entry);\
    \
    entry->hash = hash_val;\
    entry->key = key;\
    entry->record = value;\
    entry->next = table->bins[bin_pos];\
    table->bins[bin_pos] = entry;\
    table->num_entries++;\
} while (0)

static int
st_insert(table, key, value)
    register st_table *table;
    register st_data_t key;
    st_data_t value;
{
    unsigned int hash_val, bin_pos;
    register st_table_entry *ptr;

    hash_val = st_do_hash(key, table);
    ST_FIND_ENTRY(table, ptr, hash_val, bin_pos);

    if (ptr == 0) {
	ST_ADD_DIRECT(table, key, value, hash_val, bin_pos);
	return 0;
    }
    else {
	ptr->record = value;
	return 1;
    }
}

static void
st_add_direct(table, key, value)
    st_table *table;
    st_data_t key;
    st_data_t value;
{
    unsigned int hash_val, bin_pos;

    hash_val = st_do_hash(key, table);
    bin_pos = hash_val % table->num_bins;
    ST_ADD_DIRECT(table, key, value, hash_val, bin_pos);
}

static void
st_internal_rehash(table)
    register st_table *table;
{
    register st_table_entry *ptr, *next, **new_bins;
    int i, old_num_bins = table->num_bins, new_num_bins;
    unsigned int hash_val;

    new_num_bins = st_internal_new_size(old_num_bins+1);
    new_bins = (st_table_entry**)st_internal_calloc(new_num_bins, sizeof(st_table_entry*));

    for(i = 0; i < old_num_bins; i++) {
	ptr = table->bins[i];
	while (ptr != 0) {
	    next = ptr->next;
	    hash_val = ptr->hash % new_num_bins;
	    ptr->next = new_bins[hash_val];
	    new_bins[hash_val] = ptr;
	    ptr = next;
	}
    }
    free(table->bins);
    table->num_bins = new_num_bins;
    table->bins = new_bins;
}

static st_table*
st_copy(old_table)
    st_table *old_table;
{
    st_table *new_table;
    st_table_entry *ptr, *entry;
    int i, num_bins = old_table->num_bins;

    new_table = st_internal_alloc(st_table);
    if (new_table == 0) {
	return 0;
    }

    *new_table = *old_table;
    new_table->bins = (st_table_entry**)
	st_internal_calloc((unsigned)num_bins, sizeof(st_table_entry*));

    if (new_table->bins == 0) {
	free(new_table);
	return 0;
    }

    for(i = 0; i < num_bins; i++) {
	new_table->bins[i] = 0;
	ptr = old_table->bins[i];
	while (ptr != 0) {
	    entry = st_internal_alloc(st_table_entry);
	    if (entry == 0) {
		free(new_table->bins);
		free(new_table);
		return 0;
	    }
	    *entry = *ptr;
	    entry->next = new_table->bins[i];
	    new_table->bins[i] = entry;
	    ptr = ptr->next;
	}
    }
    return new_table;
}

static int
st_delete(table, key, value)
    register st_table *table;
    register st_data_t *key;
    st_data_t *value;
{
    unsigned int hash_val;
    st_table_entry *tmp;
    register st_table_entry *ptr;

    hash_val = st_do_hash_bin(*key, table);
    ptr = table->bins[hash_val];

    if (ptr == 0) {
	if (value != 0) *value = 0;
	return 0;
    }

    if (ST_EQUAL(table, *key, ptr->key)) {
	table->bins[hash_val] = ptr->next;
	table->num_entries--;
	if (value != 0) *value = ptr->record;
	*key = ptr->key;
	free(ptr);
	return 1;
    }

    for(; ptr->next != 0; ptr = ptr->next) {
	if (ST_EQUAL(table, ptr->next->key, *key)) {
	    tmp = ptr->next;
	    ptr->next = ptr->next->next;
	    table->num_entries--;
	    if (value != 0) *value = tmp->record;
	    *key = tmp->key;
	    free(tmp);
	    return 1;
	}
    }

    return 0;
}

static int
st_delete_safe(table, key, value, never)
    register st_table *table;
    register st_data_t *key;
    st_data_t *value;
    st_data_t never;
{
    unsigned int hash_val;
    register st_table_entry *ptr;

    hash_val = st_do_hash_bin(*key, table);
    ptr = table->bins[hash_val];

    if (ptr == 0) {
	if (value != 0) *value = 0;
	return 0;
    }

    for(; ptr != 0; ptr = ptr->next) {
	if ((ptr->key != never) && ST_EQUAL(table, ptr->key, *key)) {
	    table->num_entries--;
	    *key = ptr->key;
	    if (value != 0) *value = ptr->record;
	    ptr->key = ptr->record = never;
	    return 1;
	}
    }

    return 0;
}

static int
st_internal_delete_never(key, value, never)
    st_data_t key, value, never;
{
    if (value == never) return ST_DELETE;
    return ST_CONTINUE;
}

static int
st_foreach(table, func, arg)
    st_table *table;
    int (*func)();
    st_data_t arg;
{
    st_table_entry *ptr, *last, *tmp;
    enum st_retval retval;
    int i;

    for(i = 0; i < table->num_bins; i++) {
	last = 0;
	for(ptr = table->bins[i]; ptr != 0;) {
	    retval = (*func)(ptr->key, ptr->record, arg);
	    switch (retval) {
	    case ST_CHECK:	/* check if hash is modified during iteration */
	        tmp = 0;
		if (i < table->num_bins) {
		    for (tmp = table->bins[i]; tmp; tmp=tmp->next) {
			if (tmp == ptr) break;
		    }
		}
		if (!tmp) {
		    /* call func with error notice */
		    return 1;
		}
		/* fall through */
	    case ST_CONTINUE:
		last = ptr;
		ptr = ptr->next;
		break;
	    case ST_STOP:
	        return 0;
	    case ST_DELETE:
		tmp = ptr;
		if (last == 0) {
		    table->bins[i] = ptr->next;
		}
		else {
		    last->next = ptr->next;
		}
		ptr = ptr->next;
		free(tmp);
		table->num_entries--;
	    }
	}
    }
    return 0;
}

static void
st_cleanup_safe(table, never)
    st_table *table;
    st_data_t never;
{
    int num_entries = table->num_entries;

    st_foreach(table, st_internal_delete_never, never);
    table->num_entries = num_entries;
}


static int
st_internal_strhash(string)
    register const char *string;
{
    register int c;

    register int val = 0;

    while ((c = *string++) != '\0') {
	val = val*997 + c;
    }

    return val + (val>>5);
}

static int
st_internal_numcmp(x, y)
    long x, y;
{
    return x != y;
}

static int
st_internal_numhash(n)
    long n;
{
    return n;
}

#define ST_NUMCMP	((int (*)()) 0)
#define ST_NUMHASH	((int (*)()) -2)

#define st_numcmp	ST_NUMCMP
#define st_numhash	ST_NUMHASH

#endif /* ST_INCLUDED */
