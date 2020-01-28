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

#include "memory/root.hpp"

namespace rubinius {

  class CodeDB;
  class LookupTable;
  class Tuple;
  class Module;
  class Thread;
  class Class;
  class Symbol;
  class Exception;
  class Encoding;
  class JIT;

  struct Globals {

    memory::Roots roots;

    /* This requires some manual work when adding a root, but thats ok.
     * First, add the root to the END of this list. Then add it to
     * the constructor for Globals, again, at the END of the list. */

    /* classes for the core 'types' */
    memory::TypedRoot<Class*> compiled_code, tuple, rtuple, module, basicobject, object, array;
    memory::TypedRoot<Class*> klass, method_table, bytearray;
    memory::TypedRoot<Class*> blokenv, bignum, regexp, matchdata;
    memory::TypedRoot<Class*> string, character, symbol, io;
    memory::TypedRoot<Class*> nil_class, true_class, false_class, fixnum_class, undef_class;
    memory::TypedRoot<Class*> floatpoint, list, list_node;
    memory::TypedRoot<Class*> channel, thread, unwind_state, constantscope;
    memory::TypedRoot<Class*> constant_table, lookup_table;
    memory::TypedRoot<Class*> iseq, executable, native_function;
    memory::TypedRoot<Class*> select, fdset, rio_stream;
    memory::TypedRoot<Class*> included_module;

    /* the primary symbol table */
    memory::TypedRoot<Symbol*> sym_method_missing;
    memory::TypedRoot<Symbol*> sym_respond_to_missing;
    memory::TypedRoot<Symbol*> sym_inherited;
    memory::TypedRoot<Symbol*> sym_from_literal, sym_method_added, sym_s_method_added, sym_init_copy;
    memory::TypedRoot<Symbol*> sym_plus, sym_minus, sym_equal, sym_nequal, sym_tequal, sym_lt, sym_gt;
    memory::TypedRoot<Symbol*> sym_initialize;
    memory::TypedRoot<Symbol*> sym_coerce_to_array, sym_to_ary, sym_to_hash;
    memory::TypedRoot<Symbol*> sym_allocation_site, sym_keyword_object;

    memory::TypedRoot<Class*> exception;
    memory::TypedRoot<Class*> exc_arg, exc_segfault;
    memory::TypedRoot<Class*> exc_loe, exc_type, exc_rex, exc_rte, exc_cue;
    memory::TypedRoot<Class*> exc_primitive_failure;

    memory::TypedRoot<LookupTable*> external_ivars;
    memory::TypedRoot<LookupTable*> errno_mapping;
    memory::TypedRoot<Object*> config;
    memory::TypedRoot<Symbol*> sym_send;
    memory::TypedRoot<Symbol*> sym_public, sym_private, sym_protected, sym_const_missing;
    memory::TypedRoot<Symbol*> sym_object_id, sym_call, sym_undef;

    memory::TypedRoot<Object*> main, undefined;
    memory::TypedRoot<Class*> dir;
    memory::TypedRoot<Class*> compactlookuptable;
    memory::TypedRoot<Class*> lookup_table_bucket;
    memory::TypedRoot<Class*> constant_table_bucket;
    memory::TypedRoot<Class*> method_table_bucket;
    memory::TypedRoot<Class*> access_variable;
    memory::TypedRoot<Module*> rubinius;
    memory::TypedRoot<Class*> time_class;
    memory::TypedRoot<Class*> stat_class;
    memory::TypedRoot<Class*> integer;
    memory::TypedRoot<Class*> numeric;
    memory::TypedRoot<Class*> memory_pointer;
    memory::TypedRoot<Module*> ffi;
    memory::TypedRoot<Class*> ffi_pointer;
    memory::TypedRoot<Class*> taskprobe;
    memory::TypedRoot<Class*> randomizer;

    memory::TypedRoot<Class*> nmethod;        /**< NativeMethod */

    memory::TypedRoot<Class*> data;

    memory::TypedRoot<Class*> autoload; /**< Autoload class */
    memory::TypedRoot<Class*> proc; /**< Proc class */
    memory::TypedRoot<Class*> variable_scope;
    memory::TypedRoot<Class*> location;
    memory::TypedRoot<Class*> stack_error;
    memory::TypedRoot<Class*> jump_error;
    memory::TypedRoot<Class*> exc_vm_internal;
    memory::TypedRoot<Class*> exc_vm_bad_bytecode;
    memory::TypedRoot<Class*> call_site;
    memory::TypedRoot<Class*> unwind_site;
    memory::TypedRoot<Class*> inline_cache;
    memory::TypedRoot<Class*> call_custom_cache;
    memory::TypedRoot<Class*> respond_to_cache;
    memory::TypedRoot<Class*> constant_cache;
    memory::TypedRoot<Class*> cls_weakref;
    memory::TypedRoot<Class*> fiber;
    memory::TypedRoot<Class*> alias;
    memory::TypedRoot<Class*> encoding;
    memory::TypedRoot<Object*> encoding_list;
    memory::TypedRoot<Module*> type;
    memory::TypedRoot<Class*> vm_class;
    memory::TypedRoot<Class*> atomic_ref;
    memory::TypedRoot<Class*> mirror;
    memory::TypedRoot<Class*> fsevent;
    memory::TypedRoot<Class*> logger;
    memory::TypedRoot<JIT*> jit;
    memory::TypedRoot<Module*> runtime;
    memory::TypedRoot<Class*> codedb;
    memory::TypedRoot<CodeDB*> coredb;
    memory::TypedRoot<Class*> prediction;
    memory::TypedRoot<Class*> method_prediction;

    memory::TypedRoot<Encoding*> usascii_encoding, utf8_encoding, ascii8bit_encoding;

    /* Add new globals above this line. */

    /* Leave this as the last data member always */
    memory::TypedRoot<Class*> special_classes[SPECIAL_CLASS_SIZE];

    Globals() :
      compiled_code(&roots),
      tuple(&roots),
      rtuple(&roots),
      module(&roots),
      basicobject(&roots),
      object(&roots),
      array(&roots),
      klass(&roots),
      method_table(&roots),
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
      list(&roots),
      list_node(&roots),
      channel(&roots),
      thread(&roots),
      unwind_state(&roots),
      constantscope(&roots),
      constant_table(&roots),
      lookup_table(&roots),
      iseq(&roots),
      executable(&roots),
      native_function(&roots),
      select(&roots),
      fdset(&roots),
      rio_stream(&roots),
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
      sym_to_hash(&roots),
      sym_allocation_site(&roots),
      sym_keyword_object(&roots),
      exception(&roots),
      exc_arg(&roots),
      exc_segfault(&roots),
      exc_loe(&roots),
      exc_type(&roots),
      exc_rex(&roots),
      exc_rte(&roots),
      exc_cue(&roots),
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
      lookup_table_bucket(&roots),
      constant_table_bucket(&roots),
      method_table_bucket(&roots),
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
      unwind_site(&roots),
      inline_cache(&roots),
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
      fsevent(&roots),
      logger(&roots),
      jit(&roots),
      runtime(&roots),
      codedb(&roots),
      coredb(&roots),
      prediction(&roots),
      method_prediction(&roots),
      usascii_encoding(&roots),
      utf8_encoding(&roots),
      ascii8bit_encoding(&roots)

      /* Add initialize of globals above this line. */
    { }
  };
};

#endif
