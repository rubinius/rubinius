#ifndef RBX_GLOBALS_HPP
#define RBX_GLOBALS_HPP

#define SPECIAL_CLASS_MASK 0x1f
#define SPECIAL_CLASS_SIZE 32

namespace rubinius {
  struct Globals {
    /* classes for the core 'types' */
    Class *blokctx, *cmethod, *tuple, *module, *object, *array;
    Class *klass, *hash, *methtbl, *bytearray, *methctx, *blank;
    Class *blokenv, *bignum, *regexp, *regexpdata, *matchdata;
    Class *string, *symbol, *io, *metaclass, *symtbl;
    Class *nil_class, *true_class, *false_class, *fixnum_class, *undef_class;
    Class *floatpoint, *fastctx, *data, *nmethod, *nmc, *task, *list, *list_node;
    Class *channel, *thread, *staticscope, *send_site, *selector, *lookuptable;
    Class *iseq, *executable, *ffi_ptr, *ffi_func, *iobuffer;
    Class *cmethod_vis;

    /* the primary symbol table */
    SymbolTable* symbols;
    SYMBOL sym_method_missing;
    SYMBOL sym_inherited, sym_opened_class;
    SYMBOL sym_from_literal, sym_method_added, sym_s_method_added, sym_init_copy;
    SYMBOL sym_plus, sym_minus, sym_equal, sym_nequal, sym_tequal, sym_lt, sym_gt;
    OBJECT exc_arg, exc_segfault;
    OBJECT exc_loe, exc_type, exc_rex;
    OBJECT exc_stack_explosion;
    OBJECT exc_primitive_failure, sym_initialize;

    LookupTable* external_ivars;
    Tuple* scheduled_threads;
    LookupTable* errno_mapping;
    LookupTable* selectors;
    OBJECT config;
    SYMBOL sym_send;
    SYMBOL sym_public, sym_private, sym_protected, sym_const_missing;
    SYMBOL sym_object_id, sym_call;
    OBJECT exception;
    OBJECT top_scope, on_gc_channel;

    Class* special_classes[SPECIAL_CLASS_SIZE];
  };

  const static size_t num_globals = ((size_t)(sizeof(struct Globals) / sizeof(OBJECT)));

};

#endif
