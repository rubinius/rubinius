module Rubinius
  module MethodContext
    Fields = [:sender, :ip, :sp, :block, :ms, :method, :bytecodes,
              :receiver, :locals]
    
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
    # The first N arguments need to be the same as the fields of 
    # MethodContext so that a BlockContext can be 'cast' as a 
    # MethodContext.
    Fields = [:sender, :ip, :sp, :block, :home, :last_op, :start_op]
    
    IsBlockContextFlag = 0x40
    
    def self.under_context(ctx)
      obj = allocate
      obj.flag_set IsBlockContextFlag
      obj.ip = RObject.wrap(0)
      obj.sp = RObject.wrap(0)
      obj.home = ctx
      obj.last_op = RObject.wrap(0)
      
      # The 5 is here because we always to a basic jump right after
      # a block is created. By setting the block context's ip to
      # + 5, we bypass the jump and go to the bytecode for the block
      # itself.
      obj.start_op = RObject.wrap(ctx.ip.to_int + 5)
      return obj
    end
    
    def self.block_context?(obj)
      obj.flag_set? IsBlockContextFlag
    end
    
  end
  
  add_type :blokctx, BlockContext
end