##
# Contains all logic for gathering and displaying backtraces.

class Rubinius::Backtrace
  include Enumerable

  MAX_WIDTH = 36
  MIN_WIDTH = 20

  attr_accessor :first_color
  attr_accessor :kernel_color
  attr_accessor :eval_color

  # If passed nil, we assume someone forgot to create a backtrace
  # in the VM.
  def self.backtrace(locations)
    locations ? new(locations) : new([Rubinius::Location::Missing.new])
  end

  def initialize(locations, width = Rubinius::TERMINAL_WIDTH, min_width = MIN_WIDTH)
    color_config = Rubinius::Config["rbx.colorize_backtraces"]
    if ENV['RBX_NOCOLOR'] or color_config == "no" or color_config == "NO"
      @colorize = false
    else
      @colorize = true
    end

    @locations = locations
    @first_color = "\033[0;31m"
    @kernel_color = "\033[0;34m"
    @eval_color = "\033[0;33m"
    @inline_effect = "\033[0;4m"

    @width = width
    @min_width = min_width

    @mri_backtrace = nil
  end

  def [](index)
    @locations[index]
  end

  def show(sep="\n", show_color=true)
    first = true

    show_color = false unless @colorize
    if show_color
      clear = "\033[0m"
    else
      clear = ""
    end

    max = 0
    lines = []
    last_method = nil
    last_line = nil
    last_name = nil
    times = 0

    @locations.each do |loc|
      if loc.name == last_name and loc.method == last_method \
                               and loc.line == last_line
        times += 1
      else
        lines.last[-1] = times if lines.size > 0
        last_method = loc.method
        last_line = loc.line
        last_name = loc.name

        str = loc.describe
        max = str.size if str.size > max
        lines << [str, loc, 1]
        times = 0
      end
    end

    max = MAX_WIDTH if max > MAX_WIDTH

    str = ""
    lines.each do |recv, location, rec_times|
      pos  = location.position(Dir.getwd)
      color = show_color ? color_from_loc(pos, first) : ""
      first = false # special handling for first line

      spaces = max - recv.size
      spaces = 0 if spaces < 0

      if show_color and location.inlined?
        start = " #{' ' * spaces}#{recv} #{@inline_effect}at#{clear}#{color} "
      else
        start = " #{' ' * spaces}#{recv} at "
      end

      line_break = @width - start.size - 1
      line_break = nil if line_break < @min_width

      if line_break and pos.size >= line_break
        indent = start.size

        new_pos = ""
        bit = ""
        parts = pos.split("/")
        file = parts.pop

        first = true
        parts.each do |part|
          if bit.size + part.size > line_break
            new_pos << bit << "\n" << (' ' * indent)
            bit = ""
          end

          bit << "/" unless first
          first = false
          bit << part
        end

        new_pos << bit
        if bit.size + file.size > line_break
          new_pos << "\n" << (' ' * indent)
        end
        new_pos << "/" << file
        str << color
        str << start
        str << new_pos
        str << clear
      else
        if start.size > @width - @min_width
          str << "#{color} #{start}\\\n          #{pos}#{clear}"
        else
          str << "#{color} #{start}#{pos}#{clear}"
        end
      end

      if rec_times > 1
        str << " (#{rec_times} times)"
      end

      if location.is_jit and $DEBUG
        str << " (jit)"
      end

      str << sep
    end

    return str
  end

  def join(sep)
    show
  end

  alias_method :to_s, :show

  def color_from_loc(loc, first)
    return @first_color if first

    if loc =~ /^kernel/
      @kernel_color
    elsif loc =~ /\(eval\)/
      @eval_color
    else
      ""
    end
  end

  def each
    @locations.each { |f| yield f }
    self
  end

  def self.detect_backtrace(obj)
    if bt = obj.instance_variable_get(:@rbx_backtrace)
      return bt if bt.same_mri_backtrace?(obj)
    end

    return nil
  end

  def to_mri
    return @mri_backtrace if @mri_backtrace

    @mri_backtrace = @locations.map do |loc|
      "#{loc.position}:in `#{loc.describe_method}'"
    end

    # A little extra magic, so we can carry the backtrace along and reuse it

    @mri_backtrace.instance_variable_set(:@rbx_backtrace, self)

    return @mri_backtrace
  end

  def same_mri_backtrace?(obj)
    currently = @locations.map do |loc|
      "#{loc.position}:in `#{loc.describe_method}'"
    end

    currently == obj
  end
end
