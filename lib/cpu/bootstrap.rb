class CPU
  def bootstrap
    cls = Rubinius::Class.create_anonymous
    cls.rclass = cls
    cls.instance_fields = RObject.wrap(Rubinius::Class::NumberOfFields)
    Global.class = cls
    # Global.blank = Rubinius::BlankObject.basic_class_obj RObject.nil
    Global.object = Rubinius::Object.basic_class_obj RObject.nil
    Global.module = Rubinius::Module.basic_class_obj Global.object

    cls.superclass = Global.module
    Global.metaclass = Rubinius::MetaClass.basic_class_obj Global.class
    
    Global.object.create_metaclass cls
    Global.module.create_metaclass Global.object.metaclass
    Global.class.create_metaclass Global.module.metaclass
    
    Global.symbol = Rubinius::Symbol.class_obj Global.object
    Global.tuple = Rubinius::Tuple.class_obj Global.object
    Global.array = Rubinius::Array.class_obj Global.object
    
    Global.bytearray = Rubinius::ByteArray.class_obj Global.object
    Global.hash = Rubinius::Hash.class_obj Global.object
    
    Global.string = Rubinius::String.class_obj Global.object
    Global.symtbl = Rubinius::SymbolTable.class_obj Global.object
    Global.methtbl = Rubinius::MethodTable.class_obj Global.hash
    Global.cmethod = Rubinius::CompiledMethod.class_obj Global.object
    
    Global.io = Rubinius::IO.class_obj Global.object
    
    Global.symbols = Rubinius::SymbolTable.new
    
    Global.object.setup "Object"
    # Global.blank.setup "BlankObject"
    Global.class.setup "Class"
    Global.module.setup "Module"
    Global.metaclass.setup "MetaClass"
    Global.symbol.setup "Symbol"
    Global.tuple.setup "Tuple"
    Global.array.setup "Array"
    Global.bytearray.setup "ByteArray"
    Global.hash.setup "Hash"
    Global.string.setup "String"
    Global.symtbl.setup "SymbolTable"
    Global.methtbl.setup "MethodTable"
    Global.cmethod.setup "CompiledMethod"
    Global.io.setup "IO"
    
    sym = Rubinius::String.new("Symbols").to_sym
    Global.object.const_set sym, Global.symbols
    
    Global.nil_class = Rubinius::Class.create_normal "NilClass", Global.object, 0
    Global.true_class = Rubinius::Class.create_normal "TrueClass", Global.object, 0
    Global.false_class = Rubinius::Class.create_normal "FalseClass", Global.object, 0
    num = Rubinius::Class.create_normal "Numeric", Global.object, 0
    int = Rubinius::Class.create_normal "Integer", num, 0
    Global.fixnum = Rubinius::Class.create_normal "Fixnum", int, 0
    Global.undef_class = Rubinius::Class.create_normal "UndefClass", Global.object, 0
    Global.bignum = Rubinius::Class.create_normal "Bignum", int, 0
    bootstrap_contexts
    initialize_context
    bootstrap_exceptions
    return nil
  end
  
  def bootstrap_contexts
    Global.methctx = Rubinius::MethodContext.class_obj Global.object
    Global.blokenv = Rubinius::BlockEnvironment.class_obj Global.object
    Global.blokctx = Rubinius::BlockContext.class_obj Global.object
    
    Global.methctx.setup 'MethodContext'
    Global.blokenv.setup 'BlockEnvironment'
    Global.blokctx.setup 'BlockContext'
  end
  
  def define_class(name, sup=Global.object, fields=0)
    Rubinius::Class.create_normal name, sup, fields
  end
  
  def bootstrap_exceptions
    exc_fields = 2
    exc = define_class "Exception", Global.object, exc_fields
    fat = define_class "fatal", exc, exc_fields
    std = define_class "StandardError", exc, exc_fields
    arg = define_class "ArgumentError", std, exc_fields
    ner = define_class "NameError", std, exc_fields
    nme = define_class "NoMethodError", ner, exc_fields
    syn = define_class "SyntaxError", exc, exc_fields
    loe = define_class "LoadError", exc, exc_fields
    run = define_class "RuntimeError", std, exc_fields
    sys = define_class "SystemCallError", std, exc_fields
    
    Global.exc_arg = arg
    Global.exc_loe = loe
  end
end