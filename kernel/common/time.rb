#--
# _load
# parse
# --------------------
# _dump
# marshal_dump
# marshal_load
#++

class Time
  include Comparable

  ZoneOffset =      { 'UTC' => 0, 'Z' => 0,  'UT' => 0, 'GMT' => 0,
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

  TM_FIELDS = {
    :sec => 0,
    :min => 1,
    :hour => 2,
    :mday => 3,
    :mon => 4,
    :year => 5,
    :wday => 6,
    :yday => 7,
    :isdst => 8,
  }

  TIMEVAL_FIELDS = {
    :sec => 0,
    :usec => 1,
  }

  def initialize
    gettimeofday
  end

  private :initialize

  #--
  # TODO: doesn't load nsec or ivars
  #++

  def self._load(data)
    raise TypeError, 'marshaled time format differ' unless data.length == 8

    major, minor = data.unpack 'VV'

    if (major & (1 << 31)) == 0 then
      at major, minor
    else
      major &= ~(1 << 31)

      is_gmt =  (major >> 30) & 0x1
      year   = ((major >> 14) & 0xffff) + 1900
      mon    = ((major >> 10) & 0xf) + 1
      mday   =  (major >>  5) & 0x1f
      hour   =  major         & 0x1f

      min   =  (minor >> 26) & 0x3f
      sec   =  (minor >> 20) & 0x3f
      isdst = false

      usec = minor & 0xfffff

      time = gm year, mon, mday, hour, min, sec, usec
      time.localtime if is_gmt.zero?
      time
    end
  end

  #--
  # TODO: doesn't dump nsec or ivars
  #++

  def _dump(limit = nil)
    build_tm

    tm = @tm
    is_gmt = @is_gmt

    unless is_gmt
      time_switch true
    end

    year = @tm[TM_FIELDS[:year]]

    if (year & 0xffff) != year then
      raise ArgumentError, "year too big to marshal: #{year}"
    end

    gmt = @is_gmt ? 1 : 0

    major = 1                     << 31 | # 1 bit
            (@is_gmt ? 1 : 0)     << 30 | # 1 bit
            @tm[TM_FIELDS[:year]] << 14 | # 16 bits
            @tm[TM_FIELDS[:mon]]  << 10 | # 4 bits
            @tm[TM_FIELDS[:mday]] <<  5 | # 5 bits
            @tm[TM_FIELDS[:hour]]         # 5 bits
    minor = @tm[TM_FIELDS[:min]]  << 26 | # 6 bits
            @tm[TM_FIELDS[:sec]]  << 20 | # 6 bits
            @usec # 20 bits

    [major, minor].pack 'VV'
  ensure
    @tm = tm
    @is_gmt = is_gmt
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
      if args[0] && (args[0].kind_of?(String) || args[0].respond_to?(:to_str))
        args[0] = args[0].to_str if args[0].respond_to?(:to_str)
        month = MonthValue[args[0].upcase] || args[0].to_i || raise(ArgumentError.new('argument out of range'))
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
    t
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
      if args[0] && args[0].respond_to?(:to_str) && (args[0] = args[0].to_str).to_i == 0
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
    t
  end

  def self.at(secs_or_time, msecs = nil)
    if secs_or_time.kind_of? Time
      return secs_or_time.dup
    end

    Time.allocate.at_gmt(secs_or_time, msecs, false)
  end

  def strftime(format)
    build_tm

    __strftime__(@tm, format.to_str)
  end

  def inspect
    if @is_gmt
      strftime("%a %b %d %H:%M:%S UTC %Y")
    else
      strftime("%a %b %d %H:%M:%S %z %Y")
    end
  end

  def seconds
    @sec
  end

  def +(other)
    raise TypeError, 'time + time?' if other.kind_of?(Time)
    other      = FloatValue(other) unless other.is_a?(Integer)
    other_usec = 0
    if other.kind_of?(Float) 
      other_usec = (other % 1) * 1000000
      other      = other.to_i
    end
    dup.at_gmt(seconds + other, usec + other_usec, @is_gmt)
  end

  def -(other)
    if other.kind_of? Time
      seconds - other.seconds + (usec - other.usec) * 0.000001
    else
      other      = FloatValue(other) unless other.is_a?(Integer)
      other_usec = 0
      if other.kind_of?(Float) 
        other_usec = (1 - other % 1) * 1000000
        other      = other.ceil.to_i
      end
      dup.at_gmt(seconds - other, usec + other_usec, @is_gmt)
    end
  end

  def succ
    self + 1
  end

  def <=>(other)
    if other.kind_of? Time
      [self.seconds, self.usec] <=> [other.seconds, other.usec]
    else
      nil
    end
  end

  def eql?(other)
    (self <=> other) == 0
  end

  def asctime
    strftime("%a %b %e %H:%M:%S %Y")
  end

  def hour
    build_tm

    @tm[2]
  end

  def min
    build_tm

    @tm[1]
  end

  def sec
    build_tm

    @tm[0]
  end

  def day
    build_tm

    @tm[3]
  end

  def year
    build_tm

    @tm[5] + 1900
  end

  def yday
    build_tm

    @tm[7] + 1
  end

  def wday
    build_tm

    @tm[6]
  end

  def zone
    strftime("%Z")
  end

  def mon
    build_tm

    @tm[4] + 1
  end

  def gmt?
    @is_gmt
  end

  def usec
    @usec
  end

  def to_i
    self.seconds
  end

  def to_f
    seconds + (usec / 1000000.0)
  end

  ##
  # Returns:
  #   [ sec, min, hour, day, month, year, wday, yday, isdst, zone ]

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
    if @is_gmt
      @is_gmt = false
      @tm = nil
    end

    self
  end

  def gmtime
    unless @is_gmt
      @is_gmt = true
      @tm = nil
    end

    self
  end

  def dst?
    build_tm

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

  def mktime(sec, min, hour, mday, mon, year, usec, isdst, from_gmt)
    sec  = sec.to_i
    min  = min.to_i
    hour = hour.to_i
    mday = mday.to_i
    mon  = mon.to_i
    year = year.to_i
    usec = usec.to_i

    # This logic is taken from MRI, on how to deal with 2 digit dates.
    if year < 200
      if 0 <= year and year < 39
        warn "2 digit year used: #{year}"
        year += 2000
      elsif 69 <= year and year < 139
        warn "2 or 3 digit year used: #{year}"
        year += 1900
      end
    end

    @sec, @usec = time_mktime(sec, min, hour, mday, mon, year, usec, isdst, from_gmt)
    @is_gmt = from_gmt

    raise ArgumentError, "time out of range" if @sec == -1

    self
  end

  ##
  # +sec+ and +usec+ are always given in gmt here.
  #
  # +want_gmt+ says whether the caller wants a gmtime or local time object.

  def at_gmt(sec, usec, want_gmt)
    if sec.kind_of?(Integer) || usec
      sec  = Type.coerce_to sec, Integer, :to_i
      usec = usec ? usec.to_i : 0
    else
      sec  = Type.coerce_to sec, Float, :to_f
      usec = ((sec % 1) * 1000000).to_i
      sec  = sec.to_i
    end

    sec  = sec + (usec / 1000000)
    usec = usec % 1000000

    @sec = sec
    @usec = usec
    @is_gmt = want_gmt
    @tm = nil

    self
  end

  ##
  # Ensures @tm reflects the values in @sec/@usec and @is_gmt
  def build_tm
    if @tm.nil?
      time_switch @is_gmt
    end
  end

  private :build_tm

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

  class << self
    alias_method :now,    :new
    alias_method :mktime, :local
    alias_method :utc,    :gm
  end

  alias_method :utc?,       :gmt?
  alias_method :month,      :mon
  alias_method :ctime,      :asctime
  alias_method :mday,       :day
  alias_method :to_i,       :seconds
  alias_method :to_s,       :inspect
  alias_method :tv_sec,     :seconds
  alias_method :tv_usec,    :usec
  alias_method :utc,        :gmtime
  alias_method :isdst,      :dst?
  alias_method :utc_offset, :gmt_offset
  alias_method :gmtoff,     :gmt_offset
  alias_method :getutc,     :getgm
end
