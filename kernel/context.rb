
# Hey! Be careful with this! This is used by backtrace and if it doesn't work,
# you can get recursive exceptions being raise (THATS BAD, BTW).
class MethodContext
  def self.current
    cur = nil
    Ruby.asm "push_context\nset cur\n"
    return cur.sender
  end
  
  def sender
    Ruby.primitive :context_sender
    # We use a primitive here so that the underside is aware that
    # we're pulling a methodcontext into the normal fraw. Currently
    # this means that it can't be reused automatically.
  end
  
  def to_s
    "#<#{self.class}:0x#{self.object_id.to_s(16)} #{receiver}##{name} #{file}:#{line}>"
  end
  
  def file
    return "(unknown)" unless self.method
    method.file
  end
  
  def lines
    return [] unless self.method
    method.lines
  end
  
  def line
    return 0 unless self.method
    return self.method.line_from_ip(self.ip)
  end
  
  def activate(val)
    Ruby.primitive :activate_context    
  end
  
  def copy(locals=false)
    d = self.dup
    return d unless locals
    
    i = 0
    lc = self.locals
    tot = lc.fields
    nl = Tuple.new(tot)
    while i < tot
      nl.put i, lc.at(i)
      i += 1
    end
    
    # d.put 10, nl
    
    return d
  end
end

class BlockContext
  def activate(val)
    Ruby.primitive :activate_context    
  end  
end

class BlockEnvironment
  def call(*args)
    if args.total == 1
      execute args[0]
    else
      execute args.tuple
    end
  end
  
  def execute(args)
    Ruby.primitive :block_call
  end
  
  # These should be safe since I'm unsure how you'd have a BlockContext
  # and have a nil CompiledMethod (something that can (and has) happened
  # with MethodContexts)
  
  def file
    self.home.method.file
  end
  
  def line
    self.home.method.line_from_ip(self.initial_ip)
  end
end

class Proc
  
  self.instance_fields = 3
  
  def block
    at(1)
  end
  
  def self.from_environment(env, check_args=false)
    obj = allocate()
    obj.put 1, env
    obj.put 2, check_args
    return obj
  end
  
  def self.new(&block)
    return block
  end
  
  def inspect
    "#<#{self.class}:0x#{self.object_id.to_s(16)} @ #{self.block.file}:#{self.block.line}>"
  end
  
  def self.block_passed(blk)
    if blk === Proc
      blk = blk.block
    elsif !blk.kind_of?(BlockEnvironment)
      if blk.respond_to?(:to_proc)
        prc = blk.to_proc
        if Proc === prc
          blk = prc.block
        else
          raise ArgumentError.new("to_proc did not return a Proc")
        end
      else
        raise ArgumentError.new("Unable to turn a #{blk.class} into a Proc")
      end
    end
      
    obj = allocate()
    obj.put 1, blk
    return obj
  end
    
  def call(*args)
    obj = self.block
    obj.call(*args)
  end
end

class Backtrace
  def initialize
    put 0, {}
    @frames = []
  end
  
  def show
    strs = @frames.reverse.join("\n")
    return strs
  end
  
  def fill_from(ctx)
    while ctx
      if ctx.method
        str = "    "
        if ctx.receiver == MAIN
          str << "#{ctx.receiver.to_s}."
        elsif Module === ctx.receiver
          str << "#{ctx.receiver}."
        else
          str << "#{ctx.receiver.class}#"
        end
        if ctx.name == ctx.method.name
          str << "#{ctx.name}"
        else
          str << "#{ctx.name} (#{ctx.method.name})"
        end
        str << " at #{ctx.file}:#{ctx.line}"
        @frames << str
      end
      ctx = ctx.sender
    end
  end
  
  def self.backtrace(ctx=nil)
    obj = new()
    unless ctx
      ctx = MethodContext.current.sender
    end
    obj.fill_from ctx
    return obj
  end
end