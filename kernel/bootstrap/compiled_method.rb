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
  ivar_as_index :__ivars__ => 0, :primitive => 1, :required => 2, :serial => 3, :bytecodes => 4, :name => 5, :file => 6, :locals => 7, :literals => 8, :arguments => 9, :scope => 10, :exceptions => 11, :lines => 12, :path => 13, :cache => 14
  def __ivars__ ; @__ivars__  ; end
  def primitive ; @primitive  ; end
  def required  ; @required   ; end
  def serial    ; @serial     ; end
  def bytecodes ; @bytecodes  ; end
  def name      ; @name       ; end
  def file      ; @file       ; end
  def locals    ; @locals     ; end
  def literals  ; @literals   ; end
  def arguments ; @arguments  ; end
  def scope     ; @scope      ; end
  def exceptions; @exceptions ; end
  def lines     ; @lines      ; end
  def path      ; @path       ; end
  def cache     ; @cache      ; end
  
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
  
  def compile
    @bytecodes.compile
  end
  
  def activate(recv, args, locals=nil, &prc)
    sz = args.total
    if prc
      block = prc.block
    else
      block = nil
    end
    
    Ruby.asm <<-ASM
#local args
push_array
#local block
#local locals
#local sz
push self
#local recv
activate_method
    ASM
  end
  
  def self.load_from_file(path, version)
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
  
  def first_ip_on_line(line)
    @lines.each do |t|
      if t.at(2) >= line
        return t.at(0)
      end
    end
    
    return -1
  end

  def arity
    @required
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
  
  def compiled_method
    @method
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

