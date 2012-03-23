# -*- encoding: us-ascii -*-

class Time
  include Comparable


  MonthValue = {
    'JAN' => 1, 'FEB' => 2, 'MAR' => 3, 'APR' => 4, 'MAY' => 5, 'JUN' => 6,
    'JUL' => 7, 'AUG' => 8, 'SEP' => 9, 'OCT' =>10, 'NOV' =>11, 'DEC' =>12
  }

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

    if (year & 0xffff) != year || year < 1900 then
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

  def self.local(p1, p2=1, p3=1, p4=0, p5=0, p6=0, p7=0,
                 yday=undefined, isdst=undefined, tz=undefined)
    if !tz.equal?(undefined)
      sec    = p1.kind_of?(String) ? p1.to_i : Rubinius::Type.num2long(p1)
      minute = p2.kind_of?(String) ? p2.to_i : Rubinius::Type.num2long(p2)
      hour   = p3.kind_of?(String) ? p3.to_i : Rubinius::Type.num2long(p3)
      day    = p4.kind_of?(String) ? p4.to_i : Rubinius::Type.num2long(p4)
      month  = p5.kind_of?(String) ? p5.to_i : Rubinius::Type.num2long(p5)
      year   = p6.kind_of?(String) ? p6.to_i : Rubinius::Type.num2long(p6)
      nsec   = 0
      isdst  = isdst ? 1 : 0
    else
      unless isdst.equal?(undefined)
        raise ArgumentError, "wrong number of arguments (9 for 1..8)"
      end

      # resolve month names to numbers
      month = p2
      if month
        if month.kind_of?(String) or month.respond_to?(:to_str)
          month = StringValue(month)

          month = MonthValue[month.upcase] || month.to_i

          raise ArgumentError, "month argument out of range" unless month
        else
          month = Rubinius::Type.num2long(month)
        end
      else
        month = 1
      end

      year   = p1.kind_of?(String) ? p1.to_i : Rubinius::Type.num2long(p1)
      day    = p3.kind_of?(String) ? p3.to_i : Rubinius::Type.num2long(p3)
      hour   = p4.kind_of?(String) ? p4.to_i : Rubinius::Type.num2long(p4)
      minute = p5.kind_of?(String) ? p5.to_i : Rubinius::Type.num2long(p5)

      sec  = p6
      usec = p7
      nsec = nil

      if usec.kind_of?(String)
        nsec = usec.to_i * 1000
      elsif usec
        nsec = (usec * 1000).to_i
      end

      isdst =  -1
    end

    # This logic is taken from MRI, on how to deal with 2 digit dates.
    if year < 200
      if 0 <= year and year < 39
        warn "2 digit year used: #{year}" if $VERBOSE
        year += 2000
      elsif 69 <= year and year < 139
        warn "2 or 3 digit year used: #{year}" if $VERBOSE
        year += 1900
      end
    end

    from_array(sec, minute, hour, day, month, year, nsec, isdst, false)
  end

  def self.gm(p1, p2=1, p3=1, p4=0, p5=0, p6=0, p7=0,
              yday=undefined, isdst=undefined, tz=undefined)
    if !tz.equal?(undefined)
      sec    = p1.kind_of?(String) ? p1.to_i : Rubinius::Type.num2long(p1)
      minute = p2.kind_of?(String) ? p2.to_i : Rubinius::Type.num2long(p2)
      hour   = p3.kind_of?(String) ? p3.to_i : Rubinius::Type.num2long(p3)
      day    = p4.kind_of?(String) ? p4.to_i : Rubinius::Type.num2long(p4)
      month  = p5.kind_of?(String) ? p5.to_i : Rubinius::Type.num2long(p5)
      year   = p6.kind_of?(String) ? p6.to_i : Rubinius::Type.num2long(p6)
      nsec   = 0
    else
      unless isdst.equal?(undefined)
        raise ArgumentError, "wrong number of arguments (9 for 1..8)"
      end

      # resolve month names to numbers
      month = p2
      if month
        if month.kind_of?(String) or month.respond_to?(:to_str)
          month = StringValue(month)

          month = MonthValue[month.upcase] || month.to_i

          raise ArgumentError, "month argument out of range" unless month
        else
          month = Rubinius::Type.num2long(month) rescue month.to_i
        end
      else
        month = 1
      end

      year   = p1.kind_of?(String) ? p1.to_i : Rubinius::Type.num2long(p1)
      day    = p3.kind_of?(String) ? p3.to_i : Rubinius::Type.num2long(p3)
      hour   = p4.kind_of?(String) ? p4.to_i : Rubinius::Type.num2long(p4)
      minute = p5.kind_of?(String) ? p5.to_i : Rubinius::Type.num2long(p5)

      sec  = p6
      usec = p7
      nsec = nil

      if usec.kind_of?(String)
        nsec = usec.to_i * 1000
      elsif usec
        nsec = (usec * 1000).to_i
      end
    end

    # This logic is taken from MRI, on how to deal with 2 digit dates.
    if year < 200
      if 0 <= year and year < 39
        warn "2 digit year used: #{year}" if $VERBOSE
        year += 2000
      elsif 69 <= year and year < 139
        warn "2 or 3 digit year used: #{year}" if $VERBOSE
        year += 1900
      end
    end

    from_array(sec, minute, hour, day, month, year, nsec, -1, true)
  end

  def self.times
    Process.times
  end

  def succ
    self + 1
  end

  def asctime
    strftime("%a %b %e %H:%M:%S %Y")
  end

  def to_a
    decomposed(@is_gmt)
  end

  def sec
    to_a[0]
  end

  def min
    to_a[1]
  end

  def hour
    to_a[2]
  end

  def day
    to_a[3]
  end

  def mon
    to_a[4]
  end

  def year
    to_a[5]
  end

  def wday
    to_a[6]
  end

  def yday
    to_a[7]
  end

  def dst?
    to_a[8]
  end

  def zone
    to_a[9]
  end

  def gmt?
    @is_gmt
  end

  alias_method :to_i, :seconds

  def to_f
    seconds + (usec * 0.000001)
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

  def getlocal
    dup.localtime
  end

  def getgm
    dup.gmtime
  end

  def hash
    seconds ^ usec
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
  alias_method :tv_sec,     :seconds
  alias_method :tv_usec,    :usec
  alias_method :utc,        :gmtime
  alias_method :isdst,      :dst?
  alias_method :utc_offset, :gmt_offset
  alias_method :gmtoff,     :gmt_offset
  alias_method :getutc,     :getgm
end
