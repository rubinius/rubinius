module Rubinius
  module MethodContext
    Fields = [:sender, :ip, :sp, :ms, :method, :bytecodes,
              :receiver, :locals, :block]
    
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
    
    def self.from_method(meth, from)
      mc = new(meth.locals.to_int)
      mc.bytecodes = meth.bytecodes
      mc.sender = from
      mc.method = meth
      return mc
    end
    
    def home
      self
    end
  end
  
  add_type :methctx, MethodContext
  
  module BlockContext
    Fields = [:sender, :ip, :sp, :home, :last_op, :start_op]
    
    def self.under_context(ctx)
      obj = allocate
      obj.ip = RObject.wrap(0)
      obj.sp = RObject.wrap(0)
      obj.home = ctx
      obj.last_op = RObject.wrap(0)
      
      # The 2 is here because we always to a basic jump right after
      # a block is created. By setting the block context's ip to
      # + 2, we bypass the jump and go to the bytecode for the block
      # itself.
      obj.start_op = RObject.wrap(ctx.ip.to_int + 2)
      return obj
    end
  end
  
  add_type :blokctx, BlockContext
end