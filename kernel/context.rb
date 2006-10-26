
# Hey! Be careful with this! This is used by backtrace and if it doesn't work,
# you can get recursive exceptions being raise (THATS BAD, BTW).
class MethodContext
  def self.current
    cur = nil
    Ruby.asm "push_context\nset cur\n"
    return cur.sender
  end
    
  def to_s
    "#<#{self.class}:0x#{self.object_id.to_s(16)} #{receiver}##{name} #{file}:#{line}>"
  end
  
  def file
    method.file
  end
  
  def lines
    method.lines
  end
  
  def line
    i = self.ip
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

class BlockContext
  def call(*args)
    if args.total == 1
      block_call args[0]
    else
      block_call args.tuple
    end
  end
  def block_call(args)
    Ruby.primitive :block_call
  end
end

class Proc
  
  Ruby.asm "push self\npush 2\npush 5\nstore_field"
  
  def initialize(blk)
    put 1, blk
  end
  
  def self.block_passed(blk)
    if blk === Proc
      blk = blk.block
    elsif !blk.kind_of?(BlockContext)
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
  
  def block
    Ruby.asm "push 1\npush self\nfetch_field"
  end
  
  def call(*args)
    obj = block.dup
    obj.call(*args)
  end
end

class Backtrace
  def initialize
    put 0, {}
    @frames = []
  end
  
  def show
    #strs = @frames.map do |ary|
    #  "#{ary[0]}##{ary[1]} at #{ary[2]}:#{ary[3]}"
    #end
    strs = @frames.join("\n")
    return strs
    strs.join("\n")
  end
  
  def fill_from(ctx)
    while ctx
      str = "#{ctx.receiver.to_s}##{ctx.name} at #{ctx.file}:#{ctx.line}"
      # str = "#{ctx.receiver.to_s}##{ctx.name}"
      @frames << str
      ctx = ctx.sender
    end
  end
  
  def self.backtrace
    obj = new()
    ctx = MethodContext.current.sender
    obj.fill_from ctx
    return obj
  end
end