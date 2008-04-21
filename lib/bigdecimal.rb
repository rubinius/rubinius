# depends on: class.rb numeric.rb regexp.rb string.rb

def BigDecimal(string, _precs=0)
  BigDecimal.new(string, _precs)
end

class BigDecimal < Numeric
  # See stdlib/ext/bigdecimal for MatzRuby implementation.

  # call-seq:
  #   BigDecimal("3.14159")   => big_decimal
  #   BigDecimal("3.14159", 10)   => big_decimal
  def initialize(_val, _precs=0)
    # set up defaults
    @sign = '+'
    @int = '0'
    @frac = '0'
    @exp = '0'
    @special = nil # 'n' for NaN, 'i' for Infinity, nil otherwise
<<<<<<< HEAD:lib/bigdecimal.rb
    
=======

>>>>>>> 991c6e6133839b18ba8390a8e4859818431aa156:lib/bigdecimal.rb
    v = _val.strip
    if v == "NaN"
      @special = 'n'
    elsif v =~ /[-+]?Infinity/
      @special = 'i'
      @sign = '-' if v =~ /-/
    else
      v = _val.gsub('_', '')
      m = /^\s*(([-+]?)(\d*)(?:\.(\d*))?(?:[EeDd](\d+))?).*$/.match(v)
      if !m.nil?
        @sign = m[2] unless m[2].to_s.empty?
        @int = m[3] unless m[3].to_s.empty?
        @frac = m[4] unless m[4].to_s.empty?
        @exp = m[5] unless m[5].to_s.empty?
      end
    @precs = [self.to_s.length, _precs].max
    end
  end
<<<<<<< HEAD:lib/bigdecimal.rb
  
=======

>>>>>>> 991c6e6133839b18ba8390a8e4859818431aa156:lib/bigdecimal.rb
  # As for Float.finite? .
  # call-seq:
  #   BigDecimal.new("Infinity").finite?  => false
  #   BigDecimal.new("NaN").finite?  => true
  def finite?
    @special != 'i'
  end
<<<<<<< HEAD:lib/bigdecimal.rb
  
=======

>>>>>>> 991c6e6133839b18ba8390a8e4859818431aa156:lib/bigdecimal.rb
  # As for Float.nan? .
  # call-seq:
  #   BigDecimal.new("NaN").nan?  => true
  #   BigDecimal.new("123").nan?  => false
  def nan?
    @special == 'n'
  end
<<<<<<< HEAD:lib/bigdecimal.rb
  
=======

>>>>>>> 991c6e6133839b18ba8390a8e4859818431aa156:lib/bigdecimal.rb
  def precs
    [nil, @precs]
  end
<<<<<<< HEAD:lib/bigdecimal.rb
  
=======

>>>>>>> 991c6e6133839b18ba8390a8e4859818431aa156:lib/bigdecimal.rb
  def to_s
    radix = '.'
    e = 'E'
    nan = 'NaN'
    infinity = 'Infinity'
<<<<<<< HEAD:lib/bigdecimal.rb
  
=======

>>>>>>> 991c6e6133839b18ba8390a8e4859818431aa156:lib/bigdecimal.rb
    if self.nan?
      return nan
    end
<<<<<<< HEAD:lib/bigdecimal.rb
    
=======

>>>>>>> 991c6e6133839b18ba8390a8e4859818431aa156:lib/bigdecimal.rb
    if @sign == '+'
      str = ''
    else
      str = '-'
    end
<<<<<<< HEAD:lib/bigdecimal.rb
    
=======

>>>>>>> 991c6e6133839b18ba8390a8e4859818431aa156:lib/bigdecimal.rb
    if self.finite?
      str << @int
      if @frac != '0'
<<<<<<< HEAD:lib/bigdecimal.rb
        str << radix 
=======
        str << radix
>>>>>>> 991c6e6133839b18ba8390a8e4859818431aa156:lib/bigdecimal.rb
        str << @frac
      end
      if @exp != '0'
        str << e
        str << @exp
      end
    else
      str << infinity
    end
    return str
  end

  def coerce(other)
    Ruby.primitive :numeric_coerce
    [BigDecimal(other.to_s), self]
  end
<<<<<<< HEAD:lib/bigdecimal.rb
  
=======

>>>>>>> 991c6e6133839b18ba8390a8e4859818431aa156:lib/bigdecimal.rb
  #########################
  # Arithmetic operations #
  #########################
<<<<<<< HEAD:lib/bigdecimal.rb
  
=======

>>>>>>> 991c6e6133839b18ba8390a8e4859818431aa156:lib/bigdecimal.rb
  # These are stubbed out until we implement them so that their respective specfiles don't crash.
<<<<<<< HEAD:lib/bigdecimal.rb
  
=======

>>>>>>> 991c6e6133839b18ba8390a8e4859818431aa156:lib/bigdecimal.rb
  def +(other)
  end
<<<<<<< HEAD:lib/bigdecimal.rb
  
=======

>>>>>>> 991c6e6133839b18ba8390a8e4859818431aa156:lib/bigdecimal.rb
  def -(other)
  end
<<<<<<< HEAD:lib/bigdecimal.rb
  
=======

>>>>>>> 991c6e6133839b18ba8390a8e4859818431aa156:lib/bigdecimal.rb
  def quo(other)
  end
  alias / quo
<<<<<<< HEAD:lib/bigdecimal.rb
  
=======

>>>>>>> 991c6e6133839b18ba8390a8e4859818431aa156:lib/bigdecimal.rb
  def remainder(other)
  end
  alias % remainder
<<<<<<< HEAD:lib/bigdecimal.rb
  
=======

>>>>>>> 991c6e6133839b18ba8390a8e4859818431aa156:lib/bigdecimal.rb
  def >=(other)
  end
<<<<<<< HEAD:lib/bigdecimal.rb
  
=======

>>>>>>> 991c6e6133839b18ba8390a8e4859818431aa156:lib/bigdecimal.rb
  def <=(other)
  end
<<<<<<< HEAD:lib/bigdecimal.rb
  
=======

>>>>>>> 991c6e6133839b18ba8390a8e4859818431aa156:lib/bigdecimal.rb
  # This will need to be refactored
  def <=>(other)
    if other != 0 or self.nan?
      raise
    elsif self.finite? and @int == '0' and @frac == '0'
      return 0
    else
      case @sign
      when '+'
        return 1
      when '-'
        return -1
      end
    end
  end
<<<<<<< HEAD:lib/bigdecimal.rb
  
=======

>>>>>>> 991c6e6133839b18ba8390a8e4859818431aa156:lib/bigdecimal.rb
  def <(other)
    return self.<=>(other) == -1
  end
<<<<<<< HEAD:lib/bigdecimal.rb
  
=======

>>>>>>> 991c6e6133839b18ba8390a8e4859818431aa156:lib/bigdecimal.rb
  def ==(other)
    self.to_s == other.to_s and self.precs == other.precs
  end

  def >(other)
    return self.<=>(other) == 1
  end
end
