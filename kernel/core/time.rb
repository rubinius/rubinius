class Time
  def self.now
    Ruby.primitive :gettimeofday
  end
  
  def strftime(format)
    Ruby.primitive :strftime
  end
  
  def inspect
    strftime("%+")
  end
  
  def seconds
    Ruby.primitive :time_seconds
  end
  
  alias :to_i :seconds

  alias :to_s :inspect
end
