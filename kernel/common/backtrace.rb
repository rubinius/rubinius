##
# Contains all logic for gathering and displaying backtraces.

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

    formatted = @frames.map do |ctx|
      recv = ctx.describe
      loc = ctx.location
      color = color_from_loc(loc, first) if colorize
      first = false # special handling for first line
      times = @max - recv.size
      times = 0 if times < 0
      "#{color}    #{' ' * times}#{recv} at #{loc}#{clear}"
    end
    return formatted.join(sep)
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

  def fill_backtrace
    @max = 0
    @backtrace = []
    # Skip the first frame if we are raising an exception from
    # an eval's BlockContext
    if @frames.at(0).from_eval?
      frames = @frames[1, @frames.length - 1]
    else
      frames = @frames
    end

    frames.each_with_index do |ctx, i|
      str = ctx.describe
      @max = str.size if str.size > @max
      @backtrace << [str, ctx.location]
    end
    @max = MAX_WIDTH if @max > MAX_WIDTH
  end

  def self.backtrace(ctx=nil)
    ctx ||= MethodContext.current.sender
    obj = new()
    obj.top_context = ctx
    obj.frames = ctx.context_stack

    # TODO - Consider not doing this step if we know we want MRI output
    obj.fill_backtrace
    return obj
  end

  def each
    @backtrace.each { |f| yield f.last }
    self
  end

  def to_mri
    return @top_context.stack_trace_starting_at(0)
  end
end
