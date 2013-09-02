#ifndef RBX_GLOBALS_HPP
#define RBX_GLOBALS_HPP

/** Rubinius Root objects
 *
 * This is the set of root objects that the system tracks.
 * When performing garbage collection, it is these objects
 * that are considered the root of the tree and walked
 * to find all live objects.
 *
 * Typically, these are builtin class that the VM needs to
 * reference directly. This is usually the case for things like
 * String::create that need to have a reference to the ruby String
 * class object to properly create a String instance.
 *
 * The rule of thumb for adding classes/modules to this list
 * is whether or not the VM needs to directly reference the
 * class/module. Again, this is typically only because the VM
 * itself needs to create an instance of that class.
 *
 * There are a few non-class roots in here for runtime data.
 */


#define SPECIAL_CLASS_MASK 0x1f
#define SPECIAL_CLASS_SIZE 32

#include "gc/root.hpp"

namespace rubinius {

  class LookupTable;
  class Tuple;
  class Module;
  class Thread;
  class Class;
  class Symbol;
  class Exception;
  class Encoding;

  struct Globals {

    Roots roots;

    /* This requires some manual work when adding a root, but thats ok.
     * First, add the root to the END of this list. Then add it to
     * the constructor for Globals, again, at the END of the list. */

    /* classes for the core 'types' */
    TypedRoot<Class*> compiled_code, tuple, module, basicobject, object, array;
    TypedRoot<Class*> klass, methtbl, bytearray;
    TypedRoot<Class*> blokenv, bignum, regexp, matchdata;
    TypedRoot<Class*> string, character, symbol, io;
    TypedRoot<Class*> nil_class, true_class, false_class, fixnum_class, undef_class;
    TypedRoot<Class*> floatpoint, nmc, list, list_node;
    TypedRoot<Class*> channel, thread, thread_state, constantscope, constant_table, lookuptable;
    TypedRoot<Class*> iseq, executable, native_function, iobuffer;
    TypedRoot<Class*> included_module;

    /* the primary symbol table */
    TypedRoot<Symbol*> sym_method_missing;
    TypedRoot<Symbol*> sym_respond_to_missing;
    TypedRoot<Symbol*> sym_inherited;
    TypedRoot<Symbol*> sym_from_literal, sym_method_added, sym_s_method_added, sym_init_copy;
    TypedRoot<Symbol*> sym_plus, sym_minus, sym_equal, sym_nequal, sym_tequal, sym_lt, sym_gt;
    TypedRoot<Symbol*> sym_initialize;
    TypedRoot<Symbol*> sym_coerce_to_array, sym_to_ary;
    TypedRoot<Symbol*> sym_allocation_site;

    TypedRoot<Class*> exception;
    TypedRoot<Class*> exc_arg, exc_segfault;
    TypedRoot<Class*> exc_loe, exc_type, exc_rex, exc_rte;
    TypedRoot<Class*> exc_primitive_failure;

    TypedRoot<LookupTable*> external_ivars;
    TypedRoot<LookupTable*> errno_mapping;
    TypedRoot<Object*> config;
    TypedRoot<Symbol*> sym_send;
    TypedRoot<Symbol*> sym_public, sym_private, sym_protected, sym_const_missing;
    TypedRoot<Symbol*> sym_object_id, sym_call, sym_undef;

    TypedRoot<Object*> main, undefined;
    TypedRoot<Class*> dir;
    TypedRoot<Class*> compactlookuptable;
    TypedRoot<Class*> lookuptablebucket;
    TypedRoot<Class*> constant_table_bucket;
    TypedRoot<Class*> methtblbucket;
    TypedRoot<Class*> access_variable;
    TypedRoot<Module*> rubinius;
    TypedRoot<Class*> time_class;
    TypedRoot<Class*> stat_class;
    TypedRoot<Class*> integer;
    TypedRoot<Class*> numeric;
    TypedRoot<Class*> memory_pointer;
    TypedRoot<Module*> ffi;
    TypedRoot<Class*> ffi_pointer;
    TypedRoot<Class*> taskprobe;
    TypedRoot<Class*> randomizer;

    TypedRoot<Class*> nmethod;        /**< NativeMethod */

    TypedRoot<Class*> data;

    TypedRoot<Class*> autoload; /**< Autoload class */
    TypedRoot<Class*> proc; /**< Proc class */
    TypedRoot<Class*> variable_scope;
    TypedRoot<Class*> location;
    TypedRoot<Class*> stack_error;
    TypedRoot<Class*> jump_error;
    TypedRoot<Class*> exc_vm_internal;
    TypedRoot<Class*> exc_vm_bad_bytecode;
    TypedRoot<Class*> call_site;
    TypedRoot<Class*> mono_inline_cache;
    TypedRoot<Class*> poly_inline_cache;
    TypedRoot<Class*> inline_cache_entry;
    TypedRoot<Class*> call_custom_cache;
    TypedRoot<Class*> respond_to_cache;
    TypedRoot<Class*> constant_cache;
    TypedRoot<Class*> cls_weakref;
    TypedRoot<Class*> fiber;
    TypedRoot<Class*> alias;
    TypedRoot<Class*> encoding;
    TypedRoot<Object*> encoding_list;
    TypedRoot<Module*> type;
    TypedRoot<Class*> vm_class;
    TypedRoot<Class*> atomic_ref;
    TypedRoot<Class*> mirror;

    TypedRoot<Encoding*> usascii_encoding, utf8_encoding, ascii8bit_encoding;

    /* Add new globals above this line. */

    /* Leave this as the last data member always */
    TypedRoot<Class*> special_classes[SPECIAL_CLASS_SIZE];

    Globals() :
      compiled_code(&roots),
      tuple(&roots),
      module(&roots),
      basicobject(&roots),
      object(&roots),
      array(&roots),
      klass(&roots),
      methtbl(&roots),
      bytearray(&roots),
      blokenv(&roots),
      bignum(&roots),
      regexp(&roots),
      matchdata(&roots),
      string(&roots),
      character(&roots),
      symbol(&roots),
      io(&roots),
      nil_class(&roots),
      true_class(&roots),
      false_class(&roots),
      fixnum_class(&roots),
      undef_class(&roots),
      floatpoint(&roots),
      nmc(&roots),
      list(&roots),
      list_node(&roots),
      channel(&roots),
      thread(&roots),
      thread_state(&roots),
      constantscope(&roots),
      constant_table(&roots),
      lookuptable(&roots),
      iseq(&roots),
      executable(&roots),
      native_function(&roots),
      iobuffer(&roots),
      included_module(&roots),
      sym_method_missing(&roots),
      sym_respond_to_missing(&roots),
      sym_inherited(&roots),
      sym_from_literal(&roots),
      sym_method_added(&roots),
      sym_s_method_added(&roots),
      sym_init_copy(&roots),
      sym_plus(&roots),
      sym_minus(&roots),
      sym_equal(&roots),
      sym_nequal(&roots),
      sym_tequal(&roots),
      sym_lt(&roots),
      sym_gt(&roots),
      sym_initialize(&roots),
      sym_coerce_to_array(&roots),
      sym_to_ary(&roots),
      sym_allocation_site(&roots),
      exception(&roots),
      exc_arg(&roots),
      exc_segfault(&roots),
      exc_loe(&roots),
      exc_type(&roots),
      exc_rex(&roots),
      exc_rte(&roots),
      exc_primitive_failure(&roots),
      external_ivars(&roots),
      errno_mapping(&roots),
      config(&roots),
      sym_send(&roots),
      sym_public(&roots),
      sym_private(&roots),
      sym_protected(&roots),
      sym_const_missing(&roots),
      sym_object_id(&roots),
      sym_call(&roots),
      sym_undef(&roots),
      main(&roots),
      undefined(&roots),
      dir(&roots),
      compactlookuptable(&roots),
      lookuptablebucket(&roots),
      constant_table_bucket(&roots),
      methtblbucket(&roots),
      access_variable(&roots),
      rubinius(&roots),
      time_class(&roots),
      stat_class(&roots),
      integer(&roots),
      numeric(&roots),
      memory_pointer(&roots),
      ffi(&roots),
      ffi_pointer(&roots),
      taskprobe(&roots),
      randomizer(&roots),

      nmethod(&roots),
      data(&roots),
      autoload(&roots),
      proc(&roots),
      variable_scope(&roots),
      location(&roots),
      stack_error(&roots),
      jump_error(&roots),
      exc_vm_internal(&roots),
      exc_vm_bad_bytecode(&roots),
      call_site(&roots),
      mono_inline_cache(&roots),
      poly_inline_cache(&roots),
      inline_cache_entry(&roots),
      call_custom_cache(&roots),
      respond_to_cache(&roots),
      constant_cache(&roots),
      cls_weakref(&roots),
      fiber(&roots),
      alias(&roots),
      encoding(&roots),
      encoding_list(&roots),
      type(&roots),
      vm_class(&roots),
      atomic_ref(&roots),
      mirror(&roots),
      usascii_encoding(&roots),
      utf8_encoding(&roots),
      ascii8bit_encoding(&roots)

      /* Add initialize of globals above this line. */
    { }
  };
};

#endif
