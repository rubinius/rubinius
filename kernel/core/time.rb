class Time
  include Comparable

  def self.now
    Ruby.primitive :gettimeofday
  end

  def self.at(secs, msecs = 0)
    self.__at(secs, msecs)
  end

  # Primitive methods can't take default argument values, so we need a wrapper.
  def self.__at(secs, msecs)
    Ruby.primitive :time_at
  end
  
  def strftime(format)
    Ruby.primitive :strftime
  end
  
  def inspect
    strftime("%a %b %d %H:%M:%S %Z %Y")
  end
  
  def seconds
    Ruby.primitive :time_seconds
  end

  def <=>(other)
    self.seconds <=> other.seconds
  end
  
  alias :to_i :seconds

  alias :to_s :inspect
end
