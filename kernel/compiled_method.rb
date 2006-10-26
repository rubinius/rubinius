class CompiledMethod
  def activate(recv, args)
    sz = args.total
    Ruby.asm <<-ASM
    push args
    push_array
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
    self.lines.each do |t|
      start = t.at(0)
      nd = t.at(1)
      op = t.at(2)
      if i >= start and i < nd
        return op
      end
    end
    return 0
  end
end

class Method
  def initialize(recv, mod, cm)
    put 0, {}
    @receiver = recv
    @method = cm
    @module = mod
  end
  
  def inspect
    "#<#{self.class} #{@receiver.class}(#{@module})##{@method.name}>"
  end
  
  def call(*args)
    @method.activate(@receiver, args)
  end
end

class RuntimePrimitive
  SetIvar = 1024
  GetIvar = 1025
  SetIndex = 1026
  GetIndex = 1027
end

class AccessVarMethod < RuntimePrimitive
  self.instance_fields = 4
  
  def self.get_ivar(name)
    obj = allocate()
    obj.put 1, RuntimePrimitive::GetIvar
    obj.put 2, 0
    obj.put 3, name
    return obj
  end
  
  def self.set_ivar(name)
    obj = allocate()
    obj.put 1, RuntimePrimitive::SetIvar
    obj.put 2, 1
    obj.put 3, name
    return obj
  end
  
  def self.get_index(idx)
    obj = allocate()
    obj.put 1, RuntimePrimitive::GetIndex
    obj.put 2, 0
    obj.put 3, idx
    return obj
  end
  
  def self.set_index(idx)
    obj = allocate()
    obj.put 1, RuntimePrimitive::SetIndex
    obj.put 2, 1
    obj.put 3, idx
    return obj
  end
  
end