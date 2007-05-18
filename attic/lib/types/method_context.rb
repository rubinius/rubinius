module Rubinius
  module MethodContext
    Fields = [:sender, :ip, :sp, :block, :raiseable, :method, :bytecodes,
              :literals, :receiver, :locals, :argcount,
              :name, :module]
    
    def self.new_anonymous
      obj = create_anonymous
      obj.init_registers
      return obj
    end
    
    def init_registers
      self.argcount = RObject.wrap(0)
      self.ip = RObject.wrap(0)
      self.sp = RObject.wrap(0)
    end
    
    def self.new(cnt)
      obj = allocate
      obj.init_registers
      obj.set_locals(cnt)
      return obj
    end
    
    def set_locals(cnt)
      self.locals = Rubinius::Tuple.new(cnt + 2)
    end
    
    def self.from_method(meth, from)
      mc = new(meth.locals.to_int)
      mc.argcount = RObject.wrap(0)
      mc.raiseable = RObject.true
      mc.bytecodes = meth.bytecodes
      mc.sender = from
      mc.method = meth
      mc.literals = meth.literals
      return mc
    end
    
    def self.reuse(cur, meth, from)
      lcls = meth.locals.to_int
      if cur.locals.fields < lcls
        # puts "Enlarging locals to #{lcls}..."
        cur.set_locals(lcls)
      end
      
      cur.init_registers
      cur.raiseable = RObject.true
      cur.bytecodes = meth.bytecodes
      cur.sender = from
      cur.method = meth
      cur.literals = meth.literals
      
      return cur
    end
    
    def home
      self
    end
    
    WasReferenced = 0x20
    
    def self.was_referenced?(obj)
      obj.flag_set? WasReferenced
    end
    
    def referenced!
      self.flag_set WasReferenced
    end
    
  end
  
  add_type :methctx, MethodContext
    
  module BlockContext
    # The first N arguments need to be the same as the fields of 
    # MethodContext so that a BlockContext can be 'cast' as a 
    # MethodContext.
    Fields = [:sender, :ip, :sp, :block, :raiseable, :env]
    
    IsBlockContextFlag = 0x40
        
    def self.under_environment(env)
      obj = allocate
      obj.flag_set IsBlockContextFlag
      obj.raiseable = RObject.true
      
      env.as :blokenv
      obj.ip = env.initial_ip
      obj.sp = RObject.wrap(0)
      obj.env = env
      
      return obj
    end
    
    def self.block_context?(obj)
      obj.flag_set? IsBlockContextFlag
    end
    
    def home
      env = self.env
      env.as :blokenv
      return env.home
    end
  end
  
  add_type :blokctx, BlockContext
  
  module BlockEnvironment
    Fields = [:home, :initial_ip, :last_ip, :post_send]
    
    def self.under_context(ctx, lst, vlst)
      obj = allocate
      obj.home = ctx
      
      # The 5 is here because we always to a basic jump right after
      # a block is created. By setting the block context's ip to
      # + 5, we bypass the jump and go to the bytecode for the block
      # itself.
      obj.initial_ip = RObject.wrap(ctx.ip.to_int + 5)
      obj.last_ip = lst
      obj.post_send = vlst
      
      return obj
    end
    
    def create_context
      BlockContext.under_environment(self)
    end  
  end
  
  add_type :blokenv, BlockEnvironment
end