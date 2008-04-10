#include <errno.h>

#include "objects.hpp"
#include "objectmemory.hpp"
#include "vm.hpp"

#define SPECIAL_CLASS_MASK 0x1f
#define SPECIAL_CLASS_SIZE 32
#define CUSTOM_CLASS Qnil

namespace rubinius {

  /* State is a VM* so, we can just use this in here */
  #define state this

  Class *VM::new_basic_class(OBJECT sup, size_t fields) {
    Class *cls = (Class*)om->new_object(globals.klass, Class::fields);
    cls->instance_fields = Object::i2n(fields);
    cls->has_ivars = Qtrue;
    cls->object_type = Object::i2n(ObjectType);
    SET(cls, superclass, sup);

    return cls;
  }

  Class *VM::new_class(OBJECT sup, size_t fields) {
    Class *cls = new_basic_class(sup, fields);
    MetaClass::attach(this, cls);
    return cls;
  }

  Class* VM::new_class(char* name) {
    return new_class(name, globals.object, globals.object->instance_fields->n2i());
  }

  Class* VM::new_class(char* name, size_t fields) {
    return new_class(name, globals.object, fields);
  }

  Class* VM::new_class(char* name, OBJECT sup, size_t fields) {
    return new_class(name, sup, fields, globals.object);
  }

  Class* VM::new_class(char* name, OBJECT sup, size_t fields, Module* under) {
    Class* cls = new_class(sup, fields);
    cls->setup(this, name, under);
    return cls;
  }

  Module* VM::new_module(char* name, Module* under) {
    Module *mod = (Module*)om->new_object(globals.module, Module::fields);
    mod->setup(this, name, under);
    return mod;
  }

  /* Creates the rubinius object universe from scratch. */
  void VM::bootstrap_ontology() {
    /* Class is created first by hand, and twittle to setup the internal
       recursion. */

    Class *cls = (Class*)om->new_object(Qnil, Class::fields);
    cls->klass = cls;

    cls->instance_fields = Object::i2n(Class::fields);
    cls->has_ivars = Qtrue;
    cls->object_type = Object::i2n(ClassType);
    cls->obj_type = ClassType;

    globals.klass = cls;

    Class *object = new_basic_class(Qnil, NormalObject::fields);
    globals.object = object;

    globals.module = new_basic_class(object, Module::fields);
    cls->superclass = globals.module;

    globals.metaclass = new_basic_class(cls, MetaClass::fields);
    globals.metaclass->object_type = Object::i2n(MetaclassType);

    globals.tuple = new_basic_class(object, Tuple::fields);
    globals.tuple->object_type = Object::i2n(TupleType);
    globals.tuple->has_ivars = Qfalse;

    globals.lookuptable = new_basic_class(object, LookupTable::fields);
    globals.lookuptable->object_type = Object::i2n(LookupTableType);

    globals.methtbl = new_basic_class(globals.lookuptable, MethodTable::fields);
    globals.methtbl->object_type = Object::i2n(MTType);

    OBJECT mc = MetaClass::attach(this, object, cls);
    mc = MetaClass::attach(this, globals.module, mc);
    MetaClass::attach(this, cls, mc);

    MetaClass::attach(this, globals.metaclass);
    MetaClass::attach(this, globals.tuple);
    MetaClass::attach(this, globals.lookuptable);
    MetaClass::attach(this, globals.methtbl);

    globals.symbol    = new_class(object, 0);
    globals.array     = new_class(object, Array::fields);
    globals.array->object_type = Object::i2n(ArrayType);
    globals.array->has_ivars = Qfalse;

    globals.bytearray = new_class(object, 0);
    globals.bytearray->object_type = Object::i2n(ByteArrayType);

    globals.string    = new_class(object, String::fields);
    globals.string->object_type = Object::i2n(StringType);
    globals.string->has_ivars = Qfalse;

    globals.symtbl    = new_class(object, SymbolTable::fields);

    globals.executable = new_class(object, Executable::fields);

    globals.cmethod   = new_class(globals.executable, CompiledMethod::fields);
    globals.cmethod->object_type = Object::i2n(CMethodType);

    globals.hash      = new_class(object, Hash::fields);
    globals.hash->object_type = Object::i2n(HashType);

    globals.io         = new_class(object, IO::fields);

    globals.blokenv    = new_class(object, BlockEnvironment::fields);
    globals.blokenv->object_type = Object::i2n(BlockEnvType);

    globals.staticscope = new_class(object, StaticScope::fields);

    bootstrap_symbol();

    globals.object->setup(this, "Object");
    globals.klass->setup(this, "Class");
    globals.module->setup(this, "Module");
    globals.metaclass->setup(this, "MetaClass");
    globals.symbol->setup(this, "Symbol");
    globals.tuple->setup(this, "Tuple");
    globals.array->setup(this, "Array");
    globals.bytearray->setup(this, "ByteArray");
    globals.hash->setup(this, "Hash");
    globals.lookuptable->setup(this, "LookupTable");
    globals.string->setup(this, "String");
    globals.symtbl->setup(this, "SymbolTable");
    globals.methtbl->setup(this, "MethodTable");
    globals.executable->setup(this, "Executable");
    globals.cmethod->setup(this, "CompiledMethod");
    globals.io->setup(this, "IO");
    globals.blokenv->setup(this, "BlockEnvironment");
    globals.staticscope->setup(this, "StaticScope");
    globals.symbol->setup(this, "Symbol");

    globals.object->set_const(this, symbol("Symbols"), globals.symbols);

    globals.nil_class = new_class("NilClass", object, 0);
    globals.true_class = new_class("TrueClass", object, 0);
    globals.false_class = new_class("FalseClass", object, 0);

    Class* numeric = new_class("Numeric", object, 0);
    Class* integer = new_class("Integer", numeric, 0);
    globals.fixnum_class = new_class("Fixnum", integer, 0);
    globals.fixnum_class->object_type = Object::i2n(FixnumType);

    globals.bignum = new_class("Bignum", integer, 0);
    globals.bignum->object_type = Object::i2n(BignumType);
    Bignum::init(this);

    globals.floatpoint = new_class("Float", numeric, 0);
    globals.floatpoint->object_type = Object::i2n(FloatType);

    globals.methctx = new_class("MethodContext", object, 0);
    globals.blokctx = new_class("BlockContext", globals.methctx, 0);

    globals.task = new_class("Task", object, 0);
    globals.task->object_type = Object::i2n(TaskType);

    globals.iseq = new_class("InstructionSequence", globals.bytearray, 0);
    globals.iseq->object_type = Object::i2n(ISeqType);

    for(size_t i = 0; i < SPECIAL_CLASS_SIZE; i += 4) {
      globals.special_classes[i + 0] = Qnil;
      globals.special_classes[i + 1] = globals.fixnum_class;
      globals.special_classes[i + 2] = Qnil;
      if(((i + 3) & 0x7) == 0x3) {
        globals.special_classes[i + 3] = globals.symbol;
      } else {
        globals.special_classes[i + 3] = CUSTOM_CLASS;
      }
    }

    globals.special_classes[(uintptr_t)Qundef] = Qnil;
    globals.special_classes[(uintptr_t)Qfalse] = globals.false_class;
    globals.special_classes[(uintptr_t)Qnil  ] = globals.nil_class;
    globals.special_classes[(uintptr_t)Qtrue ] = globals.true_class;

    Regexp::init(this);

    new_module("Rubinius");

    bootstrap_exceptions();

    globals.external_ivars = LookupTable::create(state);

    List::init(state);
    SendSite::init(state);
    Selector::init(state);
    init_ffi();
  }

  void VM::bootstrap_symbol() {
    globals.symbols = SymbolTable::create(this);

    globals.sym_object_id = symbol("object_id");
  }

  void VM::bootstrap_exceptions() {
    int sz;
    sz = 3;

    OBJECT exc, scp, std, arg, nam, loe, rex, stk, sxp, sce, type, lje, vm;
    OBJECT fce;

#define dexc(name, sup) new_class(#name, sup, sz)

    exc = dexc(Exception, globals.object);
    globals.exception = exc;
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

    globals.exc_type = type;
    globals.exc_arg = arg;
    globals.exc_loe = loe;
    globals.exc_rex = rex;

    globals.exc_stack_explosion = sxp;
    globals.exc_primitive_failure = dexc(PrimitiveFailure, exc);

    globals.exc_segfault = dexc(MemorySegmentionError, exc);

    Module* ern = new_module("Errno");

    globals.errno_mapping = LookupTable::create(state);

    ern->set_const(state, symbol("Mapping"), globals.errno_mapping);

    sz = 4;

#define set_syserr(num, name) ({ \
    Class* _cls = new_class(name, sce, sz, ern); \
    _cls->set_const(state, symbol("Errno"), Object::i2n(num)); \
    globals.errno_mapping->store(state, Object::i2n(num), _cls); \
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
