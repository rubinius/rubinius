/* This is a public domain general purpose hash table package written by Peter Moore @ UCB. */

/* @(#) st.h 5.1 89/12/14 */

#ifndef RUBY_ST_H
#define RUBY_ST_H 1

#if defined(__cplusplus)
extern "C" {
#if 0
} /* satisfy cc-mode */
#endif
#endif

#include "ruby.h"

#if defined __GNUC__ && __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

#if SIZEOF_LONG == SIZEOF_VOIDP
typedef unsigned long st_data_t;
#elif SIZEOF_LONG_LONG == SIZEOF_VOIDP
typedef unsigned LONG_LONG st_data_t;
#else
# error ---->> st.c requires sizeof(void*) == sizeof(long) to be compiled. <<----
#endif
#define ST_DATA_T_DEFINED

#ifndef CHAR_BIT
# ifdef HAVE_LIMITS_H
#  include <limits.h>
# else
#  define CHAR_BIT 8
# endif
#endif

typedef struct st_table st_table;

typedef st_data_t st_index_t;
typedef int st_compare_func(st_data_t, st_data_t);
typedef st_index_t st_hash_func(st_data_t);

typedef char st_check_for_sizeof_st_index_t[SIZEOF_VOIDP == (int)sizeof(st_index_t) ? 1 : -1];
#define SIZEOF_ST_INDEX_T SIZEOF_VOIDP

struct st_hash_type {
    int (*compare)(ANYARGS /*st_data_t, st_data_t*/); /* st_compare_func* */
    st_index_t (*hash)(ANYARGS /*st_data_t*/);        /* st_hash_func* */
};

#define ST_INDEX_BITS (sizeof(st_index_t) * CHAR_BIT)

struct st_table {
    const struct st_hash_type *type;
    st_index_t num_bins;
    unsigned int entries_packed : 1;
#ifdef __GNUC__
    /*
     * C spec says,
     *   A bit-field shall have a type that is a qualified or unqualified
     *   version of _Bool, signed int, unsigned int, or some other
     *   implementation-defined type. It is implementation-defined whether
     *   atomic types are permitted.
     * In short, long and long long bit-field are implementation-defined
     * feature. Therefore we want to supress a warning explicitly.
     */
    __extension__
#endif
    st_index_t num_entries : ST_INDEX_BITS - 1;
    struct st_table_entry **bins;
    struct st_table_entry *head, *tail;
};

#define st_is_member(table,key) st_lookup((table),(key),(st_data_t *)0)

enum st_retval {ST_CONTINUE = 0, ST_STOP = 1, ST_DELETE = 2, ST_CHECK};


/* This is st.c, stuck in here, and with all the functions set to static. */


/* This is a public domain general purpose hash table package written by Peter Moore @ UCB. */

/* static char  sccsid[] = "@(#) st.c 5.1 89/12/14 Crucible"; */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct st_table_entry st_table_entry;

struct st_table_entry {
    st_index_t hash;
    st_data_t key;
    st_data_t record;
    st_table_entry *next;
    st_table_entry *fore, *back;
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

static int st_numcmp(st_data_t, st_data_t);
static st_index_t st_numhash(st_data_t);
static const struct st_hash_type type_numhash = {
    (int (*)(ANYARGS))st_numcmp,
    (st_index_t (*)(ANYARGS))st_numhash,
};

/* extern int strcmp(const char *, const char *); */
static st_index_t st_internal_strhash(st_data_t);
static const struct st_hash_type type_st_internal_strhash = {
    (int (*)(ANYARGS))strcmp,
    (st_index_t (*)(ANYARGS))st_internal_strhash,
};

static int st_strcasecmp(const char *, const char *);
static st_index_t st_internal_strcasehash(st_data_t);
static const struct st_hash_type type_st_internal_strcasehash = {
    (int (*)(ANYARGS))st_strcasecmp,
    (st_index_t (*)(ANYARGS))st_internal_strcasehash,
};

static void st_internal_rehash(st_table *);
static int st_insert(register st_table *, register st_data_t, st_data_t);

#define st_internal_numberof(array) (int)(sizeof(array) / sizeof((array)[0]))

#define st_internal_alloc(type) (type*)xmalloc((size_t)sizeof(type))
#define st_internal_calloc(n,s) (char*)xcalloc((n),(s))

#define ST_EQUAL(table,x,y) ((x)==(y) || (*(table)->type->compare)((x),(y)) == 0)

/* remove cast to unsigned int in the future */
#define st_do_hash(key,table) (unsigned int)(st_index_t)(*(table)->type->hash)((key))
#define st_do_hash_bin(key,table) (st_do_hash((key), (table))%(table)->num_bins)

/*
 * ST_MINSIZE is the minimum size of a dictionary.
 */

#define ST_MINSIZE 8

/*
Table of prime numbers 2^n+a, 2<=n<=30.
*/
static const unsigned int st_internal_primes[] = {
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

static st_index_t
st_internal_new_size(st_index_t size)
{
    int i;

    st_index_t newsize;

    for (i = 0, newsize = ST_MINSIZE; i < st_internal_numberof(st_internal_primes); i++, newsize <<= 1) {
        if (newsize > size) return st_internal_primes[i];
    }
    /* Ran out of polynomials */
    rb_raise(rb_eRuntimeError, "st_table too big");
    return -1;      /* should raise exception */
}

#define ST_MAX_PACKED_NUMHASH (ST_DEFAULT_INIT_TABLE_SIZE/2)

static st_table*
st_init_table_with_size(const struct st_hash_type *type, st_index_t size)
{
    st_table *tbl;

    size = st_internal_new_size(size);  /* round up to prime number */

    tbl = st_internal_alloc(st_table);
    tbl->type = type;
    tbl->num_entries = 0;
    tbl->entries_packed = type == &type_numhash && size/2 <= ST_MAX_PACKED_NUMHASH;
    tbl->num_bins = size;
    tbl->bins = (st_table_entry **)st_internal_calloc(size, sizeof(st_table_entry*));
    tbl->head = 0;
    tbl->tail = 0;

    return tbl;
}

static st_table*
st_init_table(const struct st_hash_type *type)
{
    return st_init_table_with_size(type, 0);
}

static st_table*
st_init_numtable(void)
{
    return st_init_table(&type_numhash);
}

static st_table*
st_init_numtable_with_size(st_index_t size)
{
    return st_init_table_with_size(&type_numhash, size);
}

static st_table*
st_init_strtable(void)
{
    return st_init_table(&type_st_internal_strhash);
}

static st_table*
st_init_strtable_with_size(st_index_t size)
{
    return st_init_table_with_size(&type_st_internal_strhash, size);
}

static st_table*
st_init_strcasetable(void)
{
    return st_init_table(&type_st_internal_strcasehash);
}

static st_table*
st_init_strcasetable_with_size(st_index_t size)
{
    return st_init_table_with_size(&type_st_internal_strcasehash, size);
}

static void
st_clear(st_table *table)
{
    register st_table_entry *ptr, *next;
    st_index_t i;

    if (table->entries_packed) {
        table->num_entries = 0;
        return;
    }

    for(i = 0; i < table->num_bins; i++) {
        ptr = table->bins[i];
        table->bins[i] = 0;
        while (ptr != 0) {
            next = ptr->next;
            free(ptr);
            ptr = next;
        }
    }
    table->num_entries = 0;
    table->head = 0;
    table->tail = 0;
}

static void
st_free_table(st_table *table)
{
    st_clear(table);
    free(table->bins);
    free(table);
}

static size_t
st_memsize(const st_table *table)
{
    if (table->entries_packed) {
        return table->num_bins * sizeof (void *) + sizeof(st_table);
    }
    else {
        return table->num_entries * sizeof(struct st_table_entry) + table->num_bins * sizeof (void *) + sizeof(st_table);
    }
}

#define ST_PTR_NOT_EQUAL(table, ptr, hash_val, key) \
((ptr) != 0 && ((ptr)->hash != (hash_val) || !ST_EQUAL((table), (key), (ptr)->key)))

#define ST_FIND_ENTRY(table, ptr, hash_val, bin_pos) do {\
    (bin_pos) = (hash_val)%(table)->num_bins;\
    (ptr) = (table)->bins[(bin_pos)];\
    if (ST_PTR_NOT_EQUAL((table), (ptr), (hash_val), key)) {\
        while (ST_PTR_NOT_EQUAL((table), (ptr)->next, (hash_val), key)) {\
            (ptr) = (ptr)->next;\
        }\
        (ptr) = (ptr)->next;\
    }\
} while (0)

static int
st_lookup(st_table *table, register st_data_t key, st_data_t *value)
{
    st_index_t hash_val, bin_pos;
    register st_table_entry *ptr;

    if (table->entries_packed) {
        st_index_t i;
        for (i = 0; i < table->num_entries; i++) {
            if ((st_data_t)table->bins[i*2] == key) {
                if (value !=0) *value = (st_data_t)table->bins[i*2+1];
                return 1;
            }
        }
        return 0;
    }

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

static int
st_get_key(st_table *table, register st_data_t key, st_data_t *result)
{
    st_index_t hash_val, bin_pos;
    register st_table_entry *ptr;

    if (table->entries_packed) {
        st_index_t i;
        for (i = 0; i < table->num_entries; i++) {
            if ((st_data_t)table->bins[i*2] == key) {
                if (result !=0) *result = (st_data_t)table->bins[i*2];
                return 1;
            }
        }
        return 0;
    }

    hash_val = st_do_hash(key, table);
    ST_FIND_ENTRY(table, ptr, hash_val, bin_pos);

    if (ptr == 0) {
        return 0;
    }
    else {
        if (result != 0)  *result = ptr->key;
        return 1;
    }
}

#define ST_MORE_PACKABLE_P(table) \
    ((st_index_t)((table)->num_entries+1) * 2 <= (table)->num_bins && \
     (table)->num_entries+1 <= ST_MAX_PACKED_NUMHASH)

#define ST_ADD_DIRECT(table, key, value, hash_val, bin_pos)\
do {\
    st_table_entry *entry;\
    if ((table)->num_entries > ST_DEFAULT_MAX_DENSITY * (table)->num_bins) {\
        st_internal_rehash(table);\
        (bin_pos) = (hash_val) % (table)->num_bins;\
    }\
    \
    entry = st_internal_alloc(st_table_entry);\
    \
    entry->hash = (hash_val);\
    entry->key = (key);\
    entry->record = (value);\
    entry->next = (table)->bins[(bin_pos)];\
    if ((table)->head != 0) {\
        entry->fore = 0;\
        (entry->back = (table)->tail)->fore = entry;\
        (table)->tail = entry;\
    }\
    else {\
        (table)->head = (table)->tail = entry;\
        entry->fore = entry->back = 0;\
    }\
    (table)->bins[(bin_pos)] = entry;\
    (table)->num_entries++;\
} while (0)

static void
st_internal_unpack_entries(register st_table *table)
{
    st_index_t i;
    struct st_table_entry *packed_bins[ST_MAX_PACKED_NUMHASH*2];
    st_table tmp_table = *table;

    memcpy(packed_bins, table->bins, sizeof(struct st_table_entry *) * table->num_entries*2);
    table->bins = packed_bins;
    tmp_table.entries_packed = 0;
    tmp_table.num_entries = 0;
    memset(tmp_table.bins, 0, sizeof(struct st_table_entry *) * tmp_table.num_bins);
    for (i = 0; i < table->num_entries; i++) {
        st_insert(&tmp_table, (st_data_t)packed_bins[i*2], (st_data_t)packed_bins[i*2+1]);
    }
    *table = tmp_table;
}

static int
st_insert(register st_table *table, register st_data_t key, st_data_t value)
{
    st_index_t hash_val, bin_pos;
    register st_table_entry *ptr;

    if (table->entries_packed) {
        st_index_t i;
        for (i = 0; i < table->num_entries; i++) {
            if ((st_data_t)table->bins[i*2] == key) {
                table->bins[i*2+1] = (struct st_table_entry*)value;
                return 1;
            }
        }
        if (ST_MORE_PACKABLE_P(table)) {
            i = table->num_entries++;
            table->bins[i*2] = (struct st_table_entry*)key;
            table->bins[i*2+1] = (struct st_table_entry*)value;
            return 0;
        }
        else {
            st_internal_unpack_entries(table);
        }
    }

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

static int
st_insert2(register st_table *table, register st_data_t key, st_data_t value,
     st_data_t (*func)(st_data_t))
{
    st_index_t hash_val, bin_pos;
    register st_table_entry *ptr;

    if (table->entries_packed) {
        st_index_t i;
        for (i = 0; i < table->num_entries; i++) {
            if ((st_data_t)table->bins[i*2] == key) {
                table->bins[i*2+1] = (struct st_table_entry*)value;
                return 1;
            }
        }
        if (ST_MORE_PACKABLE_P(table)) {
            i = table->num_entries++;
            table->bins[i*2] = (struct st_table_entry*)key;
            table->bins[i*2+1] = (struct st_table_entry*)value;
            return 0;
        }
        else {
            st_internal_unpack_entries(table);
        }
    }

    hash_val = st_do_hash(key, table);
    ST_FIND_ENTRY(table, ptr, hash_val, bin_pos);

    if (ptr == 0) {
        key = (*func)(key);
        ST_ADD_DIRECT(table, key, value, hash_val, bin_pos);
        return 0;
    }
    else {
        ptr->record = value;
        return 1;
    }
}

static void
st_add_direct(st_table *table, st_data_t key, st_data_t value)
{
    st_index_t hash_val, bin_pos;

    if (table->entries_packed) {
        int i;
        if (ST_MORE_PACKABLE_P(table)) {
            i = table->num_entries++;
            table->bins[i*2] = (struct st_table_entry*)key;
            table->bins[i*2+1] = (struct st_table_entry*)value;
            return;
        }
        else {
            st_internal_unpack_entries(table);
        }
    }

    hash_val = st_do_hash(key, table);
    bin_pos = hash_val % table->num_bins;
    ST_ADD_DIRECT(table, key, value, hash_val, bin_pos);
}

static void
st_internal_rehash(register st_table *table)
{
    register st_table_entry *ptr, **new_bins;
    st_index_t i, new_num_bins, hash_val;

    new_num_bins = st_internal_new_size(table->num_bins+1);
    new_bins = (st_table_entry**)
        xrealloc(table->bins, new_num_bins * sizeof(st_table_entry*));
    for (i = 0; i < new_num_bins; ++i) new_bins[i] = 0;
    table->num_bins = new_num_bins;
    table->bins = new_bins;

    if ((ptr = table->head) != 0) {
        do {
            hash_val = ptr->hash % new_num_bins;
            ptr->next = new_bins[hash_val];
            new_bins[hash_val] = ptr;
        } while ((ptr = ptr->fore) != 0);
    }
}

static st_table*
st_copy(st_table *old_table)
{
    st_table *new_table;
    st_table_entry *ptr, *entry, *prev, **tail;
    st_index_t num_bins = old_table->num_bins;
    st_index_t hash_val;

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

    if (old_table->entries_packed) {
        memcpy(new_table->bins, old_table->bins, sizeof(struct st_table_entry *) * old_table->num_bins);
        return new_table;
    }

    if ((ptr = old_table->head) != 0) {
        prev = 0;
        tail = &new_table->head;
        do {
            entry = st_internal_alloc(st_table_entry);
            if (entry == 0) {
                st_free_table(new_table);
                return 0;
            }
            *entry = *ptr;
            hash_val = entry->hash % num_bins;
            entry->next = new_table->bins[hash_val];
            new_table->bins[hash_val] = entry;
            entry->back = prev;
            *tail = prev = entry;
            tail = &entry->fore;
        } while ((ptr = ptr->fore) != 0);
        new_table->tail = prev;
    }

    return new_table;
}

#define ST_REMOVE_ENTRY(table, ptr) do         \
    {                 \
        if ((ptr)->fore == 0 && (ptr)->back == 0) {     \
            (table)->head = 0;            \
            (table)->tail = 0;            \
        }               \
        else {                \
            st_table_entry *fore = (ptr)->fore, *back = (ptr)->back;  \
            if (fore) fore->back = back;        \
            if (back) back->fore = fore;        \
            if ((ptr) == (table)->head) (table)->head = fore;   \
            if ((ptr) == (table)->tail) (table)->tail = back;   \
        }               \
        (table)->num_entries--;           \
    } while (0)

static int
st_delete(register st_table *table, register st_data_t *key, st_data_t *value)
{
    st_index_t hash_val;
    st_table_entry **prev;
    register st_table_entry *ptr;

    if (table->entries_packed) {
        st_index_t i;
        for (i = 0; i < table->num_entries; i++) {
            if ((st_data_t)table->bins[i*2] == *key) {
                if (value != 0) *value = (st_data_t)table->bins[i*2+1];
                table->num_entries--;
                memmove(&table->bins[i*2], &table->bins[(i+1)*2],
                        sizeof(struct st_table_entry*) * 2*(table->num_entries-i));
                return 1;
            }
        }
        if (value != 0) *value = 0;
        return 0;
    }

    hash_val = st_do_hash_bin(*key, table);

    for (prev = &table->bins[hash_val]; (ptr = *prev) != 0; prev = &ptr->next) {
        if (ST_EQUAL(table, *key, ptr->key)) {
            *prev = ptr->next;
            ST_REMOVE_ENTRY(table, ptr);
            if (value != 0) *value = ptr->record;
            *key = ptr->key;
            free(ptr);
            return 1;
        }
    }

    if (value != 0) *value = 0;
    return 0;
}

static int
st_delete_safe(register st_table *table, register st_data_t *key, st_data_t *value, st_data_t never)
{
    st_index_t hash_val;
    register st_table_entry *ptr;

    if (table->entries_packed) {
        st_index_t i;
        for (i = 0; i < table->num_entries; i++) {
            if ((st_data_t)table->bins[i*2] == *key) {
                if (value != 0) *value = (st_data_t)table->bins[i*2+1];
                table->bins[i*2] = (st_table_entry *)never;
                return 1;
            }
        }
        if (value != 0) *value = 0;
        return 0;
    }

    hash_val = st_do_hash_bin(*key, table);
    ptr = table->bins[hash_val];

    for (; ptr != 0; ptr = ptr->next) {
        if ((ptr->key != never) && ST_EQUAL(table, ptr->key, *key)) {
            ST_REMOVE_ENTRY(table, ptr);
            *key = ptr->key;
            if (value != 0) *value = ptr->record;
            ptr->key = ptr->record = never;
            return 1;
        }
    }

    if (value != 0) *value = 0;
    return 0;
}

static void
st_cleanup_safe(st_table *table, st_data_t never)
{
    st_table_entry *ptr, **last, *tmp;
    st_index_t i;

    if (table->entries_packed) {
        st_index_t i = 0, j = 0;
        while ((st_data_t)table->bins[i*2] != never) {
            if (i++ == table->num_entries) return;
        }
        for (j = i; ++i < table->num_entries;) {
            if ((st_data_t)table->bins[i*2] == never) continue;
            table->bins[j*2] = table->bins[i*2];
            table->bins[j*2+1] = table->bins[i*2+1];
            j++;
        }
        table->num_entries = j;
        return;
    }

    for (i = 0; i < table->num_bins; i++) {
        ptr = *(last = &table->bins[i]);
        while (ptr != 0) {
            if (ptr->key == never) {
                tmp = ptr;
                *last = ptr = ptr->next;
                free(tmp);
            }
            else {
                ptr = *(last = &ptr->next);
            }
        }
    }
}

static int
st_foreach(st_table *table, int (*func)(ANYARGS), st_data_t arg)
{
    st_table_entry *ptr, **last, *tmp;
    enum st_retval retval;
    st_index_t i;

    if (table->entries_packed) {
        for (i = 0; i < table->num_entries; i++) {
            st_index_t j;
            st_data_t key, val;
            key = (st_data_t)table->bins[i*2];
            val = (st_data_t)table->bins[i*2+1];
            retval = (enum st_retval)func(key, val, arg);
            if (!table->entries_packed) goto unpacked;
            switch (retval) {
              case ST_CHECK:  /* check if hash is modified during iteration */
                for (j = 0; j < table->num_entries; j++) {
                    if ((st_data_t)table->bins[j*2] == key)
                        break;
                }
                if (j == table->num_entries) {
                    /* call func with error notice */
                    retval = (enum st_retval)func(0, 0, arg, 1);
                    return 1;
                }
                /* fall through */
              case ST_CONTINUE:
                break;
              case ST_STOP:
                return 0;
              case ST_DELETE:
                table->num_entries--;
                memmove(&table->bins[i*2], &table->bins[(i+1)*2],
                        sizeof(struct st_table_entry*) * 2*(table->num_entries-i));
                i--;
                break;
            }
        }
        return 0;
      unpacked:
        ptr = table->head;
        while (i-- > 0) {
            if (!(ptr = ptr->fore)) return 0;
        }
    }
    else {
        ptr = table->head;
    }

    if (ptr != 0) {
        do {
          i = ptr->hash % table->num_bins;
          retval = (enum st_retval)func(ptr->key, ptr->record, arg);
          switch (retval) {
            case ST_CHECK:  /* check if hash is modified during iteration */
                for (tmp = table->bins[i]; tmp != ptr; tmp = tmp->next) {
                    if (!tmp) {
                        /* call func with error notice */
                        retval = (enum st_retval)func(0, 0, arg, 1);
                        return 1;
                    }
                }
            /* fall through */
            case ST_CONTINUE:
                ptr = ptr->fore;
                break;
            case ST_STOP:
                return 0;
            case ST_DELETE:
                last = &table->bins[ptr->hash % table->num_bins];
                for (; (tmp = *last) != 0; last = &tmp->next) {
                    if (ptr == tmp) {
                        tmp = ptr->fore;
                        *last = ptr->next;
                        ST_REMOVE_ENTRY(table, ptr);
                        free(ptr);
                        if (ptr == tmp) return 0;
                        ptr = tmp;
                        break;
                    }
                }
            }
        } while (ptr && table->head);
    }
    return 0;
}


/*
 * hash_32 - 32 bit Fowler/Noll/Vo FNV-1a hash code
 *
 * @(#) $Hash32: Revision: 1.1 $
 * @(#) $Hash32: Id: hash_32a.c,v 1.1 2003/10/03 20:38:53 chongo Exp $
 * @(#) $Hash32: Source: /usr/local/src/cmd/fnv/RCS/hash_32a.c,v $
 *
 ***
 *
 * Fowler/Noll/Vo hash
 *
 * The basis of this hash algorithm was taken from an idea sent
 * as reviewer comments to the IEEE POSIX P1003.2 committee by:
 *
 *      Phong Vo (http://www.research.att.com/info/kpv/)
 *      Glenn Fowler (http://www.research.att.com/~gsf/)
 *
 * In a subsequent ballot round:
 *
 *      Landon Curt Noll (http://www.isthe.com/chongo/)
 *
 * improved on their algorithm.  Some people tried this hash
 * and found that it worked rather well.  In an EMail message
 * to Landon, they named it the ``Fowler/Noll/Vo'' or FNV hash.
 *
 * FNV hashes are designed to be fast while maintaining a low
 * collision rate. The FNV speed allows one to quickly hash lots
 * of data while maintaining a reasonable collision rate.  See:
 *
 *      http://www.isthe.com/chongo/tech/comp/fnv/index.html
 *
 * for more details as well as other forms of the FNV hash.
 ***
 *
 * To use the recommended 32 bit FNV-1a hash, pass ST_FNV1_32A_INIT as the
 * Fnv32_t hashval argument to fnv_32a_buf() or fnv_32a_str().
 *
 ***
 *
 * Please do not copyright this code.  This code is in the public domain.
 *
 * LANDON CURT NOLL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL LANDON CURT NOLL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * By:
 *  chongo <Landon Curt Noll> /\oo/\
 *      http://www.isthe.com/chongo/
 *
 * Share and Enjoy! :-)
 */

/*
 * 32 bit FNV-1 and FNV-1a non-zero initial basis
 *
 * The FNV-1 initial basis is the FNV-0 hash of the following 32 octets:
 *
 *              chongo <Landon Curt Noll> /\../\
 *
 * NOTE: The \'s above are not back-slashing escape characters.
 * They are literal ASCII  backslash 0x5c characters.
 *
 * NOTE: The FNV-1a initial basis is the same value as FNV-1 by definition.
 */
#define ST_FNV1_32A_INIT 0x811c9dc5

/*
 * 32 bit magic FNV-1a prime
 */
#define ST_FNV_32_PRIME 0x01000193

static st_index_t
st_internal_strhash(st_data_t arg)
{
    register const char *string = (const char *)arg;
    register st_index_t hval = ST_FNV1_32A_INIT;

    /*
     * FNV-1a hash each octet in the buffer
     */
    while (*string) {
        /* xor the bottom with the current octet */
        hval ^= (unsigned int)*string++;

        /* multiply by the 32 bit FNV magic prime mod 2^32 */
        hval *= ST_FNV_32_PRIME;
    }
    return hval;
}

static int
st_strcasecmp(const char *s1, const char *s2)
{
    unsigned int c1, c2;

    while (1) {
        c1 = (unsigned char)*s1++;
        c2 = (unsigned char)*s2++;
        if (c1 == '\0' || c2 == '\0') {
            if (c1 != '\0') return 1;
            if (c2 != '\0') return -1;
            return 0;
        }
        if ((unsigned int)(c1 - 'A') <= ('Z' - 'A')) c1 += 'a' - 'A';
        if ((unsigned int)(c2 - 'A') <= ('Z' - 'A')) c2 += 'a' - 'A';
        if (c1 != c2) {
            if (c1 > c2)
                return 1;
            else
                return -1;
        }
    }
}

static int
st_strncasecmp(const char *s1, const char *s2, size_t n)
{
    unsigned int c1, c2;

    while (n--) {
        c1 = (unsigned char)*s1++;
        c2 = (unsigned char)*s2++;
        if (c1 == '\0' || c2 == '\0') {
            if (c1 != '\0') return 1;
            if (c2 != '\0') return -1;
            return 0;
        }
        if ((unsigned int)(c1 - 'A') <= ('Z' - 'A')) c1 += 'a' - 'A';
        if ((unsigned int)(c2 - 'A') <= ('Z' - 'A')) c2 += 'a' - 'A';
        if (c1 != c2) {
            if (c1 > c2)
                return 1;
            else
                return -1;
        }
    }
    return 0;
}

static st_index_t
st_internal_strcasehash(st_data_t arg)
{
    register const char *string = (const char *)arg;
    register st_index_t hval = ST_FNV1_32A_INIT;

    /*
     * FNV-1a hash each octet in the buffer
     */
    while (*string) {
        unsigned int c = (unsigned char)*string++;
        if ((unsigned int)(c - 'A') <= ('Z' - 'A')) c += 'a' - 'A';
        hval ^= c;

        /* multiply by the 32 bit FNV magic prime mod 2^32 */
        hval *= ST_FNV_32_PRIME;
    }
    return hval;
}

static int
st_numcmp(st_data_t x, st_data_t y)
{
    return x != y;
}

static st_index_t
st_numhash(st_data_t n)
{
    return (st_index_t)n;
}



#if defined __GNUC__ && __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#if defined(__cplusplus)
#if 0
{ /* satisfy cc-mode */
#endif
}  /* extern "C" { */
#endif

#endif /* RUBY_ST_H */
