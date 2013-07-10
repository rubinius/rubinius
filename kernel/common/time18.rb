# -*- encoding: us-ascii -*-

class Time
  def self.times
    Process.times
  end

  def self.at(sec, usec=undefined)
    if !undefined.equal?(usec)
      sec  = Rubinius::Type.coerce_to(sec,  Integer, :to_int)
      usec = Rubinius::Type.coerce_to(usec, Integer, :to_int)

      nsec = (usec * 1000).to_i
      sec = sec + (nsec / 1000000000)
      nsec = nsec % 1000000000

      specific(sec, nsec, false, nil)
    elsif sec.kind_of?(Time)
      duplicate(sec)
    elsif sec.kind_of?(Integer)
      specific(sec, 0, false, nil)
    elsif sec.kind_of?(String)
      raise TypeError, "can't convert #{sec} into an exact number"
    else
      float = FloatValue(sec)
      sec       = float.to_i
      nsec_frac = float % 1.0

      if float < 0 && nsec_frac > 0
        sec -= 1
      end

      nsec = (nsec_frac * 1_000_000_000 + 0.5).to_i
      specific(sec, nsec, false, nil)
    end
  end

  class << self
    alias_method :new, :now
  end

  def self.from_array(sec, min, hour, mday, month, year, nsec, is_dst, from_gmt, utc_offset)
    Rubinius.primitive :time_s_from_array

    sec = sec.kind_of?(String) ? sec.to_i : Rubinius::Type.coerce_to(sec || 0, Integer, :to_int)

    if utc_offset
      utc_offset_sec = utc_offset.to_i
      utc_offset_nsec = ((utc_offset % 1.0) * 1_000_000_000 + 0.5).to_i
    else
      utc_offset_sec = 0
      utc_offset_nsec = 0
    end

    from_array(sec, min, hour, mday, month, year, nsec || 0, is_dst, from_gmt, utc_offset, utc_offset_sec, utc_offset_nsec)
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
    Time.specific(seconds + other_sec, (usec + other_usec) * 1000, @is_gmt, @offset)
  end

  def -(other)
    if other.kind_of?(Time)
      return (seconds - other.seconds) + ((usec - other.usec) * 0.000001)
    end

    case other
    when Integer
      other_sec = other
      other_usec = 0
    else
      other = FloatValue(other)
      other_sec, usec_frac = FloatValue(other).divmod(1)
      other_usec = (usec_frac * 1_000_000 + 0.5).to_i
    end

    # Don't use self.class, MRI doesn't honor subclasses here
    Time.specific(seconds - other_sec, (usec - other_usec) * 1000, @is_gmt, @offset)
  end

  def localtime
    if @is_gmt
      @is_gmt = false
      @decomposed = nil
    end

    self
  end

  def getlocal
    dup.localtime
  end

  def eql?(other)
    other.kind_of?(Time) and seconds == other.seconds and usec == other.usec
  end

  def <=>(other)
    if other.kind_of? Time
      (seconds <=> other.seconds).nonzero? or (usec <=> other.usec)
    else
      nil
    end
  end

  def to_f
    seconds + (usec * 0.000001)
  end
end
