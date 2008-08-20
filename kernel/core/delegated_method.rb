# depends on: class.rb

##
# DelegatedMethod is used to implement Module#define_method

class DelegatedMethod < RuntimePrimitive

  Receiver = 5
  UseSelf = 6

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
end
