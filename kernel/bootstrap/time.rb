class Time
  def self.allocate
    Ruby.primitive :time_allocate
    raise PrimitiveFailure, "Time.allocate primitive failed"
  end

  def gettimeofday
    Ruby.primitive :time_gettimeofday
    raise PrimitiveFailure, "Time#gettimeofday primitive failed"
  end

  def time_switch(sec, to_gmt)
    Ruby.primitive :time_switch
    raise PrimitiveFailure, "Time#time_switch primitive failed"
  end

  def time_mktime(sec, min, hour, mday, mon, year, usec, isdst, from_gmt)
    Ruby.primitive :time_mktime
    raise ArgumentError, "Time#time_mktime primitive failed, time out of range"
  end

  def __strftime__(tm, format)
    Ruby.primitive :time_strftime
    raise PrimitiveFailure, "Time#__strftime__ primitive failed"
  end
end
