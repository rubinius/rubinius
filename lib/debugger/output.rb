class Debugger
  # Class used to return formatted output to the +Debugger+ for display.
  # Command implementations should not output anything directly, but should
  # instead return either a string, or an instance of this class if the output
  # needs to be formatted in some way.
  class Output
    # Class for defining columns of output.
    class Columns
      # Defines a new Columns block for an output.
      # Takes the following arguments:
      # - formats: Either a fixnum specifying the number of columns, or an array
      #   of format specifications (one per column). If an array is passed, the
      #   format specifications are as handled by String#%.
      # - An optional column separator string that will be inserted between
      #   columns when converting a row of cells to a string; defaults to a
      #   single space
      # - A flag indicating whether to calculate the required widths for each
      #   column, or use the fixed widths specified in the formats arg.
      def initialize(formats, col_sep=' ', auto=true)
        if formats.kind_of? Array
          @formats = formats
        elsif formats.kind_of? Fixnum
          @formats = Array.new(formats, '%-s')
        else
          raise ArgumentError, "The formats arg must be an Array or a Fixnum (got #{formats.class})"
        end
        @widths = Array.new(@formats.size, 0)
        @col_separator = col_sep
        @auto = auto

        unless @auto
          re = /%([|-])?(\d*)([sd])/
          @formats.each_with_index do |fmt, i|
            fmt =~ re
            if $2.length > 0
              @widths[i] = $2.to_i + $`.length + $'.length
            end
          end
        end
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
      # TODO: Handle line_width arg to limit line overall length
      def format_str(row, line_width=nil)
        cells = []
        cum_width = 0
        re = /%([|-])?(\d*)([sd])/
        @formats.each_with_index do |fmt, i|
          if row[i]
            # Format cell ignoring width and alignment, wrapping if necessary
            fmt =~ re
            cell = "#{$`}%#{$3}#{$'}" % row[i]
            align = case $1
            when '-' then :left
            when '|' then :center
            else :right
            end
            lines = wrap(cell, @widths[i], align)
            cells << lines
          else
            cells << []
          end
        end

        line, last_line = 0, 1
        str = []
        while line < last_line do
          line_cells = []
          cells.each_with_index do |cell, i|
            last_line = cell.size if line == 0 and cell.size > last_line
            if line < cell.size
              line_cells << cell[line]
            else
              # Cell does not wrap onto current line, so just output spaces
              line_cells << ' ' * @widths[i]
            end
          end
          str << line_cells.join(@col_separator)
          line += 1
        end
        str
      end

      # Splits the supplied string at logical breaks to ensure that no line is
      # longer than the spcecified width. Returns an array of lines.
      def wrap(str, width, align=:none)
        raise ArgumentError, "Invalid wrap length specified (#{width})" if width < 1

        return [nil] unless str

        str.strip!
        lines = []
        until str.length <= width do
          if pos = str[0, width].rindex(/[\s\-,]/)
            # Found a break on whitespace or dash
            line, str = str[0..pos].strip, str[pos+1..-1].strip
          elsif pos = str[0, width-1].rindex(/[^\w]/)
            # Found a non-word character to break on
            line, str = str[0...pos].strip, str[pos..-1].strip
          else
            # Force break at width
            line, str = str[0...width].strip, str[width..-1].strip
          end

          # Pad with spaces to requested width if an alignment is specified
          lines << align(line, width, align)
        end
        lines << align(str, width, align) if str
        lines
      end

      # Aligns 
      def align(line, width, align)
        case align
        when :left
          line = line + ' ' * (width - line.length)
        when :right
          line = ' ' * (width - line.length) + line
        when :center
          line = line.center(width, ' ')
        else
          line
        end
      end
    end


    ##
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
      self << Columns.new(formats, col_sep, auto)
    end

    # Convenience method to set a new row color
    def set_color(color)
      self << Color.new(color) unless @current_color and @current_color.color == color
    end

    def set_line_marker(marker='=> ')
      self << LineMarker.new(marker)
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
          str << column.format_str(item).join("\n  ").rstrip
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