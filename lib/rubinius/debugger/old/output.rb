module Rubinius
  class Debugger

    ##
    # Class used to return formatted output to the +Debugger+ for display.
    # Command implementations should not output anything directly, but should
    # instead return either a string, or an instance of this class if the output
    # needs to be formatted in some way.

    class Output

      ##
      # Class for defining columns of output.

      class Columns
        FORMAT_RE = /%([|-])?(0)?(\d*|\*)([sd])/

        # Defines a new Columns block for an output.
        # Takes the following arguments:
        # - formats: Either a fixnum specifying the number of columns, or an array
        #   of format specifications (one per column). If an array is passed, the
        #   items of the array may be either a format specification as handled by
        #   String#%, or an array containing the column header and a format
        #   specification.
        # - An optional column separator string that will be inserted between
        #   columns when converting a row of cells to a string; defaults to a
        #   single space.
        # Column widths will be fixed at whatever size is specified in the format,
        # or if none is specified, to whatever the necessary width is requried to
        # output all values in that column. The special width specification * can
        # be used to indicate that all remaining space on the line should be set
        # as the column width.
        def initialize(formats, col_sep=' ')
          if formats.kind_of? Array
            @formats = []
            @headers = nil
            formats.each_with_index do |fmt,i|
              if fmt.kind_of? Array
                @headers = [] unless @headers
                @headers[i] = fmt.first if fmt.size > 1
                @formats[i] = fmt.last
              else
                @formats[i] = fmt
              end
            end
          elsif formats.kind_of? Fixnum
            @formats = Array.new(formats, '%-s')
            @headers = nil
          else
            raise ArgumentError, "The formats arg must be an Array or a Fixnum (got #{formats.class})"
          end
          @col_separator = col_sep

          @fixed_widths = Array.new(@formats.size)
          @widths = Array.new(@formats.size, 0)

          # Use widths specified in format string
          @formats.each_with_index do |fmt, i|
            fmt =~ FORMAT_RE
            raise ArgumentError, "Invalid format specification" unless $4
            if $3 and $3.length > 0
              if $3 != '*'
                @fixed_widths[i] = $3.to_i + $`.length + $'.length
                @widths[i] = @fixed_widths[i]
              else
                @fixed_widths[i] = '*'
              end
            end
          end

          # Initialise column widths to column header widths
          if @headers
            @headers.each_with_index do |hdr, i|
              @widths[i] = hdr.length if hdr and hdr.length > @widths[i]
            end
          end
        end

        attr_reader :fixed_widths, :widths

        # Update the column widths required based on the row content
        def update_widths(cells)
          cells.each_with_index do |cell,col|
            if cell
              @formats[col] =~ FORMAT_RE
              str = "#{$`}%#{$2}#{$3 unless $3 == '*'}#{$4}#{$'}" % cell
              @widths[col] = str.length if str.length > @widths[col]
            end
          end
        end

        # Redistributes the calculated widths, ensuring the overall line width is
        # no greater than the specified page width. Reductions are made in the
        # following order:
        # - first, all columns that have a width specification are reduced to that
        #   width (if they exceed it)
        # - next, any columns with a variable width specification are reduced
        #   (from largest to smallest) proportionately based on the needed reduction
        def redistribute_widths(page_width, indent=0)
          if page_width
            # Reduce page_wdith by any requirements for indentation and column separators
            page_width -= indent + (@widths.size-1) * @col_separator.length
            raise ArgumentError, "Page width is insufficient to display any content" if page_width < 1

            # Determine the fixed and variable width columns
            fixed_width = 0
            var_width = 0
            cum_width = 0
            variable_widths = []
            @widths.each_with_index do |width,i|
              cum_width += @widths[i]
              if @fixed_widths[i]
                if fixed_widths[i] == '*'
                  variable_widths << i
                  var_width += @widths[i]
                else
                  fixed_width += @fixed_widths[i]
                end
              end
            end
            return true if cum_width <= page_width

            # Need to squeeze - first up, ensure fixed width columns don't exceed
            # specified size
            @fixed_widths.each_with_index do |fw,i|
              if fw and fw != '*'
                if fw < @widths[i]
                  cum_width -= @widths[i] - fw
                  @widths[i] = fw
                end
              end
            end
            return true if cum_width <= page_width

            if variable_widths.size > 0 and (fixed_width + variable_widths.size) < page_width
              # Next, reduce variable widths proportionately to needs
              cum_adj = 0
              variable_widths.sort.each do |i|
                adj = (@widths[i].to_f / var_width * (cum_width - page_width)).ceil
                @widths[i] -= adj
                cum_adj += adj
                break if cum_width - cum_adj <= page_width
              end
              cum_width -= cum_adj
            end
            return true if cum_width <= page_width

            if cum_width > page_width
              # TODO: Full squeeze - squeeze all columns to fit
            end

            return false
          end
          true
        end

        # Returns true if the column specification has headers
        def has_headers?
          !@headers.nil?
        end

        # Returns a count of the number of columns defined
        def count
          @formats.size
        end

        # Returns a formatted string containing the column headers.
        # Takes two optional parameters:
        # - indent: specifies the number of characters to indent the line by
        #   (default is 0).
        # - page_width: specifies a page width to which the output should be made
        #   to fit. If nil (the default), output is not forced to fit any width.
        def format_header_str(indent=0)
          if @headers
            hdr = [nil]
            hdr.concat format_str(@headers, indent-1, Array.new(@formats.size, '%|s'))
            str = ' ' * (indent-1) + '+' if indent > 0
            @widths.each do |width|
              str << '-' * width + '+'
            end
            hdr << str
            hdr
          end
        end

        # Format an array of cells into a string
        def format_str(row, indent=0, formats=@formats)
          cells = []
          formats.each_with_index do |fmt, i|
            if row[i]
              # Format cell ignoring width and alignment, wrapping if necessary
              fmt =~ FORMAT_RE
              cell = "#{$`}%#{$4}#{$'}" % row[i]
              align = case $1
              when '-' then :left
              when '|' then :center
              else :right
              end
              pad = $2 || ' '
              lines = wrap(cell, @widths[i], align, pad)
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
            str << ' ' * indent + line_cells.join(@col_separator)
            line += 1
          end
          str
        end

        # Splits the supplied string at logical breaks to ensure that no line is
        # longer than the spcecified width. Returns an array of lines.
        def wrap(str, width, align=:none, pad=' ')
          raise ArgumentError, "Invalid wrap length specified (#{width})" if width < 0

          return [nil] unless str and width > 0

          str.rstrip!
          lines = []
          until str.length <= width do
            if pos = str[0, width].rindex(/[\s\-,\/_]/)
              # Found a break on whitespace or dash
              line, str = str[0..pos].rstrip, str[pos+1..-1].strip
            elsif pos = str[0, width-1].rindex(/[^\w]/) and pos > 0
              # Found a non-word character to break on
              line, str = str[0...pos].rstrip, str[pos..-1].strip
            else
              # Force break at width
              line, str = str[0...width].rstrip, str[width..-1].strip
            end

            # Pad with spaces to requested width if an alignment is specified
            lines << align(line, width, align, pad)
          end
          lines << align(str, width, align, pad) if str
          lines
        end

        # Aligns
        def align(line, width, align, pad=' ')
          case align
          when :left
            line = line + pad * (width - line.length)
          when :right
            line = pad * (width - line.length) + line
          when :center
            line = line.center(width, pad)
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

      ##
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

      # Convenience method for creating an Output of type :info
      def self.info(msg, color=nil)
        out = new(:info)
        out.set_color color if color
        out << msg
        out.set_color :clear if color
        out
      end

      # Convenience method for creating an Output of type :none
      def self.none(msg, color=nil)
        out = new(:none)
        out.set_color color if color
        out << msg
        out.set_color :clear if color
        out
      end

      # Convenience method for creating an Output of type :error
      def self.error(msg, color=nil)
        out = new(:error)
        out.set_color color if color
        out << msg
        out.set_color :clear if color
        out
      end

      # Initializes a new output instance.
      # Output may be of different types, with the default being :info.
      # The output type is not used by the Output class, but may be significant
      # to the interface that displays the output. Commonly used output types
      # include:
      # - :info, the default for informational output
      # - :none, to indicate there is no output of the requested type
      # - :error, to indicate a command error
      def initialize(output_type=:info)
        @output_type = output_type
        clear
      end

      # Clears all output settings
      def clear
        @output = []
        @current_cols = nil
        @current_color = nil
        @marker_width = 0
      end
      attr_reader :output, :current_cols, :current_color

      # Adds a new row of data to the output
      # The item to be added may be an object of several different types:
      # - An array is regarded as a series of cells that make up a row in a table.
      # - A string is regarded as a complete row of data.
      # - A Columns object defines the columnar aspects of subsequent rows of
      #   data, which are typically arrays.
      # - A Color object defines a color change that remains in force until it is
      #   cleared.
      # - LineMarker object specifies a marker to appear to the left of the next
      #   row.
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
      def set_columns(formats, col_sep=' ')
        self << Columns.new(formats, col_sep)
      end

      # Convenience method to set a new row color
      def set_color(color)
        self << Color.new(color) unless @current_color and @current_color.color == color
      end

      # Sets a marker to be displayed next to the next line
      def set_line_marker(marker='=> ')
        self << LineMarker.new(marker)
      end

      # Convert this output stream to a string
      def to_s
        lines.join("\n")
      end

      # Return the output as an array of strings, one per line. If an output item
      # wraps to more than one line, each line will be a seperate entry in the
      # returned array.
      # Takes an optional parameter +page_width+ that specifies the width of the
      # page on which the output will be displayed.
      def lines(page_width=nil)
        column = nil
        color = nil
        marker = nil
        lines = []
        @output.each do |item|
          case item
          when String
            str = ""
            str << color.escape if color
            str << item.rstrip
            str << color.clear if color
            lines << str
          when Columns
            column = item
            if page_width
              # Check page_width is sufficient for marker and column headers
              width = page_width-1 - (@marker_width + 2)
              column.redistribute_widths(page_width-1, @marker_width + 2) if width > 0
            end
            if column.has_headers?
              lines.concat column.format_header_str(@marker_width+2)
            end
          when Array
            str = ""
            str << color.escape if color
            str << output_marker(marker)
            marker = nil
            l = column.format_str(item, @marker_width+2)
            l.first[0, @marker_width] = ''
            l.first.insert 0, str
            l.last << color.clear if color
            lines.concat l
          when Color
            if item.color == :clear
              color = nil
            else
              color = item
            end
          when LineMarker
            marker = item
          end
        end
        lines
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
end

