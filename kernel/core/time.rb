class Time
  include Comparable
  
  # Time Constants
  ZoneOffset  	=  	{ 'UTC' => 0, 'Z' => 0,  'UT' => 0, 'GMT' => 0, 
                      'EST' => -5, 'EDT' => -4, 'CST' => -6, 'CDT' => -5, 
                      'MST' => -7, 'MDT' => -6, 'PST' => -8, 'PDT' => -7, 
                      'A' => +1, 'B' => +2, 'C' => +3, 'D' => +4, 'E' => +5, 
                      'F' => +6, 'G' => +7, 'H' => +8, 'I' => +9, 'K' => +10, 
                      'L' => +11, 'M' => +12, 'N' => -1, 'O' => -2, 'P' => -3, 
                      'Q' => -4, 'R' => -5, 'S' => -6, 'T' => -7, 'U' => -8, 
                      'V' => -9, 'W' => -10, 'X' => -11, 'Y' => -12, }

  MonthValue = {
    'JAN' => 1, 'FEB' => 2, 'MAR' => 3, 'APR' => 4, 'MAY' => 5, 'JUN' => 6,
    'JUL' => 7, 'AUG' => 8, 'SEP' => 9, 'OCT' =>10, 'NOV' =>11, 'DEC' =>12
  }
  
  LeapYearMonthDays = [31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31]
  
  CommonYearMonthDays = [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31]
  
  RFC2822_DAY_NAME  	=  	[ 'Sun', 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat' ]
  
  RFC2822_MONTH_NAME  	=  	[ 'Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec' ]
  
  
  # Time methods
  
  def self.new
    Ruby.primitive :gettimeofday
  end
  
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
    strftime("%a %b %d %H:%M:%S %z %Y")
  end
  
  def seconds
    Ruby.primitive :time_seconds
  end

  def <=>(other)
    self.seconds <=> other.seconds
  end
  
  def asctime
    strftime("%a %b %d %H:%M:%S %Y")
  end
  
  def hour
    strftime("%H").to_i
  end
  
  def min
    strftime("%M").to_i
  end
  
  def sec
    strftime('%S').to_i
  end
  
  def day
    strftime("%d").to_i
  end
  
  def year
    strftime("%Y").to_i
  end
  
  def yday
    strftime("%j").to_i
  end
  
  def wday
    strftime("%w").to_i
  end
  
  def zone 
    strftime("%Z")
  end
  
  def mon
    strftime("%m").to_i
  end
  
  def gmt?
    if self.zone == "GMT" or self.zone == "UTC"
      return true
    else
      return false
    end
  end
  
  def usec
    Ruby.primitive :time_usec
  end

  def to_f
    return seconds + (usec / 1000000.0)
  end
  
  def gmt_offset
    ZoneOffset[strftime("%Z")] * 3600
  end
  
  
  alias :utc? :gmt?

  alias :month :mon
  
  alias :ctime :asctime
  
  alias :mday :day
  
  alias :to_i :seconds

  alias :to_s :inspect
  
  alias :tv_sec :seconds
  
  alias :tv_usec :usec
end
