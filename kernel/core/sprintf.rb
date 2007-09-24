
module Sprintf

  class Parser   

    def self.format(str, args)
      out  = []
      # The instance contains all the state handling
      fmtstr = Sprintf::Parser.new(str, args)
      numbered = nil
      
      while fmtstr.index('%')
        if !fmtstr.end_of_string? && (fmtstr[1] == ?% || fmtstr[1] == 0)
          fmtstr.next
          out << fmtstr.get_mark_to_cursor
          fmtstr.next
          fmtstr.drop_mark_point
          next
        end
        
        if fmtstr.cursor > fmtstr.mark
          out << fmtstr.get_mark_to_cursor
        end
        
        fmt = Sprintf::Formatter.new
        fmtstr.next
        while !fmtstr.end_of_string?
          c = fmtstr[]
          case c
          when ?0
            fmt.flags |= Sprintf::Formatter::PAD_ZERO
            fmtstr.next
          when ?#
            fmt.flags |= Sprintf::Formatter::PAD_RADIX
            fmtstr.next
          when ?+
            fmt.flags |= Sprintf::Formatter::PAD_PLUS
            fmtstr.next
          when ?-
            fmt.flags |= Sprintf::Formatter::PAD_LJUSTIFY
            fmtstr.next
          when 32
            fmt.flags |= Sprintf::Formatter::PAD_SPACE
            fmtstr.next
          when ?*
            value = fmtstr.get_argument_value
            fmt.width = value
            
          when ?1,?2,?3,?4,?5,?6,?7,?8,?9
            number = fmtstr.get_number
            if fmtstr[] == ?$
              raise ArgumentError, "value given twice #{number}$" if !fmt.value.nil?
              raise ArgumentError, "mixing numbered with unnumbered" if numbered == false
              fmt.value = fmtstr.get_argument(number - 1)
              numbered = true
              fmtstr.next
            else
              fmt.width = number
            end
            
          when ?.
            fmtstr.next
            if fmtstr[] == ?*
              fmt.precision = fmtstr.get_argument_value
            else
              fmt.precision = fmtstr.get_number
            end
            
          when ?B, ?b,?c,?d,?E,?e,?f,?G,?g,?i,?o,?p,?s,?u,?X,?x
            fmt.type = c
            if fmt.unset?
              raise ArgumentError, "mixing numbered with unnumbered" if numbered == true
              fmt.value = fmtstr.get_next_argument if fmt.unset?
              numbered = false
            end
            fmtstr.next
            break
          else
            raise ArgumentError,"malformed format string"
          end
        end
        if fmt.type.nil?
          puts fmtstr.instance_variable_get("@str")
          unless (fmtstr.next_char == "\n") || fmt.value
            raise ArgumentError, "malformed format string - missing field type" 
          else
            # fmtstr.next
            # out << fmtstr.get_mark_to_cursor
            out << "%"
            fmtstr.drop_mark_point
            next
          end
        end
        # Pop another argument off the stack if no absolute reference provided
        if fmt.unset?
          fmt.value = fmtstr.get_next_argument
          raise ArgumentError, "mixing numbered with unnumbered" if numbered == true
        end
        out << fmt
        fmtstr.drop_mark_point
      end
      
      if fmtstr.cursor > fmtstr.mark
        out << fmtstr.get_mark_to_cursor
      end
      
      # Now render out the captured format
      str = ""
      out.each do |s|
        str << s.to_s
      end
      
      str
    end
    
    def initialize(str, args)
      @str  = str
      @len  = str.length
      @idx  = 0
      @mark = 0
      
      if !(Array === args)
        args = [args]
      end
      
      @args    = args
      @arg_idx = 0
    end
    
    # return a char with a relative offset to the index
    def [](offset=0)
      @str.data[@idx+offset]
    end
    
    def len
      @len
    end
    
    def cursor
      @idx
    end
    
    def next
      @idx += 1
    end
    
    def rewind
      @idx  = 0
      @mark = 0
    end
    
    def last_char?
      @idx == @len - 1
    end
    
    def end_of_string?
      @idx >= @len
    end
    
    def cur_char
      @str[@idx]
    end
    
    def next_char
      @str[@idx + 1]
    end
    
    def index(char)
      tmp = @str.index(char, @idx)
      @idx = tmp.nil? ? @len : tmp
      tmp
    end
    
    def drop_mark_point
      @mark = @idx
    end
    
    def mark
      @mark
    end
    
    def get_mark_to_cursor
      @str.substring(@mark, @idx - @mark)
    end
    
    def get_argument(idx)
      raise ArgumentError, "too few arguments" if idx >= @args.length
      @arg_idx = idx
      @args[idx]
    end
    
    def get_next_argument
      raise ArgumentError, "too few arguments" if @arg_idx >= @args.length
      v = @args[@arg_idx]
      @arg_idx += 1
      v
    end
  
    def get_number
      n = number = 0
      while !self.end_of_string?
        n = self[]
        break if n < ?0 || n > ?9
        number = number * 10 + n - ?0
        self.next
      end
      if self.end_of_string?
        raise ArgumentError, "malformed format string - %.[0-9]"
      end
      number
    end
    
    def get_argument_value
      self.next if self[] == ?*
      if self[] >= ?0 && self[] <= ?9
        number = get_number
        if self[] != ?$
          raise ArgumentError, "malformed format string - argument number missing $"
        end
        self.next
        return self.get_argument(number - 1)
      end
      self.get_next_argument    
    end
    
  end
  
  class Formatter
    
    PAD_ZERO     = 1
    PAD_SPACE    = 2
    PAD_PLUS     = 4
    PAD_RADIX    = 8
    PAD_LJUSTIFY = 16
    
    def initialize
      @flags     = 0
      @value     = nil
      @unset     = true
      @type      = nil
      @width     = nil
      @precision = nil
    end
    
    def flags
      @flags
    end
    
    def flags=(v)
      @flags = v
    end
    
    def value
      @value
    end
    
    def value=(v)
      raise ArgumentError, "value given twice " unless @unset
      @unset = false
      @value = v
    end

    def unset?
      @unset
    end
    
    def type
      @type
    end
    
    def type=(v)
      raise ArgumentError, "type given twice " if !@type.nil?
      @type = v
    end
  
    def width
      @width
    end
    
    def width=(v)
      raise ArgumentError, "width given twice " if !@width.nil?
      @width = Integer(v)
    end
    
    def precision
      @precision
    end
    
    def precision=(v)
      raise ArgumentError, "precision given twice" if !@precision.nil?
      @precision = Integer(v)
    end
    
    def to_s
      out = ""
      case @type
      when ?c
        # character
        v = Integer(@value)
        out << fill((v & 0xff).chr)
      when ?s
        # string
        s = String(@value)
        s = truncate(s)
        out << fill(s)
      when ?p
        # argument.inspect
        s = @value.inspect
        s = truncate(s)
        out << fill(s)
        
      when ?d, ?i
        # decimal (signed integer)
        out << integer_value(10, true)
        
      when ?x, ?X
        # hexadecimal
        out << integer_value(16, false)
        out.upcase! if @type == ?X
        
      when ?o
        # octal
        out << integer_value(8, false)
  
      when ?b, ?B
        # binary
        out << integer_value(2, false)
        
      when ?e, ?E, ?f, ?g, ?G
        v = Float(@value)
        if v.nan? == true || v.infinite?
          s = (v.infinite? == -1 ? '-' : '') << (v.nan? ? "nan" : "Inf")
          s = zeropad(s, @width) if @width && (@flags & PAD_LJUSTIFY) == 0
          s = sign(s, v)
          out << fill(s)
        else
          fmt = "%"
          fmt << ' ' if @flags & PAD_SPACE    == PAD_SPACE
          fmt << ?#  if @flags & PAD_RADIX    == PAD_RADIX
          fmt << ?+  if @flags & PAD_PLUS     == PAD_PLUS
          fmt << ?-  if @flags & PAD_LJUSTIFY == PAD_LJUSTIFY
          fmt << ?0  if @flags & PAD_ZERO     == PAD_ZERO
          
          fmt << @width.to_s            if @width
          fmt << '.' << @precision.to_s if @precision
          fmt << @type
          out << v.to_s_formatted(fmt)
        end
        
      when ?u
        # print unsigned integers - if negative print the twos complement with
        # a leading .. to indicate infinite number of sign bits
        out << integer_value(10, false)
      else
        raise ArgumentError, "Unrecognized type"
      end
      out
    end
    
    def fill(str)
      len = str.length
      if @width && len < @width
        str = @flags & PAD_LJUSTIFY != 0 ? str << ' ' * (width - len) : ' ' * (width - len) << str
      end
      str
    end
    
    def truncate(str)
      if @precision && @precision < str.length
        str = str.substring(0, @precision)
      end
      str
    end
    
    def radix(str)
      return str if @flags & PAD_RADIX == 0
      radix = case @type
      when ?b
        "0b"
      when ?B
        "0B"
      when ?o
        "0"
      when ?x
        "0x"
      when ?X
        "0X"
      else
        ""
      end
      radix << str
    end
    
    def onespad(str, base)
      return str if @flags & PAD_ZERO == 0 && @precision.nil?
      width = @precision ? @precision : @width
      return str if str.length >= width
  
      c = case base
      when 2
        "1"
      when 8
        "7"
      when 10
        "."
      when 16
        "f"
      end
      c * (width - str.length) << str
    end
    
    def zeropad(str, width)
      return str if str.length >= width
      '0' * (width - str.length) << str
    end
    
    def sign(str, v)
      return str if v < 0
      return '+' << str if @flags & PAD_PLUS  == PAD_PLUS
      return ' ' << str if @flags & PAD_SPACE == PAD_SPACE
      str
    end
    
    def twos_complement_value(v)
      #Bignum & Fixnum
      #Ruby.primitive :bignum_twos_complement
      #Ruby.primitive :fixnum_twos_complement
      v
    end
        
    def integer_value(base, signed)
      # Negative numbers:
      # [+, space] : does not two's complement the number
      #
      # Suppress (..) for negaive number with the following -
      # [0]        : if width is supplied pads with 0 to width
      # [.dd]      : if precision is supplied pads with 0 to precision width
      v = Integer(@value)
  
      if v < 0 && (@flags & (PAD_PLUS|PAD_SPACE)) == 0 && !signed
        v = twos_complement_value(-v)
        s = v.to_s(base)
        s = onespad(s, base)
      else
        s = v.to_s(base)
      end
      
      if (@flags & PAD_ZERO) == PAD_ZERO || @precision
        width = @width ? @width : 0
        s = zeropad(s, !@precision.nil? ? @precision : width)
      end
      
      s = radix(s)
      s = sign(s, v)
      fill(s)
    end
  end
end


class YSprintf
  
  attr_accessor :fmt
  attr_accessor :args
  attr_accessor :flags
  def initialize(fmt, *args)
    @fmt, @args, @arg_position = fmt, args, 0
  end
  
  def parse
    start = 0
    ret = ""
    width = nil
    precision = nil
    @arg_position = 0
    
    while (match = /%/.match_from(fmt, start))
      @flags = {:space => nil, :position => nil, :alternative => nil, :plus => nil, 
        :minus => nil, :zero => nil, :star => nil}
      @width = @precision = @type = nil

      ret << match.pre_match_from(start)
      start = match.begin(0) + 1
      # FLAG STATE
      while token = /\G( |\d\$|#|\+|\-|0|\*)/.match_from(fmt, start)
        case token[0]
        # Special case: if we get two \d\$, it means that we're outside of flag-land
        when /\d\$/
          break if flags[:position]
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
          @flags[:star] = true
        end
        start += 1
      end
      
      # WIDTH STATE
      if width_match = /\G(\d\$|\*|\d+)/.match_from(fmt, start)
        @width = Slot.new(width_match[0])
        start += width_match[0].size
      end
    
      # PRECISION DETERMINATION STATE
      if /\G\./.match_from(fmt, start)
        start += 1
        # PRECISION STATE
        if precision_match = /\G(\d\$|\*|\d+)/.match_from(fmt, start)
          @precision = Slot.new(precision_match[0])
          start += precision_match[0].size
        else
          @precision = Slot.new("0")
        end
      end
    
      # TYPE STATE
      unless type = /\G[bcdEefGgiopsuXx]/.match_from(fmt, start)
        raise ArgumentError, "malformed format string - missing field type" 
      else
        @type = type[0]
        start += 1
      end
    
      # Next: Use the parsed values to format some stuff :)
      f = format
      ret << f if f
    end
    ret
  end
  
  def format
    raise ArgumentError, "provided width twice" if flags[:star] && @width
    
    # GET VALUE
    if flags[:position]
      val = args[flags[:position] - 1]
      raise ArgumentError, "you specified an argument position that did not exist" unless val
    else
      val = args[@arg_position]
      @arg_position += 1
      raise ArgumentError, "you have more format specifiers than arguments" unless val      
    end

    
    # GET WIDTH
    @width = Slot.new("*") if flags[:star]
    width = get_arg(@width)
    
    # GET PRECISION
    precision = get_arg(@precision) || 0
    
    case @type
    when "d"
      ret = pad(val.to_s(10), width, precision)
    when "b"
      ret = pad(val.to_s(2), width, precision)
    end
    ret
  end
  
  def get_arg(slot)
    return nil unless slot
    if slot.position == :next
      ret = args[@arg_position]
      @arg_position += 1
    elsif slot.pos
      ret = args[slot.position - 1]
    elsif slot.value
      ret = slot.value
    else
      ret = nil
    end
    raise ArgumentError, "you specified an argument position that did not exist: #{slot.str}" unless ret
    ret
  end
  
  def pad(val, width, precision)
    direction = flags[:minus] ? :ljust : :rjust
    ret = val
    width = nil if width.to_i <= val.size
    if precision
      ret = plus_char + ret.send(direction, precision, "0") 
      flags[:zero] = flags[:plus] = flags[:space] = nil
    end
    if width
      ret = ret.send(direction, width, pad_char)
      ret[0] = plus_char unless plus_char.empty?
    else
      ret = plus_char + ret
    end
    ret
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
      if str.size == 2 && str[1] == ?$
        @pos = true
        @position = str[0..0].to_i
      elsif str == "*"
        @position = :next
      else
        @value = str.to_i
      end
    end
  end
  
end