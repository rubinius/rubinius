# -*- encoding: us-ascii -*-

class Time
  def self.at(sec, usec=undefined)
    if undefined.equal?(usec)
      if sec.kind_of?(Time)
        return duplicate(sec)
      elsif sec.kind_of?(Integer)
        return specific(sec, 0, false, nil)
      end
    end

    usec = 0 if undefined.equal?(usec)

    s = Rubinius::Type.coerce_to_exact_num(sec)
    u = Rubinius::Type.coerce_to_exact_num(usec)

    sec       = s.to_i
    nsec_frac = s % 1.0

    sec -= 1 if s < 0 && nsec_frac > 0
    nsec = (nsec_frac * 1_000_000_000 + 0.5).to_i + (u * 1000).to_i

    sec += nsec / 1_000_000_000
    nsec %= 1_000_000_000

    specific(sec, nsec, false, nil)
  end

  def self.from_array(sec, min, hour, mday, month, year, nsec, is_dst, from_gmt, utc_offset)
    Rubinius.primitive :time_s_from_array

    if sec.kind_of?(String)
      sec = sec.to_i
    elsif nsec
      sec = Rubinius::Type.coerce_to(sec || 0, Integer, :to_int)
    else
      s = Rubinius::Type.coerce_to_exact_num(sec || 0)

      sec       = s.to_i
      nsec_frac = s % 1.0

      if s < 0 && nsec_frac > 0
        sec -= 1
      end

      nsec = (nsec_frac * 1_000_000_000 + 0.5).to_i
    end

    nsec ||= 0
    sec += nsec / 1_000_000_000
    nsec %= 1_000_000_000

    if utc_offset
      utc_offset_sec = utc_offset.to_i
      utc_offset_nsec = ((utc_offset % 1.0) * 1_000_000_000 + 0.5).to_i
    else
      utc_offset_sec = 0
      utc_offset_nsec = 0
    end

    from_array(sec, min, hour, mday, month, year, nsec, is_dst, from_gmt, utc_offset, utc_offset_sec, utc_offset_nsec)
  end

  def self.new(year=undefined, month=nil, day=nil, hour=nil, minute=nil, second=nil, utc_offset=nil)
    if undefined.equal?(year)
      now
    elsif utc_offset == nil
      compose(:local, year, month, day, hour, minute, second)
    else
      compose(Rubinius::Type.coerce_to_utc_offset(utc_offset), year, month, day, hour, minute, second)
    end
  end

  def inspect
    if @is_gmt
      strftime("%Y-%m-%d %H:%M:%S UTC")
    else
      strftime("%Y-%m-%d %H:%M:%S %z")
    end
  end

  alias_method :to_s, :inspect

  def nsec
    Rubinius.primitive :time_nseconds
    raise PrimitiveFailure, "Time#nsec primitive failed"
  end

  alias_method :tv_nsec, :nsec

  def subsec
    if nsec == 0
      0
    else
      Rational(nsec, 1_000_000_000)
    end
  end

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
    (seconds + subsec).to_r
  end

  def to_f
    to_r.to_f
  end

  def +(other)
    raise TypeError, 'time + time?' if other.kind_of?(Time)

    case other = Rubinius::Type.coerce_to_exact_num(other)
    when Integer
      other_sec = other
      other_nsec = 0
    else
      other_sec, nsec_frac = other.divmod(1)
      other_nsec = (nsec_frac * 1_000_000_000).to_i
    end

    # Don't use self.class, MRI doesn't honor subclasses here
    Time.specific(seconds + other_sec, nsec + other_nsec, @is_gmt, @offset)
  end

  def -(other)
    if other.kind_of?(Time)
      return (seconds - other.seconds) + ((usec - other.usec) * 0.000001)
    end

    case other = Rubinius::Type.coerce_to_exact_num(other)
    when Integer
      other_sec = other
      other_nsec = 0
    else
      other_sec, nsec_frac = other.divmod(1)
      other_nsec = (nsec_frac * 1_000_000_000 + 0.5).to_i
    end

    # Don't use self.class, MRI doesn't honor subclasses here
    Time.specific(seconds - other_sec, nsec - other_nsec, @is_gmt, @offset)
  end

  def localtime(offset=nil)
    offset = Rubinius::Type.coerce_to_utc_offset(offset) unless offset.nil?

    if @is_gmt or offset
      @is_gmt = false
      @offset = offset
      @decomposed = nil
    end

    self
  end

  def getlocal(offset=nil)
    dup.localtime(offset)
  end

  def eql?(other)
    other.kind_of?(Time) and seconds == other.seconds and nsec == other.nsec
  end

  def <=>(other)
    if other.kind_of? Time
      (seconds <=> other.seconds).nonzero? or (nsec <=> other.nsec)
    else
      r = (other <=> self)
      return nil if r == nil
      return -1 if r > 0
      return  1 if r < 0
      0
    end
  end

  #
  # Rounds sub seconds to a given precision in decimal digits
  #
  # Returns a new time object.
  #
  # places should be nonnegative, it is 0 by default.
  #
  def round(places = 0)
    return dup if nsec == 0

    roundable_time = (to_i + subsec.to_r).round(places)

    sec = roundable_time.floor
    nano = ((roundable_time - sec) * 1_000_000_000).floor

    Time.specific(sec, nano, @is_gmt, @offset)
  end
end
