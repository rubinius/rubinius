class Time
  include Comparable


  MonthValue = {
    'JAN' => 1, 'FEB' => 2, 'MAR' => 3, 'APR' => 4, 'MAY' => 5, 'JUN' => 6,
    'JUL' => 7, 'AUG' => 8, 'SEP' => 9, 'OCT' =>10, 'NOV' =>11, 'DEC' =>12
  }

  def self.at(sec, usec=nil)
    return sec.dup if sec.kind_of? Time

    if sec.kind_of?(Integer) || usec
      sec  = Type.coerce_to sec, Integer, :to_i
      usec = usec ? usec.to_i : 0

      sec  = sec + (usec / 1000000)
      usec = usec % 1000000
    else
      float = FloatValue(sec)
      if float < 0
        pos_sec, usec_frac = float.divmod(-1)
        sec = -pos_sec

        if usec_frac < 0
          usec_frac += 1
          sec -= 1
        end
      else
        sec, usec_frac = float.divmod(1)
      end

      usec = (usec_frac * 1_000_000 + 0.5).to_i
    end

    return specific(sec, usec, false)
  end

  #--
  # TODO: doesn't load ivars
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
  # TODO: doesn't dump ivars
  #++

  def _dump(limit = nil)
    tm = decomposed(true)

    if (year & 0xffff) != year then
      raise ArgumentError, "year too big to marshal: #{year}"
    end

    gmt = @is_gmt ? 1 : 0

    major =  1             << 31 | # 1 bit
             gmt           << 30 | # 1 bit
            (tm[5] - 1900) << 14 | # 16 bits
            (tm[4] - 1)    << 10 | # 4 bits
             tm[3]         <<  5 | # 5 bits
             tm[2]                 # 5 bits
    minor =  tm[1]   << 26 | # 6 bits
             tm[0]   << 20 | # 6 bits
             usec # 20 bits

    [major, minor].pack 'VV'
  end

  def self.local(first, *args)
    if args.size == 9
      second = Integer(first)
      minute = Integer(args[0])
      hour =   Integer(args[1])
      day =    Integer(args[2])
      month =  Integer(args[3])
      year =   Integer(args[4])
      usec =   0
      isdst =  args[7] ? 1 : 0
    else
      # resolve month names to numbers
      month = args[0]
      if month
        if month.kind_of?(String) or month.respond_to?(:to_str)
          month = StringValue(month)

          month = MonthValue[month.upcase] || month.to_i

          raise ArgumentError, "month argument out of range" unless month
        else
          month = Integer(month)
        end
      else
        month = 1
      end

      year =   Integer(first)
      day =    Integer(args[1] || 1)
      hour =   Integer(args[2] || 0)
      minute = Integer(args[3] || 0)
      second = Integer(args[4] || 0)
      usec =   Integer(args[5] || 0)
      isdst =  -1
    end

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

    Time.from_array(second, minute, hour, day, month, year, usec, isdst, false)
  end

  def self.gm(first, *args)
    if args.size == 9
      second = Integer(first)
      minute = Integer(args[0])
      hour =   Integer(args[1])
      day =    Integer(args[2])
      month =  Integer(args[3])
      year =   Integer(args[4])
      usec =   0
    else
      # resolve month names to numbers
      month = args[0]
      if month
        if month.kind_of?(String) or month.respond_to?(:to_str)
          month = StringValue(month)

          month = MonthValue[month.upcase] || month.to_i

          raise ArgumentError, "month argument out of range" unless month
        else
          month = Integer(month)
        end
      else
        month = 1
      end

      year =   Integer(first)
      day =    Integer(args[1] || 1)
      hour =   Integer(args[2] || 0)
      minute = Integer(args[3] || 0)
      second = Integer(args[4] || 0)
      usec =   Integer(args[5] || 0)
    end

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

    Time.from_array(second, minute, hour, day, month, year, usec, -1, true)
  end

  def self.times
    Process.times
  end

  def inspect
    if @is_gmt
      strftime("%a %b %d %H:%M:%S UTC %Y")
    else
      strftime("%a %b %d %H:%M:%S %z %Y")
    end
  end

  def +(arg)
    raise TypeError, 'time + time?' if arg.kind_of?(Time)

    if arg.kind_of?(Integer)
      other_sec = arg
      other_usec = 0
    else
      other_sec, usec_frac = FloatValue(arg).divmod(1)
      other_usec = (usec_frac * 1_000_000 + 0.5).to_i
    end

    Time.specific(seconds + other_sec, usec + other_usec, @is_gmt)
  end

  def -(other)
    case other
    when Time
      (seconds - other.seconds) + ((usec - other.usec) * 0.000001)
    when Integer
      Time.specific(seconds - other, usec, @is_gmt)
    else
      other = FloatValue(other)

      other_sec, usec_frac = FloatValue(other).divmod(1)
      other_usec = (usec_frac * 1_000_000 + 0.5).to_i

      Time.specific(seconds - other_sec, usec - other_usec, @is_gmt)
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
    return false unless other.kind_of?(Time)
    seconds == other.seconds and usec == other.usec
  end

  def asctime
    strftime("%a %b %e %H:%M:%S %Y")
  end

  def hour
    decomposed(@is_gmt)[2]
  end

  def min
    decomposed(@is_gmt)[1]
  end

  def sec
    decomposed(@is_gmt)[0]
  end

  def day
    decomposed(@is_gmt)[3]
  end

  def year
    decomposed(@is_gmt)[5]
  end

  def yday
    decomposed(@is_gmt)[7]
  end

  def wday
    decomposed(@is_gmt)[6]
  end

  def zone
    strftime("%Z")
  end

  def mon
    decomposed(@is_gmt)[4]
  end

  def gmt?
    @is_gmt
  end

  alias_method :to_i, :seconds

  def to_f
    seconds + (usec / 1000000.0)
  end

  ##
  # Returns:
  #   [ sec, min, hour, day, month, year, wday, yday, isdst, zone ]

  def to_a
    decomposed(@is_gmt)
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
      @decomposed = nil
    end

    self
  end

  def gmtime
    unless @is_gmt
      @is_gmt = true
      @decomposed = nil
    end

    self
  end

  def dst?
    decomposed(@is_gmt)[8]
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

    return Time.from_array(sec, min, hour, mday, mon, year, usec, isdst, from_gmt)
  end


  class << self
    alias_method :new,    :now
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
