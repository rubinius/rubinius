#include <errno.h>

#include "objectmemory.hpp"
#include "vm.hpp"

#include "builtin/access_variable.hpp"
#include "builtin/array.hpp"
#include "builtin/block_environment.hpp"
#include "builtin/bytearray.hpp"
#include "builtin/class.hpp"
#include "builtin/compactlookuptable.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/channel.hpp"
#include "builtin/data.hpp"
#include "builtin/dir.hpp"
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
#include "builtin/staticscope.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/thread.hpp"
#include "builtin/time.hpp"
#include "builtin/tuple.hpp"
#include "builtin/taskprobe.hpp"
#include "builtin/autoload.hpp"
#include "builtin/proc.hpp"
#include "builtin/variable_scope.hpp"
#include "builtin/location.hpp"
#include "builtin/global_cache_entry.hpp"
#include "builtin/weakref.hpp"
#include "builtin/fiber.hpp"
#include "builtin/alias.hpp"

#include "configuration.hpp"
#include "config.h"

#define SPECIAL_CLASS_MASK 0x1f
#define SPECIAL_CLASS_SIZE 32

namespace rubinius {

  /* State is a VM* so, we can just use this in here */
  #define state this

  // Reset macros since we're inside state
#undef G
#undef GO
#define G(whatever) globals().whatever.get()
#define GO(whatever) globals().whatever

  void VM::bootstrap_class() {
    /* Class is created first by hand, and twiddle to setup the internal
       recursion. */
    Class *cls = (Class*)om->allocate_object_raw(sizeof(Class));

    /* We create these 8 classes in a particular way and in a particular
     * order. We need all 8 to create fully initialized Classes and
     * Modules, so we just create them all uninitialized, then initialize
     * them all at once */

    // Class's klass is Class
    cls->klass(state, cls);
    cls->ivars(state, Qnil);
    cls->set_obj_type(ClassType);

    cls->set_object_type(state, ClassType);
    cls->set_class_id(shared.inc_class_count());
    cls->set_packed_size(0);

    // Set Class into the globals
    GO(klass).set(cls);

    // Now do Object
    Class *object = new_basic_class((Class*)Qnil);
    GO(object).set(object);

    object->set_object_type(state, ObjectType);

    // Now Module
    GO(module).set(new_basic_class(object));
    G(module)->set_object_type(state, ModuleType);

    // Fixup Class's superclass to be Module
    cls->superclass(state, G(module));

    // Create Tuple
    GO(tuple).set(new_basic_class(object));
    G(tuple)->set_object_type(state, TupleType);

    // Create LookupTable
    GO(lookuptable).set(new_basic_class(object));
    G(lookuptable)->set_object_type(state, LookupTableType);

    // Create LookupTableBucket
    GO(lookuptablebucket).set(new_basic_class(object));
    G(lookuptablebucket)->set_object_type(state, LookupTableBucketType);

    // Create MethodTable
    GO(methtbl).set(new_basic_class(object));
    G(methtbl)->set_object_type(state, MethodTableType);

    // Create MethodTableBucket
    GO(methtblbucket).set(new_basic_class(object));
    G(methtblbucket)->set_object_type(state, MethodTableBucketType);

    /* Now, we have:
     *  Class
     *  Module
     *  Object
     *  Tuple
     *  LookupTable
     *  LookupTableBucket
     *  MethodTable
     *  MethodTableBucket
     *
     *  With these 8 in place, we can now create fully initialized classes
     *  and modules.
     *
     *  Next we need to finish up the MetaClass protocol (a.k.a. MOP).
     *  The MetaClass of a subclass points to the MetaClass of the superclass.
     */

    // Object's MetaClass instance has Class for a superclass
    Class* mc = MetaClass::attach(this, object, cls);
    // Module's metaclass's superclass is Object's metaclass
    mc = MetaClass::attach(this, G(module), mc);
    // Class's metaclass likewise has Module's metaclass above it
    MetaClass::attach(this, cls, mc);

    // See?
    assert(object->superclass() == Qnil);
    assert(object->klass()->superclass() == cls);

    assert(G(module)->superclass() == object);
    assert(G(module)->klass()->superclass() == object->klass());

    assert(cls->superclass() == G(module));
    assert(cls->klass()->superclass() == G(module)->klass());

    // The other builtin classes get MetaClasses wired to Object's metaclass
    mc = G(object)->metaclass(this);
    MetaClass::attach(this, G(tuple), mc);
    MetaClass::attach(this, G(lookuptable), mc);
    MetaClass::attach(this, G(lookuptablebucket), mc);
    MetaClass::attach(this, G(methtbl), mc);
    MetaClass::attach(this, G(methtblbucket), mc);

    // Now, finish initializing the basic Class/Module
    G(object)->setup(this, "Object");
    G(klass)->setup(this, "Class");
    G(module)->setup(this, "Module");

    // Create the namespace for various implementation classes
    GO(rubinius).set(new_module("Rubinius"));

    // Finish initializing the rest of the special 8
    G(tuple)->setup(this, "Tuple", G(rubinius));
    G(tuple)->name(this, symbol("Rubinius::Tuple"));

    G(lookuptable)->setup(this, "LookupTable", G(rubinius));
    G(lookuptable)->name(this, symbol("Rubinius::LookupTable"));
    G(lookuptablebucket)->setup(this, "Bucket", G(lookuptable));
    G(lookuptablebucket)->name(state, symbol("Rubinius::LookupTable::Bucket"));

    G(methtbl)->setup(this, "MethodTable", G(rubinius));
    G(methtbl)->name(this, symbol("Rubinius::MethodTable"));
    G(methtblbucket)->setup(this, "Bucket", G(methtbl));
    G(methtblbucket)->name(state, symbol("Rubinius::MethodTable::Bucket"));
  }

  void VM::initialize_builtin_classes() {
    // Create the immediate classes.
    GO(nil_class).set(new_class("NilClass"));
    G(nil_class)->set_object_type(state, NilType);
    GO(nil_class)->type_info()->allow_user_allocate = false;

    GO(true_class).set(new_class("TrueClass"));
    G(true_class)->set_object_type(state, TrueType);
    GO(true_class)->type_info()->allow_user_allocate = false;

    GO(false_class).set(new_class("FalseClass"));
    G(false_class)->set_object_type(state, FalseType);
    GO(false_class)->type_info()->allow_user_allocate = false;

    Class* numeric = new_class("Numeric");
    GO(numeric).set(numeric);
    Class* integer = new_class("Integer", numeric);
    GO(integer).set(integer);
    Fixnum::init(this);
    Symbol::init(this);

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

    globals().special_classes[(uintptr_t)Qfalse] = GO(false_class);
    globals().special_classes[(uintptr_t)Qnil  ] = GO(nil_class);
    globals().special_classes[(uintptr_t)Qtrue ] = GO(true_class);

    /* Create IncludedModule */
    GO(included_module).set(new_class("IncludedModule", G(module), G(rubinius)));
    G(included_module)->set_object_type(state, IncludedModuleType);
    G(included_module)->name(this, symbol("Rubinius::IncludedModule"));

    // Let all the builtin classes initialize themselves. This
    // typically means creating a Ruby class.
    Array::init(this);
    ArrayIterator::init(this);
    ByteArray::init(this);
    String::init(this);
    kcode::init(this);
    Executable::init(this);
    CompiledMethod::init(this);
    IO::init(this);
    BlockEnvironment::init(this);
    StaticScope::init(this);
    Dir::init(this);
    CompactLookupTable::init(this);
    Time::init(this);
    Regexp::init(this);
    Bignum::init(this);
    Float::init(this);
    InstructionSequence::init(this);
    List::init(this);
    init_ffi();
    Thread::init(this);
    AccessVariable::init(this);
    Pointer::init(this);
    NativeFunction::init(this);
    TaskProbe::init(this);
    Exception::init(this);
    Data::init(this);
    Autoload::init(this);
    Proc::init(this);
    VariableScope::init(this);
    Location::init(this);

    Channel::init(this);

    NativeMethod::init(this);

    GlobalCacheEntry::init(this);
    WeakRef::init(this);
    Fiber::init(this);
    Alias::init(this);
  }

  // @todo document all the sections of bootstrap_ontology
  /* Creates the rubinius object universe from scratch. */
  void VM::bootstrap_ontology() {

    /*
     * Bootstrap everything so we can create fully initialized
     * Classes.
     */
    bootstrap_class();

    /*
     * Everything is now setup for us to make fully initialized
     * classes.
     */

    /*
     * Create our Rubinius module that we hang stuff off
     */

    initialize_fundamental_constants();

    bootstrap_symbol();
    initialize_builtin_classes();
    bootstrap_exceptions();

    /*
     * Create any 'stock' objects
     */

    Object* main = new_object<Object>(G(object));
    GO(main).set(main);
    G(object)->set_const(this, "MAIN", main); // HACK test hooking up MAIN

    Object* undef = new_object<Object>(G(object));
    GO(undefined).set(undef);

    GO(vm).set(new_class_under("VM", G(rubinius)));
    G(vm)->name(state, state->symbol("Rubinius::VM"));

    Object::bootstrap_methods(this);
    Class::bootstrap_methods(this);
    Module::bootstrap_methods(this);
    StaticScope::bootstrap_methods(this);
    VariableScope::bootstrap_methods(this);

    /*
     * Setup the table we use to store ivars for immediates
     */

    GO(external_ivars).set(LookupTable::create(this));

    initialize_platform_data();
  }

  void VM::initialize_fundamental_constants() {
    if(sizeof(int) == sizeof(long)) {
      G(rubinius)->set_const(state, "L64", Qfalse);
    } else {
      G(rubinius)->set_const(state, "L64", Qtrue);
    }

    G(rubinius)->set_const(state, "WORDSIZE", Fixnum::from(sizeof(void*) * 8));
  }

  void VM::initialize_platform_data() {
    // HACK test hooking up IO
    IO* in_io  = IO::create(state, fileno(stdin));
    IO* out_io = IO::create(state, fileno(stdout));
    IO* err_io = IO::create(state, fileno(stderr));

    out_io->sync(state, Qtrue);
    err_io->sync(state, Qtrue);

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
    G(rubinius)->set_const(state, "BIN_PATH", String::create(state, RBX_BIN_PATH));
    G(rubinius)->set_const(state, "LIB_PATH", String::create(state, RBX_LIB_PATH));
    G(rubinius)->set_const(state, "EXT_PATH", String::create(state, RBX_EXT_PATH));
    G(rubinius)->set_const(state, "HDR_PATH", String::create(state, RBX_HDR_PATH));
    G(rubinius)->set_const(state, "GEMS_PATH", String::create(state, RBX_GEMS_PATH));
    G(rubinius)->set_const(state, "SITE_PATH", String::create(state, RBX_SITE_PATH));
    G(rubinius)->set_const(state, "VENDOR_PATH", String::create(state, RBX_VENDOR_PATH));

    G(rubinius)->set_const(state, "VERSION", String::create(state, RBX_VERSION));
    G(rubinius)->set_const(state, "LIB_VERSION", String::create(state, RBX_LIB_VERSION));
    G(rubinius)->set_const(state, "BUILD_REV", String::create(state, RBX_BUILD_REV));
    G(rubinius)->set_const(state, "RELEASE_DATE", String::create(state, RBX_RELEASE_DATE));
    G(rubinius)->set_const(state, "LDSHARED", String::create(state, RBX_LDSHARED));

    G(rubinius)->set_const(state, "HOST", String::create(state, RBX_HOST));
    G(rubinius)->set_const(state, "CPU", String::create(state, RBX_CPU));
    G(rubinius)->set_const(state, "VENDOR", String::create(state, RBX_VENDOR));
    G(rubinius)->set_const(state, "OS", String::create(state, RBX_OS));

#ifdef RBX_LITTLE_ENDIAN
    G(rubinius)->set_const(state, "ENDIAN", symbol("little"));
#else
    G(rubinius)->set_const(state, "ENDIAN", symbol("big"));
#endif

    // Used in Array.pack
    G(rubinius)->set_const(state, "SIZEOF_SHORT", Fixnum::from(sizeof(short)));
    G(rubinius)->set_const(state, "SIZEOF_INT", Fixnum::from(sizeof(int)));
    G(rubinius)->set_const(state, "SIZEOF_LONG", Fixnum::from(sizeof(long)));
  }

  void VM::bootstrap_symbol() {
#define add_sym(name) GO(sym_ ## name).set(symbol(#name))
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
    add_sym(coerce_into_array);
#undef add_sym
    GO(sym_s_method_added).set(symbol("singleton_method_added"));
    GO(sym_init_copy).set(symbol("initialize_copy"));
    GO(sym_plus).set(symbol("+"));
    GO(sym_minus).set(symbol("-"));
    GO(sym_equal).set(symbol("=="));
    GO(sym_nequal).set(symbol("!="));
    GO(sym_tequal).set(symbol("==="));
    GO(sym_lt).set(symbol("<"));
    GO(sym_gt).set(symbol(">"));
  }

  void VM::setup_errno(int num, const char* name, Class* sce, Module* ern) {
    bool found = false;

    Object* key = Fixnum::from(num);

    Object* current = state->globals().errno_mapping->fetch(state, key, &found);
    if(found) {
      ern->set_const(state, symbol(name), current);
    } else {
      Class* cls = state->new_class(name, sce, ern);

      // new_class has simply name setting logic that doesn't take into account
      // being not under Object. So we set it again using the smart method.
      cls->set_name(state, ern, state->symbol(name));

      cls->set_const(state, symbol("Errno"), key);
      cls->set_const(state, symbol("Strerror"), String::create(state, strerror(num)));
      state->globals().errno_mapping->store(state, key, cls);
    }
  }

  void VM::bootstrap_exceptions() {
    Class *exc, *scp, *std, *arg, *nam, *loe, *rex, *stk, *sxp, *sce, *type, *lje, *vme;
    Class* rng;

#define dexc(name, sup) new_class(#name, sup)

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
    dexc(RuntimeError, std);
    sce = dexc(SystemCallError, std);
    stk = dexc(StackError, exc);
    sxp = dexc(StackExploded, stk);
    lje = dexc(LocalJumpError, std);
    rng = dexc(RangeError, std);
    dexc(FloatDomainError, rng);
    dexc(ZeroDivisionError, std);
    dexc(IOError, std);

    GO(jump_error).set(lje);

    GO(exc_vm_internal).set(new_class("Internal", exc, G(rubinius)));

    // Some special exceptions scoped under the Rubinius module
    vme = new_class("VMException", exc, G(rubinius));
    new_class("AssertionError", vme, G(rubinius));
    new_class("ObjectBoundsExceededError", vme, G(rubinius));

    // Create the stack error object now, since we probably wont be
    // able to later.
    GO(stack_error).set(new_object<Exception>(stk));

    GO(exc_type).set(type);
    GO(exc_arg).set(arg);
    GO(exc_loe).set(loe);
    GO(exc_rex).set(rex);

    GO(exc_stack_explosion).set(sxp);
    GO(exc_primitive_failure).set(dexc(PrimitiveFailure, exc));

    GO(exc_segfault).set(dexc(MemorySegmentionError, exc));

    Module* ern = new_module("Errno");

    GO(errno_mapping).set(LookupTable::create(state));

    ern->set_const(state, symbol("Mapping"), G(errno_mapping));

#define set_syserr(num, name) setup_errno(num, name, sce, ern)

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

  }
};
