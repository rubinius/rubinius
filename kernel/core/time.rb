# _load
# parse
# --------------------
# _dump
# marshal_dump
# marshal_load
# xmlschema

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
    @tv_sec, @tv_usec, @tz_minuteswest, @tz_dsttime = Time.gettimeofday
  end
  
  def self.local(first, *args)
    parts = self.now.parts
    
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
      
      year = first
      month ||= args[0] || 1
      day = args[1] || 1
      hour = args[2] || 0
      minute = args[3] || 0
      second = args[4] || 0
      usec = args[5] || 0
    end
            
    tv_sec = ((self.epoch_day(year, month, day) - PRE_EPOCH_DAYS) * 86400) + (parts[2] * 60)
    
    dst = self.at(tv_sec, usec).hour == 1
    
    tv_sec -= 3600 if dst
    tv_sec += (hour * 3600) + (minute * 60) + second
    Time.new.set(tv_sec, usec, nil, !dst)
  end
  
  def self.gm(first, *args)
    parts = self.now.parts
    
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
            
    tv_sec = ((self.epoch_day(year, month, day) - PRE_EPOCH_DAYS) * 86400)
    tv_sec += (hour * 3600) + (minute * 60) + second
    Time.new.set(tv_sec, usec, 0, 0)
  end

  def self.at(secs_or_time, msecs = 0)
    if secs_or_time.kind_of?(Integer)
      t = Time.new.set(secs_or_time, msecs)
      # t.set(nil, nil, nil, Time.local(*t.to_a).dst?)
    else
      secs_or_time.dup
    end
  end
  
  def strftime(format)
    __strftime(@tv_sec, format, @tz_minuteswest)
  end
  
  def inspect
    strftime("%a %b %d %H:%M:%S %z %Y")
  end
  
  def seconds
    @tv_sec
  end
  
  def +(other)
    self.dup.set(self.seconds + other)
  end
  
  def -(other)
    self.dup.set(self.seconds - other)
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
    self.zone == "GMT" || self.zone == "UTC"
  end
  
  def usec
    @tv_usec
  end
  
  def to_i
    self.seconds
  end

  def to_f
    seconds + (usec / 1000000.0)
  end
  
  # [ sec, min, hour, day, month, year, wday, yday, isdst, zone ]
  def to_a
    a = strftime("%S %M %H %d %m %Y %w %j").split(' ').map { |x| x.to_i }
    a + [self.dst?, zone]
  end
  
  def gmt_offset
    ZoneOffset[strftime("%Z")] * 3600
  end
  
  def localtime
    t = Time.at(self.seconds)
    self.set(*t.parts)
    self
  end
  
  def gmtime
    self.set(nil, nil, 0, 0)
    self
  end
  
  def dst?
    @tz_dsttime == 1
  end
  
  def getlocal
    t = Time.at(self.seconds)
    self.dup.set(*t.parts)
  end
  
  def getgm
    self.dup.set(nil, nil, 0, 0)
  end
  
  def hash
    @tv_sec ^ @tv_usec
  end
  
  # Parses +date+ as date-time defined by RFC 2822 and converts it to a Time
  # object.  The format is identical to the date format defined by RFC 822 and
  # updated by RFC 1123.
  #
  # ArgumentError is raised if +date+ is not compliant with RFC 2822
  # or Time class cannot represent specified date.
  #
  # See #rfc2822 for more information on this format.
  def self.rfc2822(date)
    if /\A\s*
        (?:(?:Mon|Tue|Wed|Thu|Fri|Sat|Sun)\s*,\s*)?
        (\d{1,2})\s+
        (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)\s+
        (\d{2,})\s+
        (\d{2})\s*
        :\s*(\d{2})\s*
        (?::\s*(\d{2}))?\s+
        ([+-]\d{4}|
         UT|GMT|EST|EDT|CST|CDT|MST|MDT|PST|PDT|[A-IK-Z])/ix =~ date
      # Since RFC 2822 permit comments, the regexp has no right anchor.
      day = $1.to_i
      mon = MonthValue[$2.upcase]
      year = $3.to_i
      hour = $4.to_i
      min = $5.to_i
      sec = $6 ? $6.to_i : 0
      zone = $7

      # following year completion is compliant with RFC 2822.
      year = if year < 50
        2000 + year
      elsif year < 1000
        1900 + year
      else
        year
      end

      year, mon, day, hour, min, sec = apply_offset(year, mon, day, hour, min, sec, zone_offset(zone))
      t = self.utc(year, mon, day, hour, min, sec)
      t.localtime if !zone_utc?(zone)
      t
    else
      raise ArgumentError.new("not RFC 2822 compliant date: #{date.inspect}")
    end
  end
  
  # Parses +date+ as HTTP-date defined by RFC 2616 and converts it to a Time
  # object.
  #
  # ArgumentError is raised if +date+ is not compliant with RFC 2616 or Time
  # class cannot represent specified date.
  #
  # See #httpdate for more information on this format.
  def self.httpdate(date)
    if /\A\s*
        (?:Mon|Tue|Wed|Thu|Fri|Sat|Sun),\x20
        (\d{2})\x20
        (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)\x20
        (\d{4})\x20
        (\d{2}):(\d{2}):(\d{2})\x20
        GMT
        \s*\z/ix =~ date
      self.rfc2822(date)
    elsif /\A\s*
           (?:Monday|Tuesday|Wednesday|Thursday|Friday|Saturday|Sunday),\x20
           (\d\d)-(Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)-(\d\d)\x20
           (\d\d):(\d\d):(\d\d)\x20
           GMT
           \s*\z/ix =~ date
      self.parse(date)
    elsif /\A\s*
           (?:Mon|Tue|Wed|Thu|Fri|Sat|Sun)\x20
           (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)\x20
           (\d\d|\x20\d)\x20
           (\d\d):(\d\d):(\d\d)\x20
           (\d{4})
           \s*\z/ix =~ date
      self.utc($6.to_i, MonthValue[$1.upcase], $2.to_i, $3.to_i, $4.to_i, $5.to_i)
    else
      raise ArgumentError.new("not RFC 2616 compliant date: #{date.inspect}")
    end
  end
  
  def self.xmlschema(date)
      if /\A\s*
          (-?\d+)-(\d\d)-(\d\d)
          T
          (\d\d):(\d\d):(\d\d)
          (\.\d*)?
          (Z|[+-]\d\d:\d\d)?
          \s*\z/ix =~ date
        year = $1.to_i
        mon = $2.to_i
        day = $3.to_i
        hour = $4.to_i
        min = $5.to_i
        sec = $6.to_i
        usec = 0
        usec = $7.to_f * 1000000 if $7
        if $8
          zone = $8
          year, mon, day, hour, min, sec =
            apply_offset(year, mon, day, hour, min, sec, zone_offset(zone))
          Time.utc(year, mon, day, hour, min, sec, usec)
        else
          Time.local(year, mon, day, hour, min, sec, usec)
        end
      else
        raise ArgumentError.new("invalid date: #{date.inspect}")
      end
    end
  
  # internal
  
  # private
  
  def self.gettimeofday
    Ruby.primitive :gettimeofday
  end
  
  def parts
    [@tv_sec, @tv_usec, @tz_minuteswest, @tz_dsttime]
  end
  
  def self.epoch_day(y, m, d)
    m = (m + 9) % 12
    y -= m / 10
    365 * y + y / 4 - y / 100 + y / 400 + (m * 306 + 5) / 10 + d - 1
  end
  
  def set(tv_sec = nil, tv_usec = nil, tz_minuteswest = nil, tz_dsttime = nil)
    @tv_sec = tv_sec if tv_sec
    @tv_usec = tv_usec if tv_usec
    @tz_minuteswest = tz_minuteswest if tz_minuteswest
    @tz_dsttime = tz_dsttime if tz_dsttime
    self
  end
  
  def __strftime(tv_sec, format, zone)
    Ruby.primitive :strftime
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
  
  # Parses +date+ as dateTime defined by XML Schema and converts it to a Time
  # object.  The format is restricted version of the format defined by ISO
  # 8601.
  #
  # ArgumentError is raised if +date+ is not compliant with the format or Time
  # class cannot represent specified date.
  #
  # See #xmlschema for more information on this format.
  def self.xmlschema(date)
    if /\A\s*
        (-?\d+)-(\d\d)-(\d\d)
        T
        (\d\d):(\d\d):(\d\d)
        (\.\d*)?
        (Z|[+-]\d\d:\d\d)?
        \s*\z/ix =~ date
      year = $1.to_i
      mon = $2.to_i
      day = $3.to_i
      hour = $4.to_i
      min = $5.to_i
      sec = $6.to_i
      usec = 0
      usec = $7.to_f * 1000000 if $7
      if $8
        zone = $8
        year, mon, day, hour, min, sec =
          apply_offset(year, mon, day, hour, min, sec, zone_offset(zone))
        Time.utc(year, mon, day, hour, min, sec, usec)
      else
        Time.local(year, mon, day, hour, min, sec, usec)
      end
    else
      raise ArgumentError.new("invalid date: #{date.inspect}")
    end
  end
  
  def self.zone_offset(zone, year=Time.now.year)
    off = nil
    zone = zone.upcase
    if /\A([+-])(\d\d):?(\d\d)\z/ =~ zone
      off = ($1 == '-' ? -1 : 1) * ($2.to_i * 60 + $3.to_i) * 60
    elsif /\A[+-]\d\d\z/ =~ zone
      off = zone.to_i * 3600
    elsif ZoneOffset.include?(zone)
      off = ZoneOffset[zone] * 3600
    elsif ((t = Time.local(year, 1, 1)).zone.upcase == zone rescue false)
      off = t.utc_offset
    elsif ((t = Time.local(year, 7, 1)).zone.upcase == zone rescue false)
      off = t.utc_offset
    end
    off
  end
  
  def self.zone_utc?(zone)
    # * +0000 means localtime. [RFC 2822]
    # * GMT is a localtime abbreviation in Europe/London, etc.
    if /\A(?:-00:00|-0000|-00|UTC|Z|UT)\z/i =~ zone
      true
    else
      false
    end
  end
  
  # aliases
  
  public
  
  class << self
    alias :now :new
    alias :mktime :local
    alias :utc :gm
    alias :rfc822 :rfc2822
    alias :iso8601 :xmlschema
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
