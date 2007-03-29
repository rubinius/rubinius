
module Sprintf

  class Parser   

    def self.format(str, args)
      out  = []
      # The instance contains all the state handling
      fmtstr = Sprintf::Parser.new(str, args)
      
      while fmtstr.index('%')
        if !fmtstr.end_of_string? && fmtstr[1] == ?%
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
              fmt.value = fmtstr.get_argument(number - 1)
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
            fmt.value = fmtstr.get_next_argument if fmt.value.nil?
            fmtstr.next
            break
          else
            raise ArgumentError,"malformed format string"
          end
        end
        if fmt.type.nil?
          raise ArgumentError, "malformed format string - missing field type"
        end
        # Pop another argument off the stack if no absolute reference provided
        fmt.value = fmtstr.get_next_argument if fmt.value.nil?
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
    
    def end_of_string?
      @idx >= @len
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
      raise ArgumentError, "value given twice " if !@value.nil?
      @value = v
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
          s = (v.infinite? == -1 ? '-' : '') << (v.nan? ? "Nan" : "Inf")
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
          out << __float_sprintf__(fmt, v)
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
    
    def __float_sprintf__(fmt, v)
      Ruby.primitive :float_sprintf
      #sprintf(fmt,v)
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

