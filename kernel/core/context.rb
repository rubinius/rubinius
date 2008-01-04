# depends on: class.rb

# Hey! Be careful with this! This is used by backtrace and if it doesn't work,
# you can get recursive exceptions being raised (THATS BAD, BTW).
class MethodContext
  
  attr_accessor :last_match

  # The Nth group of the last regexp match.
  #
  # Implemented to support Compiler2.
  def nth_ref(n)
    if lm = @last_match
      return lm[n]
    end
    
    return nil
  end

  # One of the special globals $&, $`, $' or $+.
  #
  # Implemented to support Compiler2.
  def back_ref(kind)
    if lm = @last_match
      res = case kind
      when :&
        lm[0]
      when :"`"
        lm.pre_match
      when :"'"
        lm.post_match
      when :+
        lm.captures.last
      end
      
      return res
    end
    
    return nil
  end
    
  def to_s
    "#<#{self.class}:0x#{self.object_id.to_s(16)} #{receiver}##{name} #{file}:#{line}>"
  end
  alias_method :inspect, :to_s
  
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
    # We subtract 1 because the ip is actually set to what it should do
    # next, not what it's currently doing.
    return self.method.line_from_ip(self.ip - 1)
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

  def location
    "#{file}:#{line}"
  end

  def disable_long_return!
    # 12 => fc->flags
    # CTX_FLAG_NO_LONG_RETURN => 1
    _set_field(12, 1)
  end
end

class NativeMethodContext
  def location
    "#{file}"
  end
end

class BlockContext
  
  def last_match
    home.last_match
  end
  
  def last_match=(match)
    home.last_match = match
  end
  
  def nth_ref(idx)
    home.nth_ref(idx)
  end
  
  def back_ref(idx)
    home.back_ref(idx)
  end
  
  def home
    env.home
  end
  
  def name
    home.name
  end
  
  def receiver
    home.receiver
  end
  
  def file
    home.file
  end
  
  def line
    return 0 unless method
    # We subtract 1 because the ip is actually set to what it should do
    # next, not what it's currently doing.
    return method.line_from_ip(self.ip - 1)
  end

  def method_module
    home.method_module
  end
end

class BlockEnvironment
  ivar_as_index :__ivars__ => 0, :home => 1, :initial_ip => 2, :last_ip => 3, 
    :post_send => 4, :home_block => 5, :local_count => 6, :bonus => 7, :method => 8
  def __ivars__   ; @__ivars__   ; end
  def home        ; @home        ; end
  def initial_ip  ; @initial_ip  ; end
  def last_ip     ; @last_ip     ; end
  def post_send   ; @post_send   ; end
  def home_block  ; @home_block  ; end
  def local_count ; @local_count ; end
  def bonus       ; @bonus       ; end
  def method      ; @method      ; end
      
  # Holds a Tuple of local variable names to support eval
  def bonus=(tup)
    @bonus = tup
  end

  # The CompiledMethod object that we were called from
  def method=(tup)
    @method = tup
  end

  # These should be safe since I'm unsure how you'd have a BlockContext
  # and have a nil CompiledMethod (something that can (and has) happened
  # with MethodContexts)
  def file
    home.method.file
  end
  
  def line
    home.method.line_from_ip(initial_ip)
  end
  
  def home=(home)
    @home = home
  end

  def scope=(tup)
    @scope = tup
  end
  
  def make_independent
    @home = @home.dup
  end
  
  def redirect_to(obj)
    env = dup
    env.make_independent
    env.home.receiver = obj
    return env
  end

  def call_on_instance(obj, *args)
    obj = redirect_to(obj)
    obj.call *args
  end

  def disable_long_return!
    @post_send = nil
  end
end

class Backtrace
  include Enumerable

  attr_reader :frames

  def initialize
    @frames = []
    @top_context = nil
    @first_color = "\033[0;31m"
    @kernel_color = "\033[0;34m"
    @eval_color = "\033[0;33m"
  end

  attr_accessor :first_color
  attr_accessor :kernel_color
  attr_accessor :eval_color
  
  def show(sep="\n", colorize = true)
    first = true
    color_config = RUBY_CONFIG["rbx.colorize_backtraces"]
    if color_config == "no" or color_config == "NO"
      colorize = false
      color = ""
      clear = ""
    else
      clear = "\033[0m"
    end

    fr2 = @frames.map do |ent|
      recv = ent[0]
      loc = ent[1]
      color = color_from_loc(loc, first) if colorize
      first = false # special handling for first line
      times = @max - recv.size
      times = 0 if times < 0
      "#{color}    #{' ' * times}#{recv} at #{loc}#{clear}"
    end
    return fr2.join(sep)
  end
  
  def join(sep)
    show
  end
  
  alias_method :to_s, :show

  def color_from_loc(loc, first)
    return @first_color if first
    if loc =~ /kernel/
      @kernel_color
    elsif loc =~ /\(eval\)/
      @eval_color
    else
      ""
    end
  end
  
  attr_reader :top_context
    
  MAX_WIDTH = 40
  
  def fill_from(ctx)
    @top_context = ctx
    
    @max = 0
    while ctx   
      unless ctx.method
        ctx = ctx.sender
        next
      end
      
      if MAIN == ctx.receiver
        str = "#{ctx.receiver.to_s}."
      elsif ctx.method_module.kind_of?(MetaClass)
        str = "#{ctx.receiver}."
      elsif ctx.method_module and ctx.method_module != ctx.receiver.class
        str = "#{ctx.method_module}(#{ctx.receiver.class})#"
      else
        str = "#{ctx.receiver.class}#"
      end
      
      if ctx.kind_of? BlockContext
        str << "#{ctx.name} {}"
      elsif ctx.name == ctx.method.name
        str << "#{ctx.name}"
      else
        str << "#{ctx.name} (#{ctx.method.name})"
      end
      
      if str.size > @max
        @max = str.size
      end
      
      @frames << [str, ctx.location]
      ctx = ctx.sender
    end
    @max = MAX_WIDTH if @max > MAX_WIDTH
  end

  def self.backtrace(ctx=nil)
    obj = new()
    unless ctx
      ctx = MethodContext.current.sender
    end
    obj.fill_from ctx
    return obj
  end

  def each
    @frames.each { |f| yield f.last }
    self
  end

  def reverse
    ret = self.class.new

    @frames.reverse_each do |f|
      ret.frames << f
    end

    ret
  end
end
