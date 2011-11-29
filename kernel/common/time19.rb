class Time

  def inspect
    if @is_gmt
      strftime("%Y-%m-%d %H:%M:%S UTC")
    else
      strftime("%Y-%m-%d %H:%M:%S %z")
    end
  end

  alias_method :to_s, :inspect

  # TODO: MRI uses struct timespec in 1.9
  def nsec
    0
  end

  alias_method :tv_nsec, :nsec

  def sunday?
    wday == 0
  end

  def monday?
    wday == 1
  end

  def tuesday?
    wday == 2
  end

  def wednesday?
    wday == 3
  end

  def thursday?
    wday == 4
  end

  def friday?
    wday == 5
  end

  def saturday?
    wday == 6
  end

  def to_r
    to_f.to_r
  end

  def self.local(first, *args)
    if args.size == 9
      sec    = first.kind_of?(String)   ? first.to_i   : Rubinius::Type.num2long(first)
      minute = args[0].kind_of?(String) ? args[0].to_i : Rubinius::Type.num2long(args[0])
      hour =   args[1].kind_of?(String) ? args[1].to_i : Rubinius::Type.num2long(args[1])
      day =    args[2].kind_of?(String) ? args[2].to_i : Rubinius::Type.num2long(args[2])
      month =  args[3].kind_of?(String) ? args[3].to_i : Rubinius::Type.num2long(args[3])
      year =   args[4].kind_of?(String) ? args[4].to_i : Rubinius::Type.num2long(args[4])
      usec =   0
      isdst =  args[7] ? 1 : 0
    else
      # resolve month names to numbers
      month = args[0]
      if month
        if month.kind_of?(String) or month.respond_to?(:to_str)
          month = StringValue(month)

          month = MonthValue[month.upcase] || month.to_i

          raise ArgumentError, "month argument out of range" unless month
        else
          month = Rubinius::Type.num2long(month)
        end
      else
        month = 1
      end

      year =   first.kind_of?(String)   ? first.to_i   : Rubinius::Type.num2long(first)
      day =    args[1].kind_of?(String) ? args[1].to_i : Rubinius::Type.num2long(args[1] || 1)
      hour =   args[2].kind_of?(String) ? args[2].to_i : Rubinius::Type.num2long(args[2] || 0)
      minute = args[3].kind_of?(String) ? args[3].to_i : Rubinius::Type.num2long(args[3] || 0)

      sec  = args[4] || 0
      usec = args[5]

      if sec.kind_of?(Integer) || usec
        sec  = Rubinius::Type.coerce_to sec, Integer, :to_i
        usec = usec ? usec.to_i : 0

        sec  = sec + (usec / 1000000)
        usec = usec % 1000000
      else
        float = FloatValue(sec)
        sec       = float.to_i
        usec_frac = float % 1.0

        if float < 0 && usec_frac > 0
          sec -= 1
        end

        usec = (usec_frac * 1_000_000 + 0.5).to_i
      end

      isdst =  -1
    end

    # This logic is taken from MRI, on how to deal with 2 digit dates.
    if year < 200
      if 0 <= year and year < 39
        warn "2 digit year used: #{year}" if $VERBOSE
        year += 2000
      elsif 69 <= year and year < 139
        warn "2 or 3 digit year used: #{year}" if $VERBOSE
        year += 1900
      end
    end

    from_array(sec, minute, hour, day, month, year, usec, isdst, false)
  end

  class << self
    alias_method :mktime, :local
  end

  def +(arg)
    raise TypeError, 'time + time?' if arg.kind_of?(Time)

    case arg
    when NilClass
      raise TypeError, "can't convert nil into an exact number"
    when String
      raise TypeError, "can't convert String into an exact number"
    when Integer
      other_sec = arg
      other_usec = 0
    else
      arg = Rubinius::Type.coerce_to arg, Rational, :to_r
      other_sec, usec_frac = arg.divmod(1)
      other_usec = (usec_frac * 1_000_000).to_i
    end

    # Don't use self.class, MRI doesn't honor subclasses here
    Time.specific(seconds + other_sec, usec + other_usec, @is_gmt)
  end

  def -(other)
    case other
    when NilClass
      raise TypeError, "can't convert nil into an exact number"
    when String
      raise TypeError, "can't convert String into an exact number"
    when Time
      (seconds - other.seconds) + ((usec - other.usec) * 0.000001)
    when Integer
      # Don't use self.class, MRI doesn't honor subclasses here
      Time.specific(seconds - other, usec, @is_gmt)
    else
      other = Rubinius::Type.coerce_to other, Rational, :to_r

      other_sec, usec_frac = other.divmod(1)
      other_usec = (usec_frac * 1_000_000 + 0.5).to_i

      # Don't use self.class, MRI doesn't honor subclasses here
      Time.specific(seconds - other_sec, usec - other_usec, @is_gmt)
    end
  end

end
