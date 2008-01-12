class Debugger
  # Base class for debugger commands. Each subclass *must* implement the
  # following methods:
  # - command_regexp, which returns a Regexp instance used to determine when the
  # user is executing the corresponding command.
  # - execute, which takes two parameters: the instance of the +Debugger+, and a
  # +MatchData+ object containing the results of the user input matched against
  # the command_regexp.
  # Additionally, subclasses should also implement help, which should return two
  # strings: a specification of the command syntax, and a short bit of text
  # describing what the command does.
  #
  # Debugger::Command subclasses are registered with the Debugger::Command
  # superclass via an +inherited+ hook. Standard commands are defined in this
  # file, but additional debugger commands can be defined elsewhere and
  # required in.
  # However, this should be done *before* the Debugger is instantiated.
  #
  # TODO: Add a command to load extension commands after debugger is
  # instantiated
  class Command
    @commands = []

    def self.available_commands
      @commands
    end

    def self.inherited(klass)
      @commands << klass
    end

    # By default commands are processed in alphabetic order of the first item of
    # the help string. By overriding this method, commands can order themselves
    # in relation to other commands they need to precede or follow.
    def <=>(other)
      if other.kind_of? Command
        order = 0
        if other.public_methods(false).include? '<=>'
          order = (other <=> self) * -1
        end
        if order == 0
          cmd, = help
          oth_cmd, = other.help
          order = (cmd <=> oth_cmd)
        end
        return order
      else
        return nil
      end
    end
  end


  # Class used to return formatted output to the +Debugger+ for display.
  # Command implementations should not output anything directly, but should
  # instead return either a string, or an instance of this class if the output
  # needs to be formatted in some way.
  class Output
    # Class for defining columns of output.
    class Columns
      def initialize(formats, col_sep=' ', auto=true)
        if formats.kind_of? Array
          @formats = formats
        elsif formats.kind_of? Fixnum
          @formats = Array.new(formats, '%-s')
        end
        @widths = Array.new(@formats.size, 0)
        @col_separator = col_sep
        @auto = auto
      end

      # Update the column widths required based on the row content
      def update_widths(cells)
        if @auto
          re = /%([|-])?(\d*)([sd])/
          cells.each_with_index do |cell,col|
            if cell
              @formats[col] =~ re
              str = "#{$`}%#{$2}#{$3}#{$'}" % cell
              @widths[col] = str.length if str.length > @widths[col]
            end
          end
        end
      end

      def count
        @formats.size
      end

      # Format an array of cells into a string
      # TODO: Handle wrapping
      def format_str(row)
        cells = []
        re = /%([|-])?(\d*)([sd])/
        @formats.each_with_index do |fmt, i|
          if row[i]
            fmt =~ re
            if $2.length > 0
              width = $2
            else
              width = @widths[i] - $`.length - $'.length if @auto and i < @formats.size-1
            end
            cells << "#{$`}%#{$1 if $1 == '-'}#{width}#{$3}#{$'}" % row[i]
          else
            cells << ' ' * @widths[i]
          end
        end
        cells.join(@col_separator)
      end
    end


    # Class for colorizing output lines
    class Color
      def initialize(color=:clear)
        @color = color
      end
      attr_accessor :color

      # Set the color
      def escape
        case @color
        when :blue
          "\033[0;34m"
        when :red
          "\033[0;31m"
        when :green
          "\033[0;32m"
        when :yellow
          "\033[0;33m"
        when :blue
          "\033[0;34m"
        when :magenta
          "\033[0;35m"
        when :cyan
          "\033[0;36m"
        when :white
          "\033[0;37m"
        when :clear
          "\033[0m"
        else
          @color
        end
      end

      # Clear the color
      def clear
        "\033[0m"
      end

      def to_s
        @color.to_s
      end
    end

    # Class for marking a line in some way
    class LineMarker
      def initialize(marker='=> ')
        @marker = marker
      end

      def length
        @marker.length
      end

      def to_s
        @marker
      end
    end

    def initialize
      clear
      @line_width = 80
    end

    def clear
      @output = []
      @current_cols = nil
      @current_color = nil
      @marker_width = 0
    end
    attr_reader :output, :current_cols, :current_color

    def <<(item)
      case item
      when Array
        # Line contains multiple columns of text
        unless @current_cols and item.size == @current_cols.count
          # Normally, a command will explicitly specify column formats via a
          # call to set_columns; however, if the output stream receives an array
          # of objects with a different column count to previous lines, a new
          # Columns instance is auto-created.
          @current_cols = Columns.new(item.size)
          @output << @current_cols
        end
        @current_cols.update_widths(item)
      when Columns
        @current_cols = item
      when Color
        @current_color = item
      when LineMarker
        @marker_width = item.length if @marker_width < item.length
      end
      @output << item
    end

    # Convenience method to set a new column structure
    def set_columns(formats, col_sep=' ', auto=true)
      self.<< Columns.new(formats, col_sep, auto)
    end

    # Convenience method to set a new row color
    def set_color(color)
      self.<< Color.new(color) unless @current_color and @current_color.color == color
    end

    def set_line_marker(marker='=> ')
      self.<< LineMarker.new(marker)
    end

    # Convert this output stream to a string
    def to_s
      str = ""
      column = nil
      color = nil
      marker = nil
      @output.each do |item|
        case item
        when String
          str << color.escape if color
          str << output_marker(marker)
          str << item.rstrip
          str << color.clear if color
          str << "\n"
          marker = nil
        when Array
          str << '  '
          str << color.escape if color
          str << output_marker(marker)
          str << column.format_str(item).rstrip
          str << color.clear if color
          str << "\n"
          marker = nil
        when Columns
          column = item
        when Color
          color = item
        when LineMarker
          marker = item
        end
      end
      str
    end

    def output_marker(marker)
      str = ''
      if @marker_width > 0
        if marker
          str = marker.to_s
        else
          str = ' ' * @marker_width
        end
      end
      str
    end
  end
end

# Include the standard debugger commands
require 'debugger/standard_commands'
require 'debugger/vm_commands'
