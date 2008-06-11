# depends on: class.rb

##
# DelegatedMethod is used to implement Module#define_method

class DelegatedMethod < RuntimePrimitive

  Receiver = 5
  UseSelf = 6

  self.instance_fields = 7

  def name= n; @name = n end
  def name; @name end
  def receiver
    at(Receiver)
  end

  def self.build(sym, receiver, pass_self)
    obj = allocate()
    obj.name = "<delegated to: #{receiver.to_s}>"
    obj.put RuntimePrimitive::PrimitiveIndex, :dispatch_as_method
    obj.put RuntimePrimitive::RequiredArguments, -1
    obj.put RuntimePrimitive::SerialNumber, 0
    obj.put RuntimePrimitive::ByteCodes, sym
    obj.put Receiver, receiver
    obj.put UseSelf, pass_self # If true, first argument will be 'self'
    return obj
  end

  def activate(recv, mod, args)
    sz = args.total

    out = Rubinius.asm(recv, mod, args, sz) do |r,m,a,s|
      run a
      push_array
      push :nil
      push :nil
      run s
      run m
      push :self
      run r
      activate_method 0
    end

    return out
  end
end
