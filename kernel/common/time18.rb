# -*- encoding: us-ascii -*-

class Time
  def self.from_array(sec, min, hour, mday, month, year, nsec, is_dst, from_gmt)
    Rubinius.primitive :time_s_from_array

    sec = sec.kind_of?(String) ? sec.to_i : Rubinius::Type.num2long(sec || 0)
    nsec = Rubinius::Type.num2long(nsec || 0)

    from_array(sec, min, hour, mday, month, year, nsec, is_dst, from_gmt)
  end

  def inspect
    if @is_gmt
      strftime("%a %b %d %H:%M:%S UTC %Y")
    else
      strftime("%a %b %d %H:%M:%S %z %Y")
    end
  end

  alias_method :to_s,       :inspect

  def +(arg)
    raise TypeError, 'time + time?' if arg.kind_of?(Time)

    if arg.kind_of?(Integer)
      other_sec = arg
      other_usec = 0
    else
      other_sec, usec_frac = FloatValue(arg).divmod(1)
      other_usec = (usec_frac * 1_000_000 + 0.5).to_i
    end

    # Don't use self.class, MRI doesn't honor subclasses here
    Time.specific(seconds + other_sec, (usec + other_usec) * 1000, @is_gmt)
  end

  def -(other)
    case other
    when Time
      (seconds - other.seconds) + ((usec - other.usec) * 0.000001)
    when Integer
      # Don't use self.class, MRI doesn't honor subclasses here
      Time.specific(seconds - other, usec * 1000, @is_gmt)
    else
      other = FloatValue(other)

      other_sec, usec_frac = FloatValue(other).divmod(1)
      other_usec = (usec_frac * 1_000_000 + 0.5).to_i

      # Don't use self.class, MRI doesn't honor subclasses here
      Time.specific(seconds - other_sec, (usec - other_usec) * 1000, @is_gmt)
    end
  end

end
