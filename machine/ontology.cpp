#include <sys/ioctl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "memory.hpp"
#include "thread_state.hpp"

#include "class/access_variable.hpp"
#include "class/array.hpp"
#include "class/basic_object.hpp"
#include "class/block_environment.hpp"
#include "class/byte_array.hpp"
#include "class/call_site.hpp"
#include "class/character.hpp"
#include "class/class.hpp"
#include "class/code_db.hpp"
#include "class/compact_lookup_table.hpp"
#include "class/compiled_code.hpp"
#include "class/channel.hpp"
#include "class/data.hpp"
#include "class/dir.hpp"
#include "class/encoding.hpp"
#include "class/executable.hpp"
#include "class/fixnum.hpp"
#include "class/float.hpp"
#include "class/fsevent.hpp"
#include "class/immediates.hpp"
#include "class/io.hpp"
#include "class/iseq.hpp"
#include "class/jit.hpp"
#include "class/list.hpp"
#include "class/logger.hpp"
#include "class/lookup_table.hpp"
#include "class/method_table.hpp"
#include "class/ffi_pointer.hpp"
#include "class/native_function.hpp"
#include "class/native_method.hpp"
#include "class/regexp.hpp"
#include "class/lexical_scope.hpp"
#include "class/constant_table.hpp"
#include "class/string.hpp"
#include "class/symbol.hpp"
#include "class/system.hpp"
#include "class/thread.hpp"
#include "class/time.hpp"
#include "class/stat.hpp"
#include "class/trie.hpp"
#include "class/tuple.hpp"
#include "class/autoload.hpp"
#include "class/proc.hpp"
#include "class/variable_scope.hpp"
#include "class/location.hpp"
#include "class/constant_cache.hpp"
#include "class/weakref.hpp"
#include "class/fiber.hpp"
#include "class/alias.hpp"
#include "class/randomizer.hpp"
#include "class/module.hpp"
#include "class/class.hpp"
#include "class/atomic.hpp"
#include "class/unwind_state.hpp"
#include "class/unwind_site.hpp"

#include "environment.hpp"
#include "configuration.hpp"
#include "config.h"
#include "paths.h"
#include "release.h"

#include "ontology.hpp"

#define SPECIAL_CLASS_MASK 0x1f
#define SPECIAL_CLASS_SIZE 32

namespace rubinius {
  // Reset macros since we're inside state
#undef G
#undef GO
#define G(whatever) state->globals().whatever.get()
#define GO(whatever) state->globals().whatever

  void ThreadState::bootstrap_class(STATE) {
    /* BasicObject, Object, Module, Class all have .class == Class.
     * The superclass chain is Class < Module < Object < BasicObject < nil.
     *
     * We have to build this in pieces because there are obviously
     * cross-dependencies. Additionally, creating a new normal class requires
     * a few more kinds of objects, like ConstantTable and MethodTable.
     */
    MemoryHeader::bootstrap(state);
    Class::bootstrap(state);
    BasicObject::bootstrap(state);
    Object::bootstrap(state);
    Module::bootstrap(state);

    assert(G(basicobject)->klass() == G(klass));
    assert(G(basicobject)->get_type() == ClassType);

    assert(G(object)->klass() == G(klass));
    assert(G(object)->get_type() == ClassType);

    assert(G(module)->klass() == G(klass));
    assert(G(module)->get_type() == ClassType);

    assert(G(klass)->klass() == G(klass));
    assert(G(klass)->get_type() == ClassType);

    Tuple::bootstrap(state);
    Array::bootstrap(state);
    WeakRef::bootstrap(state);
    LookupTable::bootstrap(state);
    ConstantTable::bootstrap(state);
    MethodTable::bootstrap(state);

    Module::bootstrap(state, G(object), G(object), "Object");
    Module::bootstrap(state, G(basicobject), G(object), "BasicObject");
    Module::bootstrap(state, G(klass), G(object), "Class");
    Module::bootstrap(state, G(module), G(object), "Module");
    Module::bootstrap(state, G(array), G(object), "Array");
    Module::bootstrap(state, G(cls_weakref), G(object), "WeakRef");

    G(basicobject)->set_const(state, "BasicObject", G(basicobject));

    // Create the namespace for various implementation classes
    GO(rubinius).set(state->memory()->new_module<Module>(state, "Rubinius"));
    GO(runtime).set(state->memory()->new_module<Module>(state, G(rubinius), "Runtime"));

    Module::bootstrap(state, G(tuple), G(rubinius), "Tuple");
    Module::bootstrap(state, G(rtuple), G(rubinius), "RTuple");
    Module::bootstrap(state, G(constant_table), G(rubinius), "ConstantTable");
    Module::bootstrap(state, G(constant_table_bucket), G(constant_table), "Bucket");
    Module::bootstrap(state, G(lookup_table), G(rubinius), "LookupTable");
    Module::bootstrap(state, G(lookup_table_bucket), G(lookup_table), "Bucket");
    Module::bootstrap(state, G(method_table), G(rubinius), "MethodTable");
    Module::bootstrap(state, G(method_table_bucket), G(method_table), "Bucket");

    // Set up singleton classes.
    SingletonClass::attach(state, G(basicobject), G(klass));
    Class* sc = SingletonClass::attach(state, G(object), G(basicobject)->klass());
    sc = SingletonClass::attach(state, G(module), sc);
    SingletonClass::attach(state, G(klass), sc);

    /* Verify the basic class hierarchy. We do this here because if you mess
     * this up, good luck even booting the machine.
     */
    assert(G(basicobject)->klass()->klass() == G(klass));
    assert(G(object)->klass()->klass() == G(klass));
    assert(G(module)->klass()->klass() == G(klass));
    assert(G(klass)->klass()->klass() == G(klass));

    assert(G(object)->superclass()->klass()->get_type() == SingletonClassType);
    assert(G(module)->superclass()->klass()->get_type() == SingletonClassType);
    assert(G(klass)->superclass()->klass()->get_type() == SingletonClassType);

    assert(G(object)->superclass()->klass() == G(basicobject)->klass());
    assert(G(module)->superclass()->klass() == G(object)->klass());
    assert(G(klass)->superclass()->klass() == G(module)->klass());

    assert(G(basicobject)->klass()->superclass() == G(klass));
    assert(G(object)->klass()->superclass() == G(basicobject)->klass());
    assert(G(module)->klass()->superclass() == G(object)->klass());
    assert(G(klass)->klass()->superclass() == G(module)->klass());

    assert(G(basicobject)->klass()->superclass()->klass() == G(klass)->klass());
    assert(G(object)->klass()->superclass()->klass() == G(klass));
    assert(G(module)->klass()->superclass()->klass() == G(klass));
    assert(G(klass)->klass()->superclass()->klass() == G(klass));

    assert(G(basicobject)->superclass()->nil_p());
    assert(G(object)->superclass() == G(basicobject));
    assert(G(module)->superclass() == G(object));
    assert(G(klass)->superclass() == G(module));

    // The other builtin classes get SingletonClasses wired to Object's singleton class
    sc = G(object)->singleton_class(state);
    SingletonClass::attach(state, G(tuple), sc);
    SingletonClass::attach(state, G(array), sc);
    SingletonClass::attach(state, G(cls_weakref), sc);
    SingletonClass::attach(state, G(lookup_table), sc);
    SingletonClass::attach(state, G(lookup_table_bucket), sc);
    SingletonClass::attach(state, G(constant_table), sc);
    SingletonClass::attach(state, G(constant_table_bucket), sc);
    SingletonClass::attach(state, G(method_table), sc);
    SingletonClass::attach(state, G(method_table_bucket), sc);
  }

  void ThreadState::initialize_builtin_classes(STATE) {
    // Create the immediate classes.
    GO(nil_class).set(state->memory()->new_class<Class, NilClass>(state, "NilClass"));
    GO(true_class).set(state->memory()->new_class<Class, TrueClass>(state, "TrueClass"));
    GO(false_class).set(state->memory()->new_class<Class, FalseClass>(
          state, "FalseClass"));

    Numeric::bootstrap(state);
    Integer::bootstrap(state);
    Fixnum::bootstrap(state);
    Symbol::bootstrap(state);

    // Setup the special_class lookup table. We use this to resolve
    // the classes for Fixnum's, nil, true and false.
    for(size_t i = 0; i < SPECIAL_CLASS_SIZE; i++) {
      if(SYMBOL_P(i)) {
        state->globals().special_classes[i] = GO(symbol);
      } else if(FIXNUM_P(i)) {
        state->globals().special_classes[i] = GO(fixnum_class);
      } else {
        state->globals().special_classes[i] = GO(object); /* unused slot */
      }
    }

    state->globals().special_classes[(uintptr_t)cFalse] = GO(false_class);
    state->globals().special_classes[(uintptr_t)cNil  ] = GO(nil_class);
    state->globals().special_classes[(uintptr_t)cTrue ] = GO(true_class);

    /* Create IncludedModule */
    GO(included_module).set(state->memory()->new_class<Class, IncludedModule>(
          state, G(module), G(rubinius), "IncludedModule"));

    // Let all the builtin classes initialize themselves. this
    // typically means creating a Ruby class.
    CompactLookupTable::bootstrap(state);
    ByteArray::bootstrap(state);
    String::bootstrap(state);
    Character::bootstrap(state);
    Executable::bootstrap(state);
    CompiledCode::bootstrap(state);
    AtomicReference::bootstrap(state);
    IO::bootstrap(state);
    BlockEnvironment::bootstrap(state);
    LexicalScope::bootstrap(state);
    Dir::bootstrap(state);
    Time::bootstrap(state);
    Stat::bootstrap(state);
    Regexp::bootstrap(state);
    Bignum::bootstrap(state);
    Float::bootstrap(state);
    InstructionSequence::bootstrap(state);
    List::bootstrap(state);
    init_ffi(state);
    Thread::bootstrap(state);
    UnwindState::bootstrap(state);
    AccessVariable::bootstrap(state);
    Pointer::bootstrap(state);
    NativeFunction::bootstrap(state);
    Exception::bootstrap(state);
    Data::bootstrap(state);
    Autoload::bootstrap(state);
    Proc::bootstrap(state);
    VariableScope::bootstrap(state);
    Location::bootstrap(state);

    Channel::bootstrap(state);

    NativeMethod::bootstrap(state);

    Prediction::bootstrap(state);
    CallSite::bootstrap(state);
    ConstantCache::bootstrap(state);
    Fiber::bootstrap(state);
    Alias::bootstrap(state);
    Randomizer::bootstrap(state);
    Encoding::bootstrap(state);
    FSEvent::bootstrap(state);
    FDSet::bootstrap(state);
    RIOStream::bootstrap(state);
    Logger::bootstrap(state);
    JIT::bootstrap(state);
    CodeDB::bootstrap(state);
    Trie::bootstrap(state);
    UnwindSite::bootstrap(state);
  }

  // @todo document all the sections of bootstrap_ontology
  /* Creates the rubinius object universe from scratch. */
  void ThreadState::bootstrap_ontology(STATE) {

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

    Object* undef = state->memory()->new_object<Object>(state, G(object));
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

    Object* main = state->memory()->new_object<Object>(state, G(object));
    GO(main).set(main);
    G(object)->set_const(state, "MAIN", main); // HACK test hooking up MAIN

    GO(vm_class).set(state->memory()->new_class<Class>(state, G(rubinius), "VM"));

    GO(type).set(state->memory()->new_module<Module>(state, G(rubinius), "Type"));
    GO(mirror).set(state->memory()->new_class<Class>(state, G(rubinius), "Mirror"));

    System::bootstrap_methods(state);
    Module::bootstrap_methods(state);
    LexicalScope::bootstrap_methods(state);
    VariableScope::bootstrap_methods(state);
    BlockEnvironment::bootstrap_methods(state);

    /*
     * Setup the table we use to store ivars for immediates
     */

    GO(external_ivars).set(LookupTable::create(state));

    initialize_platform_data(state);

    MachineCode::bootstrap(state);
  }

  void ThreadState::initialize_fundamental_constants(STATE) {
    if(sizeof(int) == sizeof(long)) {
      G(rubinius)->set_const(state, "L64", cFalse);
    } else {
      G(rubinius)->set_const(state, "L64", cTrue);
    }

    G(rubinius)->set_const(state, "WORDSIZE", Fixnum::from(sizeof(void*) * 8));
  }

  void ThreadState::initialize_platform_data(STATE) {
    /* Hook up stub IO class so we can begin bootstrapping. STDIN/OUT/ERR will be
     * replaced in core/zed.rb with the pure Ruby IO objects.
     */
    IO* in_io  = IO::create(state, STDIN_FILENO);
    IO* out_io = IO::create(state, STDOUT_FILENO);
    IO* err_io = IO::create(state, STDERR_FILENO);

    G(object)->set_const(state, "STDIN",  in_io);
    G(object)->set_const(state, "STDOUT", out_io);
    G(object)->set_const(state, "STDERR", err_io);

    /* Fundamental constants. These are uniformly Strings and not symbols
     * because some are passed to e.g. File.expand_path and having them
     * be uniform is simpler.
     */
    Environment* env = state->environment();

    if(env) {
      std::string prefix = env->system_prefix();
      G(rubinius)->set_const(state, "PREFIX_PATH", String::create(state, prefix.c_str()));
      std::string path = prefix + RBX_RUNTIME_PATH;
      G(rubinius)->set_const(state, "RUNTIME_PATH", String::create(state, path.c_str()));
      path = prefix + RBX_CODEDB_PATH;
      G(rubinius)->set_const(state, "CODEDB_PATH", String::create(state, path.c_str()));
      path = prefix + RBX_BIN_PATH;
      G(rubinius)->set_const(state, "BIN_PATH", String::create(state, path.c_str()));
      path = prefix + RBX_CORE_PATH;
      G(rubinius)->set_const(state, "CORE_PATH", String::create(state, path.c_str()));
      path = prefix + RBX_LIB_PATH;
      G(rubinius)->set_const(state, "LIB_PATH", String::create(state, path.c_str()));
      path = prefix + RBX_ENC_PATH;
      G(rubinius)->set_const(state, "ENC_PATH", String::create(state, path.c_str()));
      path = prefix + RBX_SITE_PATH;
      G(rubinius)->set_const(state, "SITE_PATH", String::create(state, path.c_str()));
      path = prefix + RBX_ARCH_PATH;
      G(rubinius)->set_const(state, "ARCH_PATH", String::create(state, path.c_str()));
      path = prefix + RBX_VENDOR_PATH;
      G(rubinius)->set_const(state, "VENDOR_PATH", String::create(state, path.c_str()));

      if(char* gems_path = getenv("RBX_GEMS_PATH")) {
        G(rubinius)->set_const(state, "GEMS_PATH", String::create(state, gems_path));
      } else {
        path = prefix + RBX_GEMS_PATH;
        G(rubinius)->set_const(state, "GEMS_PATH", String::create(state, path.c_str()));
      }

      path = prefix + RBX_HDR_PATH;
      G(rubinius)->set_const(state, "HDR_PATH", String::create(state, path.c_str()));
    }

    G(rubinius)->set_const(state, "PROGRAM_NAME", String::create(state, RBX_PROGRAM_NAME));
    G(rubinius)->set_const(state, "RUBY_VERSION", String::create(state, RBX_RUBY_VERSION));
    G(rubinius)->set_const(state, "ENGINE_VERSION", String::create(state, RBX_ENGINE_VERSION));
    G(rubinius)->set_const(state, "LLVM_VERSION", String::create(state, RBX_LLVM_VERSION));
    G(rubinius)->set_const(state, "VERSION", String::create(state, RBX_VERSION));
    G(rubinius)->set_const(state, "LIB_VERSION", String::create(state, RBX_LIB_VERSION));
    G(rubinius)->set_const(state, "BUILD_REV", String::create(state, RBX_BUILD_REV));
    G(rubinius)->set_const(state, "RELEASE_DATE", String::create(state, RBX_RELEASE_DATE));
    G(rubinius)->set_const(state, "DEBUG_BUILD", RBOOL(RBX_DEBUG_BUILD));
    G(rubinius)->set_const(state, "PROFILER",
        RBOOL(state->configuration()->diagnostics_profiler_enabled));
    G(rubinius)->set_const(state, "LDSHARED", String::create(state, RBX_LDSHARED));
    G(rubinius)->set_const(state, "LDSHAREDXX", String::create(state, RBX_LDSHAREDXX));

    G(rubinius)->set_const(state, "Signature", String::create(state, RBX_SIGNATURE));

    G(rubinius)->set_const(state, "HOST", String::create(state, RBX_HOST));
    G(rubinius)->set_const(state, "CPU", String::create(state, RBX_CPU));
    G(rubinius)->set_const(state, "VENDOR", String::create(state, RBX_VENDOR));
    G(rubinius)->set_const(state, "OS", String::create(state, RBX_OS));

    G(rubinius)->set_const(state, "RUBY_LIB_VERSION", Fixnum::from(RBX_RUBY_LIB_VERSION));

    G(rubinius)->set_const(state, "LIBC", String::create(state, RBX_LIBC));

    G(rubinius)->set_const(state, "HAVE_LCHMOD", RBX_HAVE_LCHMOD ? cTrue : cFalse);
    G(rubinius)->set_const(state, "HAVE_LCHOWN", RBX_HAVE_LCHOWN ? cTrue : cFalse);
    G(rubinius)->set_const(state, "HAVE_MKFIFO", RBX_HAVE_MKFIFO ? cTrue : cFalse);

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

  void ThreadState::bootstrap_symbol(STATE) {
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
    add_sym(to_hash);
#undef add_sym
    GO(sym_respond_to_missing).set(state->symbol("respond_to_missing?"));
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
    GO(sym_keyword_object).set(state->symbol("keyword_object?"));
  }

  void ThreadState::setup_errno(STATE, int num, const char* name, Class* sce, Module* ern) {
    bool found = false;

    Object* key = Fixnum::from(num);

    Object* current = state->globals().errno_mapping->fetch(state, key, &found);
    if(found) {
      ern->set_const(state, state->symbol(name), current);
    } else {
      Class* cls = state->memory()->new_class<Class>(state, sce, ern, name);

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

  void ThreadState::bootstrap_exceptions(STATE) {
    Class *exc, *scp, *std, *arg, *nam, *loe, *rex, *stk, *sce, *type, *lje, *vme, *me, *cue;
    Class *rng, *rte;

#define dexc(name, sup) state->memory()->new_class<Class>(state, sup, G(object), #name)

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
    stk = dexc(SystemStackError, exc);
    lje = dexc(LocalJumpError, std);
    rng = dexc(RangeError, std);
    dexc(FloatDomainError, rng);
    dexc(ZeroDivisionError, std);
    dexc(IOError, std);
    dexc(InterpreterError, exc);
    dexc(DeadlockError, exc);

    GO(jump_error).set(lje);

    GO(exc_vm_internal).set(state->memory()->new_class<Class>(
          state, exc, G(rubinius), "Internal"));
    GO(exc_vm_bad_bytecode).set(state->memory()->new_class<Class>(
          state, G(exc_vm_internal), G(rubinius), "InvalidBytecode"));

    // Some special exceptions scoped under the Rubinius module
    me = state->memory()->new_class<Class>(state, exc, G(rubinius), "MachineException");
    cue = state->memory()->new_class<Class>(state, me, G(rubinius), "ConcurrentUpdateError");
    vme = state->memory()->new_class<Class>(state, exc, G(rubinius), "VMException");
    state->memory()->new_class<Class>(state, vme, G(rubinius), "AssertionError");
    state->memory()->new_class<Class>(
        state, vme, G(rubinius), "ObjectBoundsExceededError");

    // The stack_error mechanisms assume that there will be enough
    // space left over to allocate the actual exception.
    GO(stack_error).set(stk);

    GO(exc_type).set(type);
    GO(exc_arg).set(arg);
    GO(exc_loe).set(loe);
    GO(exc_rex).set(rex);
    GO(exc_rte).set(rte);
    GO(exc_cue).set(cue);

    GO(exc_primitive_failure).set(dexc(PrimitiveFailure, exc));

    GO(exc_segfault).set(dexc(MemorySegmentionError, exc));

    Module* ern = state->memory()->new_module<Module>(state, "Errno");

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
#ifdef ENOTSUP
    set_syserr(ENOTSUP, "ENOTSUP");
#endif

  }
};
