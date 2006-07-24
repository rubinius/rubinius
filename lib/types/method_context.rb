module Rubinius
  module MethodContext
    Fields = [:sender, :ip, :sp, :ms, :method, :reciever, :locals, :block]
    
    def self.new_anonymous
      obj = create_anonymous
      obj.init_registers
      return obj
    end
    
    def init_registers
      self.ip = RObject.wrap(0)
      self.sp = RObject.wrap(0)
      self.ms = RObject.wrap(0)
    end
    
    def self.new(cnt)
      obj = allocate
      obj.init_registers
      cnt += 2
      obj.locals = Rubinius::Tuple.new(cnt)
      return obj
    end
  end
  
  add_type :methctx, MethodContext
end