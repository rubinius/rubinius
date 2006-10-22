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

  alias :to_s :inspect
end
