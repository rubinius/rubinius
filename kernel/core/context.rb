
# Hey! Be careful with this! This is used by backtrace and if it doesn't work,
# you can get recursive exceptions being raised (THATS BAD, BTW).
class MethodContext
    
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
end

class NativeMethodContext
  def location
    "#{file}"
  end
end

class BlockContext
  
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

  def method
    home.method
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
end

class Proc
  self.instance_fields = 3
  ivar_as_index :__ivars__ => 0, :block => 1, :check_args => 2
  def block; @block ; end

  def block=(other)
    @block = other
  end

  def check_args=(other)
    @check_args = other
  end

  class << self
    def from_environment(env, check_args=false)
      if env.nil?
        nil
      elsif env.respond_to? :to_proc
        env.to_proc
      elsif env.kind_of?(BlockEnvironment)
        obj = allocate()
        obj.block = env
        obj.check_args = check_args
        obj
      else
        raise ArgumentError.new("Unable to turn a #{env.inspect} into a Proc")
      end
    end
    
    def new(&block)
      return block
    end
    
    # Return the proc given to the currently running method or 
    # to the given MethodContext/Binding.
    #
    #   def bar(&prc)
    #      a = [prc.nil?, Proc.given.nil?]
    #      a << block_given? == !Proc.given.nil?
    #      if block_given?
    #         a << prc.object_id == Proc.given.object_id
    #         a << prc.block.object_id == Proc.given.block.object_id
    #         a << Proc.given.call(21)
    #      end
    #      a
    #   end
    #   
    #   bar()                 # => [true, true, true]
    #   bar() { |n| n * 2 }   # => [false, false, true, false, true, 42]
    #
    # An example mind trick using MethodContext.
    #
    #   def stormtrooper
    #      yield "Let me see your identification."
    #      obiwan { |reply| puts "Obi-Wan: #{reply}" }
    #   end
    #
    #   def obiwan
    #      yield "[with a small wave of his hand] You don't need to see his identification."
    #      ctx = MethodContext.current.sender
    #      Proc.given(ctx).call("We don't need to see his identification.")
    #   end
    #
    #   stormtrooper { |msg| puts "Stormtrooper: #{msg}" }
    #      
    # produces the following output:
    #
    #   Stormtrooper: Let me see your identification.
    #   Obi-Wan: [with a small wave of his hand] You don't need to see his identification.
    #   Stormtrooper: We don't need to see his identification
    #
    # Using a binging to obtain the given proc where the binding was created
    #
    #   def stormtrooper
    #      binding
    #   end
    #
    #   def obiwan(trick)
    #      yield "These aren't the droids you're looking for."
    #      trick.call("There aren't the droids we're looking for.")
    #      yield "He can go about his business."
    #      trick.call("You can go about your business.")
    #   end
    #
    #   trick = stormtrooper { |msg| puts "Stormtrooper: #{msg}" }
    #   obiwan(Proc.given(trick)) { |msg| puts "Obi-Wan: #{msg}" }
    #
    def given(ctx = nil)
      case ctx
      when nil
        ctx = MethodContext.current.sender.block
      when MethodContext
        ctx = ctx.block
        # when BlockEnvironment
        # when Binding
        # ctx = ctx.context
      end
      from_environment(ctx)
    end
  end
  
  def inspect
    "#<#{self.class}:0x#{self.object_id.to_s(16)} @ #{self.block.file}:#{self.block.line}>"
  end

  def to_proc
    self
  end

  def call(*args)
    obj = at(1)
    raise "Corrupt proc detected!" unless obj
    obj.call(*args)
  end
end

class Backtrace
  def initialize
    @frames = []
  end
  
  def show
    first = true
    color_config = RUBY_CONFIG["rbx.colorize_backtraces"]
    colorize = true
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
    return fr2.join("\n")
  end

  def color_from_loc(loc, first)
    return "\033[0;31m" if first
    if loc =~ /kernel/
      "\033[0;34m"
    elsif loc =~ /\(eval\)/
      "\033[0;33m"
    else
      ""
    end
  end
  
  MAX_WIDTH = 40
  
  def fill_from(ctx)
    @max = 0
    while ctx   
      unless ctx.method
        ctx = ctx.sender
        next
      end
      
      if MAIN == ctx.receiver
        str = "#{ctx.receiver.to_s}."
      elsif MetaClass === ctx.method_module
        str = "#{ctx.receiver}."
      elsif ctx.method_module and ctx.method_module != ctx.receiver.class
        str = "#{ctx.method_module}(#{ctx.receiver.class})#"
      else
        str = "#{ctx.receiver.class}#"
      end
      
      if ctx.name == ctx.method.name
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
end
