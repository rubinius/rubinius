class Time
  def self.now
    Rubinius.primitive :time_s_now
    raise PrimitiveFailure, "Time.now primitive failed"
  end

  def self.duplicate(other)
    Rubinius.primitive :time_s_dup
    raise ArgumentError, "descriptors reference invalid time"
  end

  def self.specific(sec, nsec, from_gmt, offset)
    Rubinius.primitive :time_s_specific
    raise ArgumentError, "descriptors reference invalid time"
  end

  def dup
    self.class.duplicate(self)
  end

  def seconds
    Rubinius.primitive :time_seconds
    raise PrimitiveFailure, "Time#second primitive failed"
  end

  def usec
    Rubinius.primitive :time_useconds
    raise PrimitiveFailure, "Time#usec primitive failed"
  end

  def to_a
    Rubinius.primitive :time_decompose
    raise PrimitiveFailure, "Time#to_a primitive failed"
  end

  def strftime(format)
    Rubinius.primitive :time_strftime
    raise PrimitiveFailure, "Time#strftime primitive failed"
  end
end
