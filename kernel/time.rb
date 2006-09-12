class Time
  self.instance_fields = 2
  
  def self.time_info
    Ruby.primitive :get_time_info
  end
  
  # TimeInfo = self.time_info()
  # TimeSize = TimeInfo.at(0)
  
  def initialize
    data = ByteArray.new(20)
    put 1, data
  end
  
  def make_now
    Ruby.primitive :gettimeofday
  end
  
  def self.now
    obj = Time.new
    obj.make_now
    return obj
  end
  
  def strftime(format)
    Ruby.primitive :strftime
  end
  
  def inspect
    strftime("%+")
  end
end

# p Time::TimeInfo