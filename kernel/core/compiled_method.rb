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
  ivar_as_index :__ivars__ => 0, :primitive => 1, :required => 2, :serial => 3, :bytecodes => 4, :name => 5, :file => 6, :locals => 7, :literals => 8, :arguments => 9, :scope => 10, :exceptions => 11, :lines => 12, :path => 13, :cache => 14, :bonus => 15, :compiled => 16
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
  def bonus     ; @bonus      ; end
  def compiled  ; @compiled   ; end
  
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

  def bonus=(tup)
    @bonus = tup
  end
  
  def compile
    @bytecodes.compile
  end
  
  def activate(recv, mod, args, locals=nil, &prc)
    sz = args.total
    if prc
      block = prc.block
    else
      block = nil
    end
    
    out = Ruby.asm <<-ASM
#local args
push_array
#local block
#local locals
#local sz
#local mod
push self
#local recv
activate_method
    ASM
    
    return out
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
