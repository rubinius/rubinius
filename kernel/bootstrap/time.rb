class Time
  def self.gettimeofday
    Ruby.primitive :gettimeofday
    raise PrimitiveFailure, "primitive failed"
  end
  
  def time_switch(sec, to_gmt)
    Ruby.primitive :time_switch
    raise PrimitiveFailure, "primitive failed"
  end
  
  def time_mktime(sec, min, hour, mday, mon, year, usec, isdst, from_gmt)
    Ruby.primitive :mktime
    raise ArgumentError, "time out of range"
  end
  
  def __strftime(tm, format)
    Ruby.primitive :strftime
    raise PrimitiveFailure, "primitive failed"
  end
  
end
