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
    raise TypeError, 'marshaled time format differ' unless data.bytesize == 8

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
    tm = getgm.to_a

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

  def self.at(sec, usec=undefined)
    if !undefined.equal?(usec)
      sec  = Rubinius::Type.coerce_to(sec,  Integer, :to_int)
      usec = Rubinius::Type.coerce_to(usec, Integer, :to_int)

      nsec = (usec * 1000).to_i
      sec = sec + (nsec / 1000000000)
      nsec = nsec % 1000000000

      specific(sec, nsec, false, nil)
    elsif sec.kind_of?(Time)
      duplicate(sec)
    elsif sec.kind_of?(Integer)
      specific(sec, 0, false, nil)
    elsif sec.kind_of?(String)
      raise TypeError, "can't convert #{sec} into an exact number"
    else
      float = FloatValue(sec)
      sec       = float.to_i
      nsec_frac = float % 1.0

      if float < 0 && nsec_frac > 0
        sec -= 1
      end

      nsec = (nsec_frac * 1_000_000_000 + 0.5).to_i
      specific(sec, nsec, false, nil)
    end
  end

  def self.compose(offset, p1, p2=nil, p3=nil, p4=nil, p5=nil, p6=nil, p7=nil,
                   yday=undefined, is_dst=undefined, tz=undefined)
    if undefined.equal?(tz)
      unless undefined.equal?(is_dst)
        raise ArgumentError, "wrong number of arguments (9 for 1..8)"
      end

      y, m, d, hr, min, sec, usec = p1, p2, p3, p4, p5, p6, p7
      is_dst = -1
    else
      y, m, d, hr, min, sec, usec = p6, p5, p4, p3, p2, p1, 0
      is_dst = is_dst ? 1 : 0
    end

    if m.kind_of?(String) or m.respond_to?(:to_str)
      m = StringValue(m)
      m = MonthValue[m.upcase] || m.to_i

      raise ArgumentError, "month argument out of range" unless m
    else
      m = Rubinius::Type.coerce_to(m || 1, Integer, :to_int)
    end

    y   = y.kind_of?(String)   ? y.to_i   : Rubinius::Type.coerce_to(y,        Integer, :to_int)
    d   = d.kind_of?(String)   ? d.to_i   : Rubinius::Type.coerce_to(d   || 1, Integer, :to_int)
    hr  = hr.kind_of?(String)  ? hr.to_i  : Rubinius::Type.coerce_to(hr  || 0, Integer, :to_int)
    min = min.kind_of?(String) ? min.to_i : Rubinius::Type.coerce_to(min || 0, Integer, :to_int)

    nsec = nil

    if usec.kind_of?(String)
      nsec = usec.to_i * 1000
    elsif usec
      nsec = (usec * 1000).to_i
    end

    y = compose_deal_with_year(y)

    case offset
      when :utc
        is_dst = -1
        is_utc = true
        offset = nil
      when :local
        is_utc = false
        offset = nil
      else
        is_dst = -1
        is_utc = false
    end

    from_array(sec, min, hr, d, m, y, nsec, is_dst, is_utc, offset)
  end

  def self.from_array(sec, min, hour, mday, month, year, nsec, is_dst, from_gmt, utc_offset)
    Rubinius.primitive :time_s_from_array

    sec = sec.kind_of?(String) ? sec.to_i : Rubinius::Type.coerce_to(sec || 0, Integer, :to_int)

    if utc_offset
      utc_offset_sec = utc_offset.to_i
      utc_offset_nsec = ((utc_offset % 1.0) * 1_000_000_000 + 0.5).to_i
    else
      utc_offset_sec = 0
      utc_offset_nsec = 0
    end

    from_array(sec, min, hour, mday, month, year, nsec || 0, is_dst, from_gmt, utc_offset, utc_offset_sec, utc_offset_nsec)
  end

  def self.local(*args)
    compose(:local, *args)
  end

  def self.gm(*args)
    compose(:utc, *args)
  end

  class << self
    alias_method :new, :now
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

  alias_method :to_s, :inspect

  def +(arg)
    raise TypeError, 'time + time?' if arg.kind_of?(Time)

    if arg.kind_of?(Integer)
      other_sec = arg
      other_usec = 0
    else
      other_sec, usec_frac = FloatValue(arg).divmod(1)
      other_usec = (usec_frac * 1_000_000 + 0.5).to_i
    end

    # Don't use self.class, MRI doesn't honor subclasses here
    Time.specific(seconds + other_sec, (usec + other_usec) * 1000, @is_gmt, @offset)
  end

  def -(other)
    if other.kind_of?(Time)
      return (seconds - other.seconds) + ((usec - other.usec) * 0.000001)
    end

    case other
    when Integer
      other_sec = other
      other_usec = 0
    else
      other_sec, usec_frac = FloatValue(other).divmod(1)
      other_usec = (usec_frac * 1_000_000 + 0.5).to_i
    end

    # Don't use self.class, MRI doesn't honor subclasses here
    Time.specific(seconds - other_sec, (usec - other_usec) * 1000, @is_gmt, @offset)
  end

  def localtime
    if @is_gmt
      @is_gmt = false
      @decomposed = nil
    end

    self
  end

  def getlocal
    dup.localtime
  end

  def eql?(other)
    other.kind_of?(Time) and seconds == other.seconds and usec == other.usec
  end

  def <=>(other)
    if other.kind_of? Time
      (seconds <=> other.seconds).nonzero? or (usec <=> other.usec)
    else
      nil
    end
  end

  def to_f
    seconds + (usec * 0.000001)
  end

  class << self
    def compose_deal_with_year(year)
      # This logic is taken from MRI, on how to deal with 2 digit dates.

      return year if year >= 139

      case year
      when 0...39
        warn "2 digit year used: #{year}" if $VERBOSE
        year + 2000
      when 69...139
        warn "2 or 3 digit year used: #{year}" if $VERBOSE
        year + 1900
      else
        year
      end
    end
    private :compose_deal_with_year
  end

  def succ
    self + 1
  end

  def asctime
    strftime("%a %b %e %H:%M:%S %Y")
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

  def gmt_offset
    Rubinius.primitive :time_utc_offset
    raise PrimitiveFailure, "Time#gmt_offset primitive failed"
  end

  def gmtime
    unless @is_gmt
      @is_gmt = true
      @offset = nil
      @decomposed = nil
    end

    self
  end

  def getgm
    dup.gmtime
  end

  def hash
    seconds ^ usec
  end

  class << self
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
