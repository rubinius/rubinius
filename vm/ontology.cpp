#include <sys/ioctl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "objectmemory.hpp"
#include "vm.hpp"

#include "builtin/access_variable.hpp"
#include "builtin/array.hpp"
#include "builtin/basicobject.hpp"
#include "builtin/block_environment.hpp"
#include "builtin/bytearray.hpp"
#include "builtin/character.hpp"
#include "builtin/class.hpp"
#include "builtin/compactlookuptable.hpp"
#include "builtin/compiledcode.hpp"
#include "builtin/channel.hpp"
#include "builtin/data.hpp"
#include "builtin/dir.hpp"
#include "builtin/encoding.hpp"
#include "builtin/executable.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/float.hpp"
#include "builtin/io.hpp"
#include "builtin/iseq.hpp"
#include "builtin/list.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/methodtable.hpp"
#include "builtin/ffi_pointer.hpp"
#include "builtin/nativefunction.hpp"
#include "builtin/nativemethod.hpp"
#include "builtin/regexp.hpp"
#include "builtin/constantscope.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/system.hpp"
#include "builtin/thread.hpp"
#include "builtin/time.hpp"
#include "builtin/stat.hpp"
#include "builtin/tuple.hpp"
#include "builtin/autoload.hpp"
#include "builtin/proc.hpp"
#include "builtin/variable_scope.hpp"
#include "builtin/location.hpp"
#include "builtin/mono_inline_cache.hpp"
#include "builtin/poly_inline_cache.hpp"
#include "builtin/call_custom_cache.hpp"
#include "builtin/respond_to_cache.hpp"
#include "builtin/constant_cache.hpp"
#include "builtin/weakref.hpp"
#include "builtin/fiber.hpp"
#include "builtin/alias.hpp"
#include "builtin/randomizer.hpp"
#include "builtin/module.hpp"
#include "builtin/class.hpp"
#include "builtin/atomic.hpp"
#include "builtin/thread_state.hpp"

#include "environment.hpp"
#include "configuration.hpp"
#include "config.h"
#include "paths.h"
#include "release.h"
#include "version.h"

#include "ontology.hpp"

#define SPECIAL_CLASS_MASK 0x1f
#define SPECIAL_CLASS_SIZE 32

namespace rubinius {
  namespace ontology {
    Class* new_basic_class(STATE, Class* sup) {
      Class *cls = state->memory()->new_object_enduring<Class>(state, G(klass));
      cls->init(state);

      if(sup->nil_p()) {
        cls->instance_type(state, Fixnum::from(ObjectType));
        cls->set_type_info(state->vm()->find_type(ObjectType));
      } else {
        cls->instance_type(state, sup->instance_type());
        cls->set_type_info(sup->type_info());
      }
      cls->superclass(state, sup);

      return cls;
    }

    Class* new_class(STATE, const char* name) {
      return new_class(state, name, G(object), G(object));
    }

    Class* new_class(STATE, const char* name, Class* sup, Module* under) {
      if(!under) under = G(object);

      Class* cls = new_basic_class(state, sup);
      cls->setup(state, name, under);

      // HACK test that we've got the MOP setup properly
      SingletonClass::attach(state, cls, sup->singleton_class(state));
      return cls;
    }

    Class* new_class_under(STATE, const char* name, Module* under) {
      return new_class(state, name, G(object), under);
    }

    Module* new_module(STATE, const char* name, Module* under) {
      Module *mod = state->new_object<Module>(G(module));
      mod->setup(state, name, under);
      return mod;
    }
  }

  // Reset macros since we're inside state
#undef G
#undef GO
#define G(whatever) globals().whatever.get()
#define GO(whatever) globals().whatever

  void VM::bootstrap_class(STATE) {
    /* Class is created first by hand, and twiddle to setup the internal
       recursion. */
    Class* cls = reinterpret_cast<Class*>(om->allocate_object_raw(sizeof(Class)));

    /* We create these 8 classes in a particular way and in a particular
     * order. We need all 8 to create fully initialized Classes and
     * Modules, so we just create them all uninitialized, then initialize
     * them all at once */

    // Class's klass is Class
    cls->set_obj_type(ClassType);
    cls->klass(state, cls);
    cls->ivars(state, cNil);

    cls->set_object_type(state, ClassType);
    cls->init(state);
    cls->set_packed_size(0);

    // Set Class into the globals
    GO(klass).set(cls);

    // Now do Object
    Class* basicobject = 0;
    Class* object;
    if(!LANGUAGE_18_ENABLED) {
      basicobject = ontology::new_basic_class(state, force_as<Class>(cNil));
      GO(basicobject).set(basicobject);
      basicobject->set_object_type(state, BasicObjectType);

      object = ontology::new_basic_class(state, basicobject);
    } else {
      object = ontology::new_basic_class(state, nil<Class>());
    }

    GO(object).set(object);
    object->set_object_type(state, ObjectType);

    // Now Module
    GO(module).set(ontology::new_basic_class(state, object));
    G(module)->set_object_type(state, ModuleType);

    // Fixup Class's superclass to be Module
    cls->superclass(state, G(module));

    // Create Tuple
    Tuple::init(state);

    // Create Array
    GO(array).set(ontology::new_basic_class(state, object));
    G(array)->set_object_type(state, ArrayType);

    // Create WeakRef
    if(!LANGUAGE_18_ENABLED) {
      GO(cls_weakref).set(ontology::new_basic_class(state, basicobject));
    } else {
      GO(cls_weakref).set(ontology::new_basic_class(state, object));
    }
    G(cls_weakref)->set_object_type(state, WeakRefType);

    // Create LookupTable
    GO(lookuptable).set(ontology::new_basic_class(state, object));
    G(lookuptable)->set_object_type(state, LookupTableType);

    // Create LookupTableBucket
    GO(lookuptablebucket).set(ontology::new_basic_class(state, object));
    G(lookuptablebucket)->set_object_type(state, LookupTableBucketType);

    // Create ConstantTable
    GO(constant_table).set(ontology::new_basic_class(state, object));
    G(constant_table)->set_object_type(state, ConstantTableType);

    // Create ConstantTableBucket
    GO(constant_table_bucket).set(ontology::new_basic_class(state, object));
    G(constant_table_bucket)->set_object_type(state, ConstantTableBucketType);

    // Create MethodTable
    GO(methtbl).set(ontology::new_basic_class(state, object));
    G(methtbl)->set_object_type(state, MethodTableType);

    // Create MethodTableBucket
    GO(methtblbucket).set(ontology::new_basic_class(state, object));
    G(methtblbucket)->set_object_type(state, MethodTableBucketType);

    /* Now, we have:
     *  Class
     *  Module
     *  Object
     *  Tuple
     *  Array
     *  LookupTable
     *  LookupTableBucket
     *  ConstantTable
     *  ConstantTableBucket
     *  MethodTable
     *  MethodTableBucket
     *
     *  With these 8 in place, we can now create fully initialized classes
     *  and modules.
     *
     *  Next we need to finish up the SingletonClass protocol (a.k.a. MOP).
     *  The SingletonClass of a subclass points to the SingletonClass of the
     *  superclass.
     */

    // BasicObject's SingletonClass instance has Class for a superclass
    if(!LANGUAGE_18_ENABLED) {
      SingletonClass::attach(state, basicobject, cls);
    }

    // Object's SingletonClass instance has Class for a superclass
    Class* sc = SingletonClass::attach(state, object, cls);

    // Module's metaclass's superclass is Object's metaclass
    sc = SingletonClass::attach(state, G(module), sc);
    // Class's metaclass likewise has Module's metaclass above it
    SingletonClass::attach(state, cls, sc);

    // See?
    if(!LANGUAGE_18_ENABLED) {
      assert(basicobject->superclass()->nil_p());
      assert(object->superclass() == basicobject);
    } else {
      assert(object->superclass()->nil_p());
      assert(object->klass()->superclass() == cls);
    }

    assert(G(module)->superclass() == object);
    assert(G(module)->klass()->superclass() == object->klass());

    assert(cls->superclass() == G(module));
    assert(cls->klass()->superclass() == G(module)->klass());

    // The other builtin classes get SingletonClasses wired to Object's singleton class
    sc = G(object)->singleton_class(state);
    SingletonClass::attach(state, G(tuple), sc);
    SingletonClass::attach(state, G(array), sc);
    SingletonClass::attach(state, G(cls_weakref), sc);
    SingletonClass::attach(state, G(lookuptable), sc);
    SingletonClass::attach(state, G(lookuptablebucket), sc);
    SingletonClass::attach(state, G(constant_table), sc);
    SingletonClass::attach(state, G(constant_table_bucket), sc);
    SingletonClass::attach(state, G(methtbl), sc);
    SingletonClass::attach(state, G(methtblbucket), sc);

    // Now, finish initializing the basic Class/Module
    G(object)->setup(state, "Object");
    if(!LANGUAGE_18_ENABLED) {
      G(basicobject)->setup(state, "BasicObject", G(object));
    }
    G(klass)->setup(state, "Class");
    G(module)->setup(state, "Module");

    // Create the namespace for various implementation classes
    GO(rubinius).set(ontology::new_module(state, "Rubinius"));

    // Finish initializing the rest of the special 8
    G(tuple)->setup(state, "Tuple", G(rubinius));
    G(array)->setup(state, "Array");
    G(cls_weakref)->setup(state, "WeakRef");

    G(lookuptable)->setup(state, "LookupTable", G(rubinius));
    G(lookuptablebucket)->setup(state, "Bucket", G(lookuptable));

    G(constant_table)->setup(state, "ConstantTable", G(rubinius));
    G(constant_table_bucket)->setup(state, "Bucket", G(constant_table));

    G(methtbl)->setup(state, "MethodTable", G(rubinius));
    G(methtblbucket)->setup(state, "Bucket", G(methtbl));
  }

  void VM::initialize_builtin_classes(STATE) {
    // Create the immediate classes.
    GO(nil_class).set(ontology::new_class(state, "NilClass"));
    G(nil_class)->set_object_type(state, NilType);
    GO(nil_class)->type_info()->allow_user_allocate = false;

    GO(true_class).set(ontology::new_class(state, "TrueClass"));
    G(true_class)->set_object_type(state, TrueType);
    GO(true_class)->type_info()->allow_user_allocate = false;

    GO(false_class).set(ontology::new_class(state, "FalseClass"));
    G(false_class)->set_object_type(state, FalseType);
    GO(false_class)->type_info()->allow_user_allocate = false;

    Numeric::init(state);
    Integer::init(state);
    Fixnum::init(state);
    Symbol::init(state);

    // Setup the special_class lookup table. We use this to resolve
    // the classes for Fixnum's, nil, true and false.
    for(size_t i = 0; i < SPECIAL_CLASS_SIZE; i++) {
      if(SYMBOL_P(i)) {
        globals().special_classes[i] = GO(symbol);
      } else if(FIXNUM_P(i)) {
        globals().special_classes[i] = GO(fixnum_class);
      } else {
        globals().special_classes[i] = GO(object); /* unused slot */
      }
    }

    globals().special_classes[(uintptr_t)cFalse] = GO(false_class);
    globals().special_classes[(uintptr_t)cNil  ] = GO(nil_class);
    globals().special_classes[(uintptr_t)cTrue ] = GO(true_class);

    /* Create IncludedModule */
    GO(included_module).set(ontology::new_class(state,
          "IncludedModule", G(module), G(rubinius)));
    G(included_module)->set_object_type(state, IncludedModuleType);

    // Let all the builtin classes initialize themselves. this
    // typically means creating a Ruby class.
    CompactLookupTable::init(state);
    ByteArray::init(state);
    String::init(state);
    Character::init(state);
    Executable::init(state);
    CompiledCode::init(state);
    AtomicReference::init(state);
    IO::init(state);
    BlockEnvironment::init(state);
    ConstantScope::init(state);
    Dir::init(state);
    Time::init(state);
    Stat::init(state);
    Regexp::init(state);
    Bignum::init(state);
    Float::init(state);
    InstructionSequence::init(state);
    List::init(state);
    init_ffi(state);
    Thread::init(state);
    ThreadState::init(state);
    AccessVariable::init(state);
    Pointer::init(state);
    NativeFunction::init(state);
    Exception::init(state);
    Data::init(state);
    Autoload::init(state);
    Proc::init(state);
    VariableScope::init(state);
    Location::init(state);

    Channel::init(state);

    NativeMethod::init(state);

    CallSite::init(state);
    MonoInlineCache::init(state);
    PolyInlineCache::init(state);
    CallCustomCache::init(state);
    ConstantCache::init(state);
    RespondToCache::init(state);
    Fiber::init(state);
    Alias::init(state);
    Randomizer::init(state);

    Encoding::init(state);
    kcode::init(state);
  }

  // @todo document all the sections of bootstrap_ontology
  /* Creates the rubinius object universe from scratch. */
  void VM::bootstrap_ontology(STATE) {

    /*
     * Bootstrap everything so we can create fully initialized
     * Classes.
     */
    bootstrap_symbol(state);
    bootstrap_class(state);

    /*
     * Everything is now setup for us to make fully initialized
     * classes.
     */

    Object* undef = new_object<Object>(G(object));
    GO(undefined).set(undef);

    /*
     * Create our Rubinius module that we hang stuff off
     */

    initialize_fundamental_constants(state);

    initialize_builtin_classes(state);
    bootstrap_exceptions(state);

    /*
     * Create any 'stock' objects
     */

    Object* main = new_object<Object>(G(object));
    GO(main).set(main);
    G(object)->set_const(state, "MAIN", main); // HACK test hooking up MAIN

    GO(vm_class).set(ontology::new_class_under(state, "VM", G(rubinius)));

    GO(type).set(ontology::new_module(state, "Type", G(rubinius)));
    GO(mirror).set(ontology::new_class(state, "Mirror", G(object), G(rubinius)));

    System::bootstrap_methods(state);
    Module::bootstrap_methods(state);
    ConstantScope::bootstrap_methods(state);
    VariableScope::bootstrap_methods(state);
    BlockEnvironment::bootstrap_methods(state);

    /*
     * Setup the table we use to store ivars for immediates
     */

    GO(external_ivars).set(LookupTable::create(state));

    initialize_platform_data(state);
  }

  void VM::initialize_fundamental_constants(STATE) {
    if(sizeof(int) == sizeof(long)) {
      G(rubinius)->set_const(state, "L64", cFalse);
    } else {
      G(rubinius)->set_const(state, "L64", cTrue);
    }

    G(rubinius)->set_const(state, "WORDSIZE", Fixnum::from(sizeof(void*) * 8));
  }

  void VM::initialize_platform_data(STATE) {
    // HACK test hooking up IO
    IO* in_io  = IO::create(state, STDIN_FILENO);
    IO* out_io = IO::create(state, STDOUT_FILENO);
    IO* err_io = IO::create(state, STDERR_FILENO);

    out_io->sync(state, cTrue);
    err_io->sync(state, cTrue);

    in_io->force_read_only(state);
    out_io->force_write_only(state);
    err_io->force_write_only(state);

    G(object)->set_const(state, "STDIN",  in_io);
    G(object)->set_const(state, "STDOUT", out_io);
    G(object)->set_const(state, "STDERR", err_io);

    /* Fundamental constants. These are uniformly Strings and not symbols
     * because some are passed to e.g. File.expand_path and having them
     * be uniform is simpler.
     */
    Environment* env = state->shared().env();

    if(env) {
      std::string prefix = env->system_prefix();
      G(rubinius)->set_const(state, "PREFIX_PATH", String::create(state, prefix.c_str()));
      std::string path = prefix + RBX_RUNTIME_PATH;
      G(rubinius)->set_const(state, "RUNTIME_PATH", String::create(state, path.c_str()));
      path = prefix + RBX_BIN_PATH;
      G(rubinius)->set_const(state, "BIN_PATH", String::create(state, path.c_str()));
      path = prefix + RBX_KERNEL_PATH;
      G(rubinius)->set_const(state, "KERNEL_PATH", String::create(state, path.c_str()));
      path = prefix + RBX_LIB_PATH;
      G(rubinius)->set_const(state, "LIB_PATH", String::create(state, path.c_str()));
      path = prefix + RBX_ENC_PATH;
      G(rubinius)->set_const(state, "ENC_PATH", String::create(state, path.c_str()));
      path = prefix + RBX_VENDOR_PATH;
      G(rubinius)->set_const(state, "VENDOR_PATH", String::create(state, path.c_str()));
      path = prefix + RBX_GEMS_PATH;
      G(rubinius)->set_const(state, "GEMS_PATH", String::create(state, path.c_str()));

      path = prefix + RBX_HDR_PATH;
      G(rubinius)->set_const(state, "HDR_PATH", String::create(state, path.c_str()));
    }

    G(rubinius)->set_const(state, "VERSION", String::create(state, RBX_VERSION));
    G(rubinius)->set_const(state, "LIB_VERSION", String::create(state, RBX_LIB_VERSION));
    G(rubinius)->set_const(state, "BUILD_REV", String::create(state, RBX_BUILD_REV));
    G(rubinius)->set_const(state, "RELEASE_DATE", String::create(state, RBX_RELEASE_DATE));
    G(rubinius)->set_const(state, "LDSHARED", String::create(state, RBX_LDSHARED));
    G(rubinius)->set_const(state, "LDSHAREDXX", String::create(state, RBX_LDSHAREDXX));

    G(rubinius)->set_const(state, "HOST", String::create(state, RBX_HOST));
    G(rubinius)->set_const(state, "CPU", String::create(state, RBX_CPU));
    G(rubinius)->set_const(state, "VENDOR", String::create(state, RBX_VENDOR));
    G(rubinius)->set_const(state, "OS", String::create(state, RBX_OS));

    if(LANGUAGE_20_ENABLED) {
      G(rubinius)->set_const(state, "RUBY_LIB_VERSION", Fixnum::from(20));
    } else if(LANGUAGE_19_ENABLED) {
      G(rubinius)->set_const(state, "RUBY_LIB_VERSION", Fixnum::from(19));
    } else {
      G(rubinius)->set_const(state, "RUBY_LIB_VERSION", Fixnum::from(18));
    }
    G(rubinius)->set_const(state, "LIBC", String::create(state, RBX_LIBC));

    G(rubinius)->set_const(state, "HAVE_LCHMOD", RBX_HAVE_LCHMOD ? cTrue : cFalse);
    G(rubinius)->set_const(state, "HAVE_LCHOWN", RBX_HAVE_LCHOWN ? cTrue : cFalse);

#ifdef RBX_LITTLE_ENDIAN
    G(rubinius)->set_const(state, "ENDIAN", state->symbol("little"));
#else
    G(rubinius)->set_const(state, "ENDIAN", state->symbol("big"));
#endif

    G(rubinius)->set_const(state, "PATH_MAX", Fixnum::from(PATH_MAX));

    // Used in Array.pack
    G(rubinius)->set_const(state, "SIZEOF_SHORT", Fixnum::from(sizeof(short)));
    G(rubinius)->set_const(state, "SIZEOF_INT", Fixnum::from(sizeof(int)));
    G(rubinius)->set_const(state, "SIZEOF_LONG", Fixnum::from(sizeof(long)));

    struct winsize w;
    if(ioctl(0, TIOCGWINSZ, &w)) {
      w.ws_col = 80;
    }
    G(rubinius)->set_const(state, "TERMINAL_WIDTH", Fixnum::from(w.ws_col));
  }

  void VM::bootstrap_symbol(STATE) {
#define add_sym(name) GO(sym_ ## name).set(state->symbol(#name))
    add_sym(object_id);
    add_sym(method_missing);
    add_sym(inherited);
    add_sym(from_literal);
    add_sym(method_added);
    add_sym(send);
    add_sym(public);
    add_sym(private);
    add_sym(protected);
    add_sym(undef);
    add_sym(const_missing);
    add_sym(object_id);
    add_sym(call);
    add_sym(coerce_to_array);
    add_sym(to_ary);
#undef add_sym
    GO(sym_s_method_added).set(state->symbol("singleton_method_added"));
    GO(sym_init_copy).set(state->symbol("initialize_copy"));
    GO(sym_plus).set(state->symbol("+"));
    GO(sym_minus).set(state->symbol("-"));
    GO(sym_equal).set(state->symbol("=="));
    GO(sym_nequal).set(state->symbol("!="));
    GO(sym_tequal).set(state->symbol("==="));
    GO(sym_lt).set(state->symbol("<"));
    GO(sym_gt).set(state->symbol(">"));
    GO(sym_allocation_site).set(state->symbol("@__allocation_site__"));
  }

  void VM::setup_errno(STATE, int num, const char* name, Class* sce, Module* ern) {
    bool found = false;

    Object* key = Fixnum::from(num);

    Object* current = state->globals().errno_mapping->fetch(state, key, &found);
    if(found) {
      ern->set_const(state, state->symbol(name), current);
    } else {
      Class* cls = ontology::new_class(state, name, sce, ern);

      // new_class has simply name setting logic that doesn't take into account
      // being not under Object. So we set it again using the smart method.
      cls->set_name(state, name, ern);

      cls->set_const(state, state->symbol("Errno"), key);

      char buf[RBX_STRERROR_BUFSIZE];
      char* err = RBX_STRERROR(num, buf, RBX_STRERROR_BUFSIZE);
      cls->set_const(state, state->symbol("Strerror"), String::create(state, err));
      state->globals().errno_mapping->store(state, key, cls);
    }
  }

  void VM::bootstrap_exceptions(STATE) {
    Class *exc, *scp, *std, *arg, *nam, *loe, *rex, *stk, *sce, *type, *lje, *vme;
    Class *rng, *rte;

#define dexc(name, sup) ontology::new_class(state, #name, sup)

    exc = G(exception);
    scp = dexc(ScriptError, exc);
    std = dexc(StandardError, exc);
    type = dexc(TypeError, std);
    arg = dexc(ArgumentError, std);
    nam = dexc(NameError, std);
    rex = dexc(RegexpError, std);
    dexc(NoMethodError, nam);
    dexc(SyntaxError, scp);
    loe = dexc(LoadError, scp);
    rte = dexc(RuntimeError, std);
    sce = dexc(SystemCallError, std);
    // SystemStackError has a different superclass in 1.9
    if(LANGUAGE_18_ENABLED) {
      stk = dexc(SystemStackError, std);
    } else {
      stk = dexc(SystemStackError, exc);
    }
    lje = dexc(LocalJumpError, std);
    rng = dexc(RangeError, std);
    dexc(FloatDomainError, rng);
    dexc(ZeroDivisionError, std);
    dexc(IOError, std);

    GO(jump_error).set(lje);

    GO(exc_vm_internal).set(ontology::new_class(state,
          "Internal", exc, G(rubinius)));
    GO(exc_vm_bad_bytecode).set(
        ontology::new_class(state, "InvalidBytecode",
          G(exc_vm_internal), G(rubinius)));

    // Some special exceptions scoped under the Rubinius module
    vme = ontology::new_class(state, "VMException", exc, G(rubinius));
    ontology::new_class(state, "AssertionError", vme, G(rubinius));
    ontology::new_class(state, "ObjectBoundsExceededError", vme, G(rubinius));

    // The stack_error mechanisms assume that there will be enough
    // space left over to allocate the actual exception.
    GO(stack_error).set(stk);

    GO(exc_type).set(type);
    GO(exc_arg).set(arg);
    GO(exc_loe).set(loe);
    GO(exc_rex).set(rex);
    GO(exc_rte).set(rte);

    GO(exc_primitive_failure).set(dexc(PrimitiveFailure, exc));

    GO(exc_segfault).set(dexc(MemorySegmentionError, exc));

    Module* ern = ontology::new_module(state, "Errno");

    GO(errno_mapping).set(LookupTable::create(state));

    ern->set_const(state, state->symbol("Mapping"), G(errno_mapping));

#define set_syserr(num, name) setup_errno(state, num, name, sce, ern)

    /*
     * Stolen from MRI
     */

#ifdef EPERM
    set_syserr(EPERM, "EPERM");
#endif
#ifdef ENOENT
    set_syserr(ENOENT, "ENOENT");
#endif
#ifdef ESRCH
    set_syserr(ESRCH, "ESRCH");
#endif
#ifdef EINTR
    set_syserr(EINTR, "EINTR");
#endif
#ifdef EIO
    set_syserr(EIO, "EIO");
#endif
#ifdef ENXIO
    set_syserr(ENXIO, "ENXIO");
#endif
#ifdef E2BIG
    set_syserr(E2BIG, "E2BIG");
#endif
#ifdef ENOEXEC
    set_syserr(ENOEXEC, "ENOEXEC");
#endif
#ifdef EBADF
    set_syserr(EBADF, "EBADF");
#endif
#ifdef ECHILD
    set_syserr(ECHILD, "ECHILD");
#endif
#ifdef EAGAIN
    set_syserr(EAGAIN, "EAGAIN");
#endif
#ifdef ENOMEM
    set_syserr(ENOMEM, "ENOMEM");
#endif
#ifdef EACCES
    set_syserr(EACCES, "EACCES");
#endif
#ifdef EFAULT
    set_syserr(EFAULT, "EFAULT");
#endif
#ifdef ENOTBLK
    set_syserr(ENOTBLK, "ENOTBLK");
#endif
#ifdef EBUSY
    set_syserr(EBUSY, "EBUSY");
#endif
#ifdef EEXIST
    set_syserr(EEXIST, "EEXIST");
#endif
#ifdef EXDEV
    set_syserr(EXDEV, "EXDEV");
#endif
#ifdef ENODEV
    set_syserr(ENODEV, "ENODEV");
#endif
#ifdef ENOTDIR
    set_syserr(ENOTDIR, "ENOTDIR");
#endif
#ifdef EISDIR
    set_syserr(EISDIR, "EISDIR");
#endif
#ifdef EINVAL
    set_syserr(EINVAL, "EINVAL");
#endif
#ifdef ENFILE
    set_syserr(ENFILE, "ENFILE");
#endif
#ifdef EMFILE
    set_syserr(EMFILE, "EMFILE");
#endif
#ifdef ENOTTY
    set_syserr(ENOTTY, "ENOTTY");
#endif
#ifdef ETXTBSY
    set_syserr(ETXTBSY, "ETXTBSY");
#endif
#ifdef EFBIG
    set_syserr(EFBIG, "EFBIG");
#endif
#ifdef ENOSPC
    set_syserr(ENOSPC, "ENOSPC");
#endif
#ifdef ESPIPE
    set_syserr(ESPIPE, "ESPIPE");
#endif
#ifdef EROFS
    set_syserr(EROFS, "EROFS");
#endif
#ifdef EMLINK
    set_syserr(EMLINK, "EMLINK");
#endif
#ifdef EPIPE
    set_syserr(EPIPE, "EPIPE");
#endif
#ifdef EDOM
    set_syserr(EDOM, "EDOM");
#endif
#ifdef ERANGE
    set_syserr(ERANGE, "ERANGE");
#endif
#ifdef EDEADLK
    set_syserr(EDEADLK, "EDEADLK");
#endif
#ifdef ENAMETOOLONG
    set_syserr(ENAMETOOLONG, "ENAMETOOLONG");
#endif
#ifdef ENOLCK
    set_syserr(ENOLCK, "ENOLCK");
#endif
#ifdef ENOSYS
    set_syserr(ENOSYS, "ENOSYS");
#endif
#ifdef ENOTEMPTY
    set_syserr(ENOTEMPTY, "ENOTEMPTY");
#endif
#ifdef ELOOP
    set_syserr(ELOOP, "ELOOP");
#endif
#ifdef EWOULDBLOCK
    set_syserr(EWOULDBLOCK, "EWOULDBLOCK");
#endif
#ifdef ENOMSG
    set_syserr(ENOMSG, "ENOMSG");
#endif
#ifdef EIDRM
    set_syserr(EIDRM, "EIDRM");
#endif
#ifdef ECHRNG
    set_syserr(ECHRNG, "ECHRNG");
#endif
#ifdef EL2NSYNC
    set_syserr(EL2NSYNC, "EL2NSYNC");
#endif
#ifdef EL3HLT
    set_syserr(EL3HLT, "EL3HLT");
#endif
#ifdef EL3RST
    set_syserr(EL3RST, "EL3RST");
#endif
#ifdef ELNRNG
    set_syserr(ELNRNG, "ELNRNG");
#endif
#ifdef EUNATCH
    set_syserr(EUNATCH, "EUNATCH");
#endif
#ifdef ENOCSI
    set_syserr(ENOCSI, "ENOCSI");
#endif
#ifdef EL2HLT
    set_syserr(EL2HLT, "EL2HLT");
#endif
#ifdef EBADE
    set_syserr(EBADE, "EBADE");
#endif
#ifdef EBADR
    set_syserr(EBADR, "EBADR");
#endif
#ifdef EXFULL
    set_syserr(EXFULL, "EXFULL");
#endif
#ifdef ENOANO
    set_syserr(ENOANO, "ENOANO");
#endif
#ifdef EBADRQC
    set_syserr(EBADRQC, "EBADRQC");
#endif
#ifdef EBADSLT
    set_syserr(EBADSLT, "EBADSLT");
#endif
#ifdef EDEADLOCK
    set_syserr(EDEADLOCK, "EDEADLOCK");
#endif
#ifdef EBFONT
    set_syserr(EBFONT, "EBFONT");
#endif
#ifdef ENOSTR
    set_syserr(ENOSTR, "ENOSTR");
#endif
#ifdef ENODATA
    set_syserr(ENODATA, "ENODATA");
#endif
#ifdef ETIME
    set_syserr(ETIME, "ETIME");
#endif
#ifdef ENOSR
    set_syserr(ENOSR, "ENOSR");
#endif
#ifdef ENONET
    set_syserr(ENONET, "ENONET");
#endif
#ifdef ENOPKG
    set_syserr(ENOPKG, "ENOPKG");
#endif
#ifdef EREMOTE
    set_syserr(EREMOTE, "EREMOTE");
#endif
#ifdef ENOLINK
    set_syserr(ENOLINK, "ENOLINK");
#endif
#ifdef EADV
    set_syserr(EADV, "EADV");
#endif
#ifdef ESRMNT
    set_syserr(ESRMNT, "ESRMNT");
#endif
#ifdef ECOMM
    set_syserr(ECOMM, "ECOMM");
#endif
#ifdef EPROTO
    set_syserr(EPROTO, "EPROTO");
#endif
#ifdef EMULTIHOP
    set_syserr(EMULTIHOP, "EMULTIHOP");
#endif
#ifdef EDOTDOT
    set_syserr(EDOTDOT, "EDOTDOT");
#endif
#ifdef EBADMSG
    set_syserr(EBADMSG, "EBADMSG");
#endif
#ifdef EOVERFLOW
    set_syserr(EOVERFLOW, "EOVERFLOW");
#endif
#ifdef ENOTUNIQ
    set_syserr(ENOTUNIQ, "ENOTUNIQ");
#endif
#ifdef EBADFD
    set_syserr(EBADFD, "EBADFD");
#endif
#ifdef EREMCHG
    set_syserr(EREMCHG, "EREMCHG");
#endif
#ifdef ELIBACC
    set_syserr(ELIBACC, "ELIBACC");
#endif
#ifdef ELIBBAD
    set_syserr(ELIBBAD, "ELIBBAD");
#endif
#ifdef ELIBSCN
    set_syserr(ELIBSCN, "ELIBSCN");
#endif
#ifdef ELIBMAX
    set_syserr(ELIBMAX, "ELIBMAX");
#endif
#ifdef ELIBEXEC
    set_syserr(ELIBEXEC, "ELIBEXEC");
#endif
#ifdef EILSEQ
    set_syserr(EILSEQ, "EILSEQ");
#endif
#ifdef ERESTART
    set_syserr(ERESTART, "ERESTART");
#endif
#ifdef ESTRPIPE
    set_syserr(ESTRPIPE, "ESTRPIPE");
#endif
#ifdef EUSERS
    set_syserr(EUSERS, "EUSERS");
#endif
#ifdef ENOTSOCK
    set_syserr(ENOTSOCK, "ENOTSOCK");
#endif
#ifdef EDESTADDRREQ
    set_syserr(EDESTADDRREQ, "EDESTADDRREQ");
#endif
#ifdef EMSGSIZE
    set_syserr(EMSGSIZE, "EMSGSIZE");
#endif
#ifdef EPROTOTYPE
    set_syserr(EPROTOTYPE, "EPROTOTYPE");
#endif
#ifdef ENOPROTOOPT
    set_syserr(ENOPROTOOPT, "ENOPROTOOPT");
#endif
#ifdef EPROTONOSUPPORT
    set_syserr(EPROTONOSUPPORT, "EPROTONOSUPPORT");
#endif
#ifdef ESOCKTNOSUPPORT
    set_syserr(ESOCKTNOSUPPORT, "ESOCKTNOSUPPORT");
#endif
#ifdef EOPNOTSUPP
    set_syserr(EOPNOTSUPP, "EOPNOTSUPP");
#endif
#ifdef EPFNOSUPPORT
    set_syserr(EPFNOSUPPORT, "EPFNOSUPPORT");
#endif
#ifdef EAFNOSUPPORT
    set_syserr(EAFNOSUPPORT, "EAFNOSUPPORT");
#endif
#ifdef EADDRINUSE
    set_syserr(EADDRINUSE, "EADDRINUSE");
#endif
#ifdef EADDRNOTAVAIL
    set_syserr(EADDRNOTAVAIL, "EADDRNOTAVAIL");
#endif
#ifdef ENETDOWN
    set_syserr(ENETDOWN, "ENETDOWN");
#endif
#ifdef ENETUNREACH
    set_syserr(ENETUNREACH, "ENETUNREACH");
#endif
#ifdef ENETRESET
    set_syserr(ENETRESET, "ENETRESET");
#endif
#ifdef ECONNABORTED
    set_syserr(ECONNABORTED, "ECONNABORTED");
#endif
#ifdef ECONNRESET
    set_syserr(ECONNRESET, "ECONNRESET");
#endif
#ifdef ENOBUFS
    set_syserr(ENOBUFS, "ENOBUFS");
#endif
#ifdef EISCONN
    set_syserr(EISCONN, "EISCONN");
#endif
#ifdef ENOTCONN
    set_syserr(ENOTCONN, "ENOTCONN");
#endif
#ifdef ESHUTDOWN
    set_syserr(ESHUTDOWN, "ESHUTDOWN");
#endif
#ifdef ETOOMANYREFS
    set_syserr(ETOOMANYREFS, "ETOOMANYREFS");
#endif
#ifdef ETIMEDOUT
    set_syserr(ETIMEDOUT, "ETIMEDOUT");
#endif
#ifdef ECONNREFUSED
    set_syserr(ECONNREFUSED, "ECONNREFUSED");
#endif
#ifdef EHOSTDOWN
    set_syserr(EHOSTDOWN, "EHOSTDOWN");
#endif
#ifdef EHOSTUNREACH
    set_syserr(EHOSTUNREACH, "EHOSTUNREACH");
#endif
#ifdef EALREADY
    set_syserr(EALREADY, "EALREADY");
#endif
#ifdef EINPROGRESS
    set_syserr(EINPROGRESS, "EINPROGRESS");
#endif
#ifdef ESTALE
    set_syserr(ESTALE, "ESTALE");
#endif
#ifdef EUCLEAN
    set_syserr(EUCLEAN, "EUCLEAN");
#endif
#ifdef ENOTNAM
    set_syserr(ENOTNAM, "ENOTNAM");
#endif
#ifdef ENAVAIL
    set_syserr(ENAVAIL, "ENAVAIL");
#endif
#ifdef EISNAM
    set_syserr(EISNAM, "EISNAM");
#endif
#ifdef EREMOTEIO
    set_syserr(EREMOTEIO, "EREMOTEIO");
#endif
#ifdef EDQUOT
    set_syserr(EDQUOT, "EDQUOT");
#endif
#ifdef EBADRPC
    set_syserr(EBADRPC, "EBADRPC");
#endif
#ifdef ERPCMISMATCH
    set_syserr(ERPCMISMATCH, "ERPCMISMATCH");
#endif
#ifdef EPROGUNAVAIL
    set_syserr(EPROGUNAVAIL, "EPROGUNAVAIL");
#endif
#ifdef EPROGMISMATCH
    set_syserr(EPROGMISMATCH, "EPROGMISMATCH");
#endif
#ifdef EPROCUNAVAIL
    set_syserr(EPROCUNAVAIL, "EPROCUNAVAIL");
#endif
#ifdef EFTYPE
    set_syserr(EFTYPE, "EFTYPE");
#endif
#ifdef EAUTH
    set_syserr(EAUTH, "EAUTH");
#endif
#ifdef ENEEDAUTH
    set_syserr(ENEEDAUTH, "ENEEDAUTH");
#endif
#ifdef ECANCELED
    set_syserr(ECANCELED, "ECANCELED");
#endif
#ifdef ENOATTR
    set_syserr(ENOATTR, "ENOATTR");
#endif
#ifdef EDOOFUS
    set_syserr(EDOOFUS, "EDOOFUS");
#endif
#ifdef ENOTCAPABLE
    set_syserr(ENOTCAPABLE, "ENOTCAPABLE");
#endif
#ifdef EPROCLIM
    set_syserr(EPROCLIM, "EPROCLIM");
#endif
#ifdef EPRWOFF
    set_syserr(EPWROFF, "EPWROFF");
#endif
#ifdef EDEVERR
    set_syserr(EDEVERR, "EDEVERR");
#endif
#ifdef ENOPOLICY
    set_syserr(ENOPOLICY, "ENOPOLICY");
#endif
#ifdef ENOMEDIUM
    set_syserr(ENOMEDIUM, "ENOMEDIUM");
#endif
#ifdef EMEDIUMTYPE
    set_syserr(EMEDIUMTYPE, "EMEDIUMTYPE");
#endif
#ifdef ENOKEY
    set_syserr(ENOKEY, "ENOKEY");
#endif
#ifdef EKEYEXPIRED
    set_syserr(EKEYEXPIRED, "EKEYEXPIRED");
#endif
#ifdef EKEYREVOKED
    set_syserr(EKEYREVOKED, "EKEYREVOKED");
#endif
#ifdef EKEYREJECTED
    set_syserr(EKEYREJECTED, "EKEYREJECTED");
#endif
#ifdef EOWNERDEAD
    set_syserr(EOWNERDEAD, "EOWNERDEAD");
#endif
#ifdef ENOTRECOVERABLE
    set_syserr(ENOTRECOVERABLE, "ENOTRECOVERABLE");
#endif
#ifdef ERFKILL
    set_syserr(ERFKILL, "ERFKILL");
#endif

  }
};
