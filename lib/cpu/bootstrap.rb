class CPU
  def bootstrap
    cls = Rubinius::Class.create_anonymous
    cls.rclass = cls
    cls.instance_fields = RObject.wrap(Rubinius::Class::NumberOfFields)
    Global.class = cls
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
    
    Global.symbols = Rubinius::SymbolTable.new
    
    Global.object.setup "Object"
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
    
    sym = Rubinius::String.new("Symbols").to_sym
    Global.object.const_set sym, Global.symbols
    
    Rubinius::Class.create_normal "NilClass", Global.object, 0
    Rubinius::Class.create_normal "TrueClass", Global.object, 0
    Rubinius::Class.create_normal "FalseClass", Global.object, 0
    Rubinius::Class.create_normal "Fixnum", Global.object, 0
    Rubinius::Class.create_normal "UndefClass", Global.object, 0
    
    bootstrap_contexts
    initialize_context
  end
  
  def bootstrap_contexts
    Global.methctx = Rubinius::MethodContext.class_obj Global.object
  end
end