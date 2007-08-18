# _load
# parse
# --------------------
# _dump
# marshal_dump
# marshal_load

class Time
  include Comparable
  
  # Time Constants
  ZoneOffset  	=  	{ 'UTC' => 0, 'Z' => 0,  'UT' => 0, 'GMT' => 0, 
                      'EST' => -5, 'EDT' => -4, 'CST' => -6, 'CDT' => -5, 
                      'CET' => 1, 'CEST' => 2,
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
  
  RFC2822_DAY_NAME = ['Sun', 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat']
  
  RFC2822_MONTH_NAME = ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec']
  
  PRE_EPOCH_DAYS = 719468
  
  # Time methods
  
  def initialize
    @timeval = Time.gettimeofday

    # this flag specifies whether this Time instance represents
    # local time or GMT. it is independent of the actual time zone.
    @is_gmt = false

    @tm = time_switch(@timeval.first, false)
  end

  def dup
    t = Time.allocate
    t.instance_variable_set(:@timeval, @timeval)
    t.instance_variable_set(:@tm, @tm)
    t.instance_variable_set(:@is_gmt, @is_gmt)
    t
  end
  
  def self.local(first, *args)
    if args.size == 9
      second = first
      minute = args[0]
      hour = args[1]
      day = args[2]
      month = args[3]
      year = args[4]
      usec = 0
      isdst = args[7] ? 1 : 0
    else
      # resolve month names to numbers
      if args[0] && args[0].kind_of?(String)
        month = MonthValue[args[0].upcase] || raise(ArgumentError.new('argument out of range'))
      end
      
      year = first
      month ||= args[0] || 1
      day = args[1] || 1
      hour = args[2] || 0
      minute = args[3] || 0
      second = args[4] || 0
      usec = args[5] || 0
      isdst = -1
    end

    t = Time.allocate
    t.mktime(second, minute, hour, day, month, year, usec, isdst, false)
    t.force_localtime
  end
  
  def self.gm(first, *args)
    if args.size == 9
      second = first
      minute = args[0]
      hour = args[1]
      day = args[2]
      month = args[3]
      year = args[4]
      usec = 0
    else
      # resolve month names to numbers
      if args[0] && args[0].kind_of?(String)
        month = MonthValue[args[0].upcase] || raise(ArgumentError.new('argument out of range'))
      end
      
      # set argument defaults
      year = first
      month ||= args[0] || 1
      day = args[1] || 1
      hour = args[2] || 0
      minute = args[3] || 0
      second = args[4] || 0
      usec = args[5] || 0
    end

    t = Time.allocate
    t.mktime(second, minute, hour, day, month, year, usec, -1, true)
    t.force_gmtime
  end

  def self.at(secs_or_time, msecs = 0)
    if secs_or_time.kind_of?(Integer)
      Time.allocate.at_gmt(secs_or_time, msecs, false)
    else
      secs_or_time.dup
    end
  end
  
  def strftime(format)
    __strftime(@tm, format.to_str)
  end
  
  def inspect
    if @is_gmt
      strftime("%a %b %d %H:%M:%S UTC %Y")
    else
      strftime("%a %b %d %H:%M:%S %z %Y")
    end
  end

  def seconds
    @timeval.first
  end

  def +(other)
    dup.at_gmt(seconds + other, usec, @is_gmt)
  end
  
  def -(other)
    if other.kind_of? Time
      seconds - other.seconds + (usec - other.usec) * 0.000001
    else
      dup.at_gmt(seconds - other, usec, @is_gmt)
    end
  end
  
  def succ
    self + 1
  end

  def <=>(other)
    [self.seconds, self.usec] <=> [other.seconds, other.usec]
  end
  
  def eql?(other)
    (self <=> other) == 0
  end
  
  def asctime
    strftime("%a %b %e %H:%M:%S %Y")
  end
  
  def hour
    @tm[2]
  end
  
  def min
    @tm[1]
  end
  
  def sec
    @tm[0]
  end
  
  def day
    @tm[3]
  end
  
  def year
    @tm[5] + 1900
  end
  
  def yday
    @tm[7] + 1
  end
  
  def wday
    @tm[6]
  end
  
  def zone 
    strftime("%Z")
  end
  
  def mon
    @tm[4] + 1
  end
  
  def gmt?
    @is_gmt
  end
  
  def usec
    @timeval.last
  end
  
  def to_i
    self.seconds
  end

  def to_f
    seconds + (usec / 1000000.0)
  end
  
  # [ sec, min, hour, day, month, year, wday, yday, isdst, zone ]
  def to_a
    [sec, min, hour, day, month, year, wday, yday, isdst, zone]
  end
  
  def gmt_offset
    return 0 if @is_gmt

    other = dup.gmtime

    if year != other.year
      offset = year < other.year ? -1 : 1
    elsif month != other.month
      offset = month < other.month ? -1 : 1
    elsif mday != other.mday
      offset = mday < other.mday ? -1 : 1
    else
      offset = 0
    end

    offset *= 24
    offset += hour - other.hour

    offset *= 60
    offset += min - other.min

    offset *= 60
    offset += sec - other.sec
  end
  
  def localtime
    force_localtime if @is_gmt

    self
  end
  
  def gmtime
    force_gmtime unless @is_gmt

    self
  end
  
  def dst?
    !@tm[8].zero?
  end
  
  def getlocal
    dup.localtime
  end
  
  def getgm
    dup.gmtime
  end
  
  def hash
    seconds ^ usec
  end
  
  # internal
  
  # private
  
  def force_localtime
    @tm = time_switch(@timeval.first, false)
    @is_gmt = false

    self
  end

  def force_gmtime
    @tm = time_switch(@timeval.first, true)
    @is_gmt = true

    self
  end

  def mktime(sec, min, hour, mday, mon, year, usec, isdst, from_gmt)
    @timeval = time_mktime(sec, min, hour, mday, mon, year, usec, isdst, from_gmt)
    self
  end

  # sec and usec are always given in gmt here.
  # want_gmt says whether the caller wants a gmtime or
  # local time object.
  def at_gmt(sec, usec, want_gmt)
    @timeval = [sec, usec]

    if want_gmt
      force_gmtime
    else
      force_localtime
    end
  end

  def self.month_days(y, m)
    if ((y % 4 == 0) && (y % 100 != 0)) || (y % 400 == 0)
      LeapYearMonthDays[m-1]
    else
      CommonYearMonthDays[m-1]
    end
  end

  def self.apply_offset(year, mon, day, hour, min, sec, off)
    if off < 0
      off = -off
      off, o = off.divmod(60)
      if o != 0 then sec += o; o, sec = sec.divmod(60); off += o end
      off, o = off.divmod(60)
      if o != 0 then min += o; o, min = min.divmod(60); off += o end
      off, o = off.divmod(24)
      if o != 0 then hour += o; o, hour = hour.divmod(24); off += o end
      if off != 0
        day += off
        if month_days(year, mon) < day
          mon += 1
          if 12 < mon
            mon = 1
            year += 1
          end
          day = 1
        end
      end
    elsif 0 < off
      off, o = off.divmod(60)
      if o != 0 then sec -= o; o, sec = sec.divmod(60); off -= o end
      off, o = off.divmod(60)
      if o != 0 then min -= o; o, min = min.divmod(60); off -= o end
      off, o = off.divmod(24)
      if o != 0 then hour -= o; o, hour = hour.divmod(24); off -= o end
      if off != 0 then
        day -= off
        if day < 1
          mon -= 1
          if mon < 1
            year -= 1
            mon = 12
          end
          day = month_days(year, mon)
        end
      end
    end
    return year, mon, day, hour, min, sec
  end
  
  # aliases
  
  public
  
  class << self
    alias :now :new
    alias :mktime :local
    alias :utc :gm
  end
    
  alias :utc? :gmt?
  alias :month :mon
  alias :ctime :asctime
  alias :mday :day
  alias :to_i :seconds
  alias :to_s :inspect
  alias :tv_sec :seconds
  alias :tv_usec :usec
  alias :utc :gmtime
  alias :isdst :dst?
  alias :utc_offset :gmt_offset
  alias :gmtoff :gmt_offset
  alias :getutc :getgm
end
