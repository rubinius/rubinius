#include <errno.h>

#include "objects.hpp"
#include "objectmemory.hpp"
#include "vm.hpp"

#include "builtin/array.hpp"
#include "builtin/block_environment.hpp"
#include "builtin/class.hpp"
#include "builtin/compactlookuptable.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/dir.hpp"
#include "builtin/executable.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/float.hpp"
#include "builtin/hash.hpp"
#include "builtin/io.hpp"
#include "builtin/iseq.hpp"
#include "builtin/list.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/methodtable.hpp"
#include "builtin/regexp.hpp"
#include "builtin/selector.hpp"
#include "builtin/sendsite.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/task.hpp"
#include "builtin/thread.hpp"
#include "builtin/tuple.hpp"

#define SPECIAL_CLASS_MASK 0x1f
#define SPECIAL_CLASS_SIZE 32
#define CUSTOM_CLASS GO(object)

namespace rubinius {

  native_int Integer::to_native() {
    if(fixnum_p()) {
      return ((FIXNUM)this)->to_native();
    }

    return as<Bignum>(this)->to_native();
  }

  // TODO: double check that this links. Evan says it doesn't. I'll
  // check my Meiers books when I get home
  template <>
  static bool kind_of<Numeric>(OBJECT obj) {
    return obj->fixnum_p() ||
      (obj->reference_p() && (obj->obj_type == Bignum::type ||
                              obj->obj_type == Float::type));
  }

  /* State is a VM* so, we can just use this in here */
  #define state this

  Class *VM::new_basic_class(OBJECT sup, size_t fields) {
    Class *cls = (Class*)om->new_object(G(klass), Class::fields);
    cls->instance_fields = Fixnum::from(fields);
    cls->has_ivars = Qtrue;
    cls->instance_type = Fixnum::from(ObjectType);
    SET(cls, superclass, sup);

    return cls;
  }

  Class *VM::new_class(OBJECT sup, size_t fields) {
    Class *cls = new_basic_class(sup, fields);
    MetaClass::attach(this, cls);
    return cls;
  }

  Class* VM::new_class(const char* name) {
    return new_class(name, G(object), G(object)->instance_fields->to_native());
  }

  Class* VM::new_class(const char* name, size_t fields) {
    return new_class(name, G(object), fields);
  }

  Class* VM::new_class(const char* name, OBJECT sup, size_t fields) {
    return new_class(name, sup, fields, G(object));
  }

  Class* VM::new_class(const char* name, OBJECT sup, size_t fields, Module* under) {
    Class* cls = new_class(sup, fields);
    cls->setup(this, name, under);
    return cls;
  }

  Module* VM::new_module(const char* name, Module* under) {
    Module *mod = (Module*)om->new_object(G(module), Module::fields);
    mod->setup(this, name, under);
    return mod;
  }

  /* Creates the rubinius object universe from scratch. */
  void VM::bootstrap_ontology() {
    /* Class is created first by hand, and twittle to setup the internal
       recursion. */
#undef G
#undef GO
#define G(whatever) globals.whatever.get()
#define GO(whatever) globals.whatever

    Class *cls = (Class*)om->allocate_object(Class::fields);
    cls->klass = cls;

    cls->instance_fields = Fixnum::from(Class::fields);
    cls->has_ivars = Qtrue;
    cls->set_object_type(ClassType);
    cls->obj_type = ClassType;

    GO(klass).set(cls);

    Class *object = new_basic_class(Qnil, Object::fields);
    GO(object).set(object);

    GO(module).set(new_basic_class(object, Module::fields));
    G(module)->set_object_type(ModuleType);

    cls->superclass = G(module);

    GO(metaclass).set(new_basic_class(cls, MetaClass::fields));
    G(metaclass)->instance_type = Fixnum::from(MetaclassType);

    GO(tuple).set(new_basic_class(object, Tuple::fields));
    G(tuple)->instance_type = Fixnum::from(TupleType);
    G(tuple)->has_ivars = Qfalse;

    GO(lookuptable).set(new_basic_class(object, LookupTable::fields));
    G(lookuptable)->instance_type = Fixnum::from(LookupTableType);

    GO(methtbl).set(new_basic_class(G(lookuptable), MethodTable::fields));
    G(methtbl)->instance_type = Fixnum::from(MTType);

    OBJECT mc = MetaClass::attach(this, object, cls);
    mc = MetaClass::attach(this, G(module), mc);
    MetaClass::attach(this, cls, mc);

    MetaClass::attach(this, G(metaclass));
    MetaClass::attach(this, G(tuple));
    MetaClass::attach(this, G(lookuptable));
    MetaClass::attach(this, G(methtbl));

    GO(symbol).set(new_class(object, 0));
    GO(array).set(new_class(object, Array::fields));
    G(array)->instance_type = Fixnum::from(ArrayType);
    G(array)->has_ivars = Qfalse;

    GO(bytearray).set(new_class(object, 0));
    G(bytearray)->instance_type = Fixnum::from(ByteArrayType);

    GO(string).set(new_class(object, String::fields));
    G(string)->instance_type = Fixnum::from(StringType);
    G(string)->has_ivars = Qfalse;

    GO(symtbl).set(new_class(object, SymbolTable::fields));

    GO(executable).set(new_class(object, Executable::fields));

    GO(cmethod).set(new_class(G(executable), CompiledMethod::fields));
    G(cmethod)->instance_type = Fixnum::from(CMethodType);

    GO(hash).set(new_class(object, Hash::fields));
    G(hash)->instance_type = Fixnum::from(HashType);

    GO(io).set(new_class(object, IO::fields));

    GO(blokenv).set(new_class(object, BlockEnvironment::fields));
    G(blokenv)->instance_type = Fixnum::from(BlockEnvType);

    GO(staticscope).set(new_class(object, StaticScope::fields));
    G(staticscope)->set_object_type(StaticScopeType);

    GO(dir).set(new_class(object, Dir::fields));
    G(dir)->instance_type = Fixnum::from(DirType);

    GO(compactlookuptable).set(new_class(G(tuple), CompactLookupTable::fields));
    G(compactlookuptable)->instance_type = Fixnum::from(CompactLookupTableType);

    bootstrap_symbol();

    G(object)->setup(this, "Object");
    G(klass)->setup(this, "Class");
    G(module)->setup(this, "Module");
    G(metaclass)->setup(this, "MetaClass");
    G(symbol)->setup(this, "Symbol");
    G(tuple)->setup(this, "Tuple");
    G(array)->setup(this, "Array");
    G(bytearray)->setup(this, "ByteArray");
    G(hash)->setup(this, "Hash");
    G(lookuptable)->setup(this, "LookupTable");
    G(string)->setup(this, "String");
    G(symtbl)->setup(this, "SymbolTable");
    G(methtbl)->setup(this, "MethodTable");
    G(executable)->setup(this, "Executable");
    G(cmethod)->setup(this, "CompiledMethod");
    G(io)->setup(this, "IO");
    G(blokenv)->setup(this, "BlockEnvironment");
    G(staticscope)->setup(this, "StaticScope");
    G(symbol)->setup(this, "Symbol");
    G(dir)->setup(this, "Dir");
    G(compactlookuptable)->setup(this, "CompactLookupTable");

    G(object)->set_const(this, symbol("Symbols"), G(symbols));

    GO(nil_class).set(new_class("NilClass", object, 0));
    GO(true_class).set(new_class("TrueClass", object, 0));
    GO(false_class).set(new_class("FalseClass", object, 0));

    Class* numeric = new_class("Numeric", object, 0);
    Class* integer = new_class("Integer", numeric, 0);
    GO(fixnum_class).set(new_class("Fixnum", integer, 0));
    G(fixnum_class)->instance_type = Fixnum::from(FixnumType);

    GO(bignum).set(new_class("Bignum", integer, 0));
    G(bignum)->instance_type = Fixnum::from(BignumType);
    Bignum::init(this);

    GO(floatpoint).set(new_class("Float", numeric, 0));
    G(floatpoint)->instance_type = Fixnum::from(FloatType);

    GO(methctx).set(new_class("MethodContext", object, 0));
    G(methctx)->set_object_type(MContextType);

    GO(blokctx).set(new_class("BlockContext", G(methctx), 0));
    G(blokctx)->set_object_type(BContextType);

    GO(task).set(new_class("Task", object, 0));
    G(task)->instance_type = Fixnum::from(TaskType);

    GO(iseq).set(new_class("InstructionSequence", G(object), InstructionSequence::fields));
    G(iseq)->instance_type = Fixnum::from(ISeqType);

    for(size_t i = 0; i < SPECIAL_CLASS_SIZE; i += 4) {
      globals.special_classes[i + 0] = GO(object); /* unused slot */
      globals.special_classes[i + 1] = GO(fixnum_class);
      globals.special_classes[i + 2] = GO(object); /* unused slot */
      if(((i + 3) & 0x7) == 0x3) {
        globals.special_classes[i + 3] = GO(symbol);
      } else {
        globals.special_classes[i + 3] = CUSTOM_CLASS;
      }
    }

    globals.special_classes[(uintptr_t)Qundef] = GO(object); /* unused slot */
    globals.special_classes[(uintptr_t)Qfalse] = GO(false_class);
    globals.special_classes[(uintptr_t)Qnil  ] = GO(nil_class);
    globals.special_classes[(uintptr_t)Qtrue ] = GO(true_class);

    GO(main).set(om->new_object(G(object), 1));

    Regexp::init(this);

    GO(cmethod_vis).set(new_class("CompiledMethod::Visibility", G(object),
        MethodVisibility::fields, G(cmethod)));
    G(cmethod_vis)->set_object_type(CMVisibilityType);

    Module* x = new_module("Rubinius");
    GO(vm).set(new_module("VM", x));

    bootstrap_exceptions();

    GO(external_ivars).set(LookupTable::create(state));

    IO::init(state);
    List::init(state);
    SendSite::init(state);
    Selector::init(state);
    init_ffi();
    Task::init(state);
    Thread::init(state);

    TypeInfo::init(state);
  }

  void VM::bootstrap_symbol() {
    GO(symbols).set(SymbolTable::create(this));
#define add_sym(name) GO(sym_ ## name).set(symbol(#name))
    add_sym(object_id);
    add_sym(method_missing);
    add_sym(inherited);
    add_sym(opened_class);
    add_sym(from_literal);
    add_sym(method_added);
    add_sym(send);
    add_sym(public);
    add_sym(private);
    add_sym(protected);
    add_sym(const_missing);
    add_sym(object_id);
    add_sym(call);
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

  void VM::bootstrap_exceptions() {
    int sz;
    sz = 3;

    Class *exc, *scp, *std, *arg, *nam, *loe, *rex, *stk, *sxp, *sce, *type, *lje, *vm;
    Class* fce;

#define dexc(name, sup) new_class(#name, sup, sz)

    exc = dexc(Exception, G(object));
    GO(exception).set(exc);
    dexc(fatal, exc);
    vm = dexc(VMError, exc);
    dexc(VMAssertion, vm);
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
    dexc(IllegalLongReturn, lje);

    fce = dexc(FlowControlException, exc);
    dexc(ReturnException, fce);
    dexc(LongReturnException, fce);

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

    sz = 4;

#define set_syserr(num, name) ({ \
    Class* _cls = new_class(name, sce, sz, ern); \
    _cls->set_const(state, symbol("Errno"), Fixnum::from(num)); \
    G(errno_mapping)->store(state, Fixnum::from(num), _cls); \
    })

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
