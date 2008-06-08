##
# Contains stack frame objects

class Backtrace
  include Enumerable

  attr_accessor :frames
  attr_accessor :top_context
  attr_accessor :first_color
  attr_accessor :kernel_color
  attr_accessor :eval_color

  def initialize
    @frames = []
    @top_context = nil
    @first_color = "\033[0;31m"
    @kernel_color = "\033[0;34m"
    @eval_color = "\033[0;33m"
  end

  def [](index)
    @frames[index]
  end

  def show(sep="\n", colorize = true)
    first = true
    color_config = Rubinius::RUBY_CONFIG["rbx.colorize_backtraces"]
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

  MAX_WIDTH = 40

  def fill_from(ctx)
    @max = 0
    while ctx
      unless ctx.method
        ctx = ctx.sender
        next
      end

      str = ctx.describe

      if str.size > @max
        @max = str.size
      end

      @frames << [str, ctx.location]
      ctx = ctx.sender
    end
    @max = MAX_WIDTH if @max > MAX_WIDTH
  end

  def self.backtrace(ctx=nil)
    ctx ||= MethodContext.current.sender
    obj = new()
    # If we are here with a Proc binding, start elsewhere where
    # the proc was registered, not at our sender
    if ctx.__kind_of__(BlockContext) and env = ctx.env.caller_env
      obj.frames = [["#{env.method.name}", "#{env.position_info}"]]
      ctx = ctx.env.caller_env.next_frame
      obj.top_context = env
    else
      obj.top_context = ctx
    end
    obj.fill_from ctx
    return obj
  end

  def each
    @frames.each { |f| yield f.last }
    self
  end

  def to_mri
    return @top_context.calling_hierarchy(0)
  end
end
