module Rubinius
  class Sprintf

    attr_accessor :fmt
    attr_accessor :args
    attr_accessor :flags

    RADIXES = {"b" => 2, "o" => 8, "d" => 10, "x" => 16}
    ALTERNATIVES = {"o" => "0", "b" => "0b", "B" => "0B", "x" => "0x", "X" => "0X"}
    PrecisionMax = 1048576 # Totally random value

    def initialize(fmt, *args)
      @tainted = fmt.tainted?
      @fmt, @args, @arg_position = fmt.to_str, args, 0
    end

    def parse
      start = 0
      ret = ""
      width = nil
      precision = nil
      @positional = false
      @relative = false
      @arg_position = 0

      while (match = /%/.match_from(fmt, start))

        @flags = {:space => nil, :position => nil, :alternative => nil, :plus => nil,
          :minus => nil, :zero => nil, :star => nil}
        @width = @precision = @type = nil

        ret << match.pre_match_from(start)
        start = match.begin(0) + 1

        # Special case: %% prints out as "%"
        if [?\n, 0].include?(@fmt[start])
          ret << "%" << @fmt[start]
          start += 1
          next
        elsif [?%, nil].include?(@fmt[start])
          ret << "%"
          start += 1
          next
        elsif @fmt[start, 3] =~ /[1-9]\$/ && !@fmt[start + 2]
          ret << "%"
          start = @fmt.size
          break
        end

        # FLAG STATE
        while token = /\G( |[1-9]\$|#|\+|\-|0|\*)/.match_from(fmt, start)
          case token[0]
          # Special case: if we get two [1-9]\$, it means that we're outside of flag-land
          when /[1-9]\$/
            raise ArgumentError, "value given twice - #{token[0]}" if flags[:position]
            @flags[:position] = token[0][0].chr.to_i
            start += 1
          when " "
            @flags[:space] = true
          when "#"
            @flags[:alternative] = true
          when "+"
            @flags[:plus] = true
          when "-"
            @flags[:minus] = true
          when "0"
            @flags[:zero] = true
          when "*"
            raise ArgumentError, "width given twice" if flags[:star]
            if width_dollar_match = /\G[1-9]\$/.match_from(fmt, start + 1)
              @width = Slot.new('*' << width_dollar_match[0])
              start += 2
            end
            @flags[:star] = true
          end
          start += 1
        end

        # WIDTH STATE
        if !flags[:star] && width_match = /\G([1-9]\$|\*|\d+)/.match_from(fmt, start)
          @width = Slot.new(width_match[0])
          start += width_match[0].size
        end

        # PRECISION DETERMINATION STATE
        if /\G\./.match_from(fmt, start)
          start += 1
          # PRECISION STATE
          if /\G\*/.match_from(fmt, start)
            if precision_dollar_match = /\G[1-9]\$/.match_from(fmt, start + 1)
              @precision = Slot.new('*' << precision_dollar_match[0])
              start += 3
            else
              @precision = Slot.new('*')
              start += 1
            end
          elsif precision_match = /\G([1-9]\$|\d+)/.match_from(fmt, start)
            @precision = Slot.new(precision_match[0])
            start += precision_match[0].size
          else
            @precision = Slot.new("0")
          end

          # check for positional value again, after the optional '.'
          if positional_match = /\G[1-9]\$/.match_from(fmt, start)
            raise ArgumentError, "value given twice - #{token[0]}" if flags[:position]
            @flags[:position] = positional_match[0][0].chr.to_i
            start += 2
          end
        end

        # TYPE STATE
        unless type = /\G[bcdEefGgiopsuXx]/i.match_from(fmt, start)
          raise ArgumentError, "malformed format string - missing field type"
        else
          @type = type[0]
          start += 1
        end

        # Next: Use the parsed values to format some stuff :)
        f = format
        ret << f if f
      end
      if $DEBUG == true && !@positional
        raise ArgumentError, "you need to use all the arguments" unless @arg_position == args.size
      end
      ret << @fmt[start..-1] if start < @fmt.size
      ret.taint if @tainted
      ret
    end

    def format
      # GET VALUE
      if flags[:position]
        val = Slot.new("#{flags[:position]}$")
        val = get_arg(val)
      end

      # GET WIDTH
      @width = Slot.new("*") if flags[:star] && !@width
      width = get_arg(@width)
      width = width.to_int if width.respond_to?(:to_int)
      if width && width < 0
        width = width.abs
        flags[:minus] = true
      end

      # GET PRECISION
      precision = get_arg(@precision)
      precision = precision.to_int if precision.respond_to?(:to_int)

      unless flags[:position]
        val = Slot.new("*")
        val = get_arg(val)
      end

      case @type
      when "e", "E", "f", "g", "G"
        if @type.downcase == "g" && flags[:alternative]
          @old_type = "g"
          @type = "f"
          precision = 4 unless precision
        end
        val = Float(val)
        if val.finite?
          ret = val.send(:to_s_formatted, build_format_string(width, precision))
          ret = plus_char + ret if val >= 0 && @old_type
        else
          ret = (val < 0 ? "-Inf" : "Inf") if val.infinite?
          ret = "NaN" if val.nan?
          ret = plus_char + ret if val > 0
          flags[:zero] = flags[:space] = flags[:plus] = nil
          ret = pad(ret, width, precision)
        end
      when "u"
        val = get_number(val)
        if val < 0
          unless val.kind_of?(Fixnum)
            raise ArgumentError, "invalid type (only Fixnum allowed)"
          end

          val = (2**(2.size * 8)) + val
          if !flags[:zero] and !precision
            ret = "..#{pad(val, width, precision)}"
          else
            ret = pad(val, width, precision)
          end
        else
          ret = pad(val, width, precision)
        end
      when "d", "i"
        val = get_number(val)
        ret = pad(val, width, precision)
      when "c"
        val = val.to_int if val.respond_to?(:to_int)
        raise TypeError, "cannot convert #{val.class} into Integer" unless val.respond_to?(:chr) && val.respond_to?(:%)
        val = (val % 256).chr
        ret = pad(val, width, precision)
      when "s"
        flags[:zero] = flags[:space] = flags[:plus] = nil
        ret = pad(val, width, precision)
        ret.taint if val.tainted?
      when "p"
        flags[:zero] = flags[:space] = flags[:plus] = nil
        ret = pad(val.inspect, width, precision)
      when "o", "x", "X", "b", "B"
        val = get_number(val)
        unless flags[:space] || flags[:plus]
          ret = Number.new(val, RADIXES[@type.downcase]).rep
          chr = val < 0 ? (RADIXES[@type.downcase] - 1).to_s(RADIXES[@type.downcase]) : 0.to_s
          ret = pad(ret, width, precision, chr)
          ret = ALTERNATIVES[@type].to_s + ret if flags[:alternative]
        else
          flags[:plus] = nil if val < 0
          ret = val.to_s(RADIXES[@type.downcase])
          ret.gsub!(/^(\-?)/, "\1#{ALTERNATIVES[@type]}") if flags[:alternative]
          ret = pad(ret, width, precision)
          ret.gsub!(/ \-/, "-")
        end
        ret = ret.downcase if @type == "x"
        ret = ret.upcase if @type == "X"
      end
      ret
    end

    def get_number(val)
      unless val.respond_to?(:full_to_i)
        if val.respond_to?(:to_int)
          val = val.to_int
        elsif val.respond_to?(:to_i)
          val = val.to_i
        end
      end
      val = val.full_to_i if val.respond_to?(:full_to_i)
      val = 0 if val.nil?
      val
    end

    def build_format_string(width, precision)
      ret = "%#{make_flags}#{width}"
      ret << ".#{precision}" if precision
      ret << @type
      ret
    end

    def make_flags
      ret = ""
      ret << " " if flags[:space]
      ret << "#" if flags[:alternative]
      ret << "+" if flags[:plus]
      ret << "-" if flags[:minus]
      ret << "0" if flags[:zero]
      ret
    end

    def get_arg(slot)
      return nil unless slot

      case
      when slot.position == :next
        raise ArgumentError, "unnumbered mixed with numbered" if @positional
        @relative = true
        raise ArgumentError, "you ran out of arguments" if @arg_position >= args.size
        ret = args[@arg_position]
        @arg_position += 1
      when slot.pos
        raise ArgumentError, "unnumbered mixed with numbered" if @relative
        @positional = true
        ret = args[slot.position - 1]
      when slot.value
        raise ArgumentError, "unnumbered mixed with numbered" if @positional
        @relative = true
        ret = slot.value
      else
        raise ArgumentError, "argument position does not exist: #{slot.str}"
      end

      ret
    end

    def pad(val, width, precision, pad_override = nil)
      direction = flags[:minus] ? :ljust : :rjust
      ret = val.to_s
      modded_width = width.to_i + (flags[:plus] ? 1 : 0)
      width = nil if modded_width <= val.to_s.size
      sign = plus_char

      if precision || flags[:zero]
        ret.gsub!("..", "")
      end
      if precision
        if precision > PrecisionMax
          raise ArgumentError, "precision too big"
        end
        ret = sign + ret.send(direction, precision, pad_override || "0")
        flags[:zero] = flags[:plus] = flags[:space] = nil
      end
      if width
        if pad_char != " " && ret[0] == ?-
          ret.slice!(0)
          sign = "-"
          width -= 1
          ret = ret.rjust(width, pad_char)
        else
          ret = ret.send(direction, width, pad_char)
          ret[0] = sign unless sign.empty?
          return ret
        end
      end
      sign + ret
    end

    def pad_char
      flags[:zero] ? "0" : " "
    end

    def plus_char
      return "+" if flags[:plus]
      return " " if flags[:space]
      ""
    end

    class Slot

      # pos means it got a N$ position
      attr_reader :pos
      attr_reader :position
      attr_reader :value
      attr_reader :str

      def initialize(str)
        @pos = false
        @str = str
        if str.size == 3 && /\*\d\$/.match(str)
          @pos = true
          @position = str[1..1].to_i
        elsif str.size == 2 && str[1] == ?$
          @pos = true
          @position = str[0..0].to_i
        elsif str == "*"
          @position = :next
        else
          @value = str.to_i
        end
      end
    end

    class Number

      def initialize(number, radix)
        @number = number
        @radix = radix
        @pad = (radix - 1).to_s(radix)
      end

      def rep
        return @number.to_s(@radix) if(@number >= 0) || @radix == 10
        strlen = (@number.to_s(@radix)).size
        max = (@pad * strlen).to_i(@radix)
        ".." + (max + @number + 1).to_s(@radix)
      end

    end

  end
end
