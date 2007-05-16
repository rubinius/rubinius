class NativeMethod
  def lines
    nil
  end
  
  def exceptions
    nil
  end
  
  def literals
    nil
  end
  
  def line_from_ip(i)
    0
  end
end

class CompiledMethod
  def __ivars__; Ruby.asm "push self\npush 0\nfetch_field"; end
  def primitive; Ruby.asm "push self\npush 1\nfetch_field"; end
  def required; Ruby.asm "push self\npush 2\nfetch_field"; end
  def serial; Ruby.asm "push self\npush 3\nfetch_field"; end
  def bytecodes; Ruby.asm "push self\npush 4\nfetch_field"; end
  def name; Ruby.asm "push self\npush 5\nfetch_field"; end
  def file; Ruby.asm "push self\npush 6\nfetch_field"; end
  def locals; Ruby.asm "push self\npush 7\nfetch_field"; end
  def literals; Ruby.asm "push self\npush 8\nfetch_field"; end
  def arguments; Ruby.asm "push self\npush 9\nfetch_field"; end
  def scope; Ruby.asm "push self\npush 10\nfetch_field"; end
  def exceptions; Ruby.asm "push self\npush 11\nfetch_field"; end
  def lines; Ruby.asm "push self\npush 12\nfetch_field"; end
  def path; Ruby.asm "push self\npush 13\nfetch_field"; end
  def cache; Ruby.asm "push self\npush 14\nfetch_field"; end
  ivar_as_index :__ivars__ => 0, :primitive => 1, :required => 2, :serial => 3, :bytecodes => 4, :name => 5, :file => 6, :locals => 7, :literals => 8, :arguments => 9, :scope => 10, :exceptions => 11, :lines => 12, :path => 13, :cache => 14
  
  def inspect
    "#<#{self.class.name}:0x#{self.object_id.to_s(16)} name=#{@name} file=#{@file}>"
  end
  
  def from_string(bc, lcls, req)
    @bytecodes = bc
    @primitive = -1
    @locals = lcls
    @literals = Tuple.new(0)
    @required = 0
    @arguments = Tuple.new(0)
    @exceptions = nil
    @lines = nil
    @file = nil
    @name = nil
    @path = nil
    @required = req
    return self
  end
  
  def exceptions=(tup)
    @exceptions = tup
  end
  
  def literals=(tup)
    @literals = tup
  end
  
  def file=(val)
    @file = val
  end
  
  def name=(val)
    @name = val
  end
  
  def lines=(val)
    @lines = val
  end
  
  def path=(val)
    @path = val
  end
  
  def primitive=(idx)
    @primitive = idx
  end
  
  def cache=(tup)
    @cache = tup
  end
  
  def serial=(ser)
    @serial = ser
  end
  
  def activate(recv, args, locals=nil, &prc)
    sz = args.total
    if prc
      block = prc.block
    else
      block = nil
    end
    
    Ruby.asm <<-ASM
    push args
    push_array
    push block
    push locals
    push sz
    push self
    push recv
    activate_method
    ASM
  end
  
  def self.load_from_file(path)
    Ruby.primitive :load_file
  end
  
  def activate_as_script
    Ruby.primitive :activate_as_script
  end
  
  def line_from_ip(i)
    @lines.each do |t|
      start = t.at(0)
      nd = t.at(1)
      op = t.at(2)
      if i >= start and i <= nd
        return op
      end
    end
    return 0
  end

  def arity
    @required
  end
  
  def bytecodes
    @bytecodes
  end

  def bytecodes=(other)
    @bytecodes = other
  end
  
  def first_line
    @lines.each do |ent|
      return ent[2] if ent[2] > 0
    end
    
    return -1
  end
end

class Method
  def initialize(recv, mod, cm)
    @receiver = recv
    @method = cm
    @module = mod
  end
  
  def inspect
    "#<#{self.class} #{@receiver.class}(#{@module})##{@method.name}>"
  end
  
  def call(*args, &prc)
    @method.activate(@receiver, args, &prc)
  end

  def unbind
    UnboundMethod.new(@module, @method)
  end

  def arity
    @method.arity
  end
  
  def location
    "#{@method.file}, near line #{@method.first_line}"
  end
end

class UnboundMethod
  def initialize(mod, cm)
    @method = cm
    @module = mod
  end
  
  def inspect
    "#<#{self.class} #{@module}##{@method.name}>"
  end

  def bind(receiver)
    Method.new(receiver, @module, @method)
  end

  def arity
    @method.arity
  end
end

class RuntimePrimitive
  SetIvar = 1024
  GetIvar = 1025
  SetIndex = 1026
  GetIndex = 1027
end

class AccessVarMethod < RuntimePrimitive
  self.instance_fields = 5
  
  def self.get_ivar(name)
    obj = allocate()
    obj.put 1, RuntimePrimitive::GetIvar
    obj.put 2, 0
    obj.put 3, 0 # serial number
    obj.put 4, name
    return obj
  end
  
  def self.set_ivar(name)
    obj = allocate()
    obj.put 1, RuntimePrimitive::SetIvar
    obj.put 2, 1
    obj.put 3, 0 # serial number
    obj.put 4, name
    return obj
  end
  
  def self.get_index(idx)
    obj = allocate()
    obj.put 1, RuntimePrimitive::GetIndex
    obj.put 2, 0
    obj.put 3, 0 # serial number
    obj.put 4, idx
    return obj
  end
  
  def self.set_index(idx)
    obj = allocate()
    obj.put 1, RuntimePrimitive::SetIndex
    obj.put 2, 1
    obj.put 3, 0 # serial number
    obj.put 4, idx
    return obj
  end
  
end
