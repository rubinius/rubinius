class Time
  def self.now
    Ruby.primitive :time_s_now
    raise PrimitiveFailure, "Time.now primitive failed"
  end

  def self.specific(sec, usec, from_gmt)
    Ruby.primitive :time_s_specific
    raise ArgumentError, "descriptors reference invalid time"
  end

  def self.from_array(sec, min, hour, mday, month, year, usec, is_dst, from_gmt)
    Ruby.primitive :time_s_from_array
    raise ArgumentError, "descriptors reference invalid time"
  end

  def dup
    Ruby.primitive :time_dup
    raise PrimitiveFailure, "Time#dup failed"
  end

  def seconds
    Ruby.primitive :time_seconds
    raise PrimitiveFailure, "Time#second failed"
  end

  def usec
    Ruby.primitive :time_useconds
    raise PrimitiveFailure, "Time#usec failed"
  end

  def decomposed(gmt)
    Ruby.primitive :time_decompose
    raise PrimitiveFailure, "Time#decompose primitive failed"
  end

  def strftime(format)
    Ruby.primitive :time_strftime
    raise PrimitiveFailure, "Time#strftime primitive failed"
  end
end
