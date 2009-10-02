##
# A future replacement for Time.  Not in use currently.

class Moment

  def self.now
    Moment.new(nil)
  end

  def self.at(time)
    new(time)
  end

  def initialize(time)
    if time
      @utc   = time.respond_to?(:first) ? time.first : time
      @micro = time.respond_to?(:last)  ? time.last  : 0
    else
      tv = FFI::Platform::POSIX::TimeVal.new

      if 0 != FFI::Platform::POSIX.gettimeofday(tv.pointer, nil)
        Errno.handle
      end

      @utc = tv[:tv_sec]
      @micro = tv[:tv_usec]

      tv.free
    end

    @local = @utc - FFI::Platform::POSIX.timezone
    @time  = @local
    @human = nil
  end

  def offset
    @local - @utc
  end

  class HumanTime
    def initialize(moment)
      @moment = moment
      # HACK. the 0 should be 1 if this Moment is during DST
      @timezone = FFI::Platform::POSIX.tzname(0)
    end

    attr_accessor :micro
    attr_accessor :second
    attr_accessor :minute
    attr_accessor :hour

    Days = {
      0 => :sunday,
      1 => :monday,
      2 => :tuesday,
      3 => :wednesday,
      4 => :thursday,
      5 => :friday,
      6 => :saturday
    }

    def from_jd(j)
      @jd = j

      # See http://www.hermetic.ch/cal_stud/jdn.htm
      l = j + 68569
      n = (4*l) / 146097
      l = l - (146097*n+3) / 4
      i = 4000 * (l+1) / 1461001
      l = l - 1461 * i / 4  + 31
      j = 80 * l / 2447
      k = l-2447*j/80
      l = j / 11
      j = j + 2 - 12 * l
      i = 100 * (n - 49) + i + l

      @year = i
      @month = j
      @day = k

      # HACK this only does gregorian weeks
      @weekday = Days[(@jd + 1) % 7]
    end

    def leap?
      @year % 4 == 0 && @year % 100 != 0 || @year % 400 == 0
    end

    # HACK these need to be locale specific tables
    ShortWeekday = {
      :monday => "Mon",
      :tuesday => "Tue",
      :wednesday => "Wed",
      :thursday => "Thu",
      :friday => "Fri",
      :saturday => "Sat",
      :sunday => "Sun"
    }

    def short_weekday
      ShortWeekday[@weekday]
    end

    ShortMonth = {
      1 => "Jan",
      2 => "Feb",
      3 => "Mar",
      4 => "Apr",
      5 => "May",
      6 => "Jun",
      7 => "Jul",
      8 => "Aug",
      9 => "Sep",
      10 => "Oct",
      11 => "Nov",
      12 => "Dec"
    }

    def short_month
      ShortMonth[@month]
    end

    def padded_day
      if @day < 10
        "0#{@day}"
      else
        @day.to_s
      end
    end

    def padded_hour_24
      if @hour < 10
        "0#{@hour}"
      else
        @hour.to_s
      end
    end

    def padded_minute
      if @minute < 10
        "0#{@minute}"
      else
        @minute.to_s
      end
    end

    def padded_second
      if @second < 10
        "0#{@second}"
      else
        @second.to_s
      end
    end

    def year
      @year.to_s
    end

    def time_zone
      @timezone
    end

    def time_offset
      offset = @moment.offset / 3600

      if offset.abs < 10
        if offset < 0
          "-0#{offset.abs}00"
        else
          "+0#{offset}00"
        end
      else
        "#{offset}00"
      end
    end
  end

  EpochToMDJ = 40587

  def to_human
    # Cache it.
    return @human if @human

    s = @time % 86400
    h = HumanTime.new(self)

    h.micro  = @micro
    h.second = (s % 60); s /= 60
    h.minute = (s % 60); s /= 60
    h.hour   = s

    u = @time / 86400

    @mjd = EpochToMDJ + u

    # The julian day began an noon, not midnight.
    jd = @mjd + 2400001

    h.from_jd(jd)

    @human = h
    return h
  end

  FormatMethod = {
    :A => :weekday,
    :a => :short_weekday,
    :B => :month,
    :b => :short_month,
    :C => :short_year,
    :c => :locale_datetime,
    :D => :mdy,
    :d => :padded_day,
    :e => :day,
    :F => :ymd,
    :g => :padded_short_commercial_year,
    :G => :commercial_year,
    :H => :padded_hour_24,
    :h => :short_month,
    :I => :padded_hour,
    :j => :day_of_year,
    :k => :bpadded_hour_24,
    :l => :bpadded_hour,
    :M => :padded_minute,
    :m => :padded_month,
    :n => :newline,
    :p => :am_pm,
    :R => :hour_minute,
    :r => :hms_ap,
    :S => :padded_second,
    :s => :epoch,
    :T => :hms,
    :t => :tab,
    :U => :week_from_sunday,
    :u => :weekday,
    :V => :week_from_monday,
    :v => :unsupported,
    :W => :unsupported,
    :X => :locale_time,
    :x => :locale_date,
    :Y => :year,
    :y => :padded_short_year,
    :Z => :time_zone,
    :z => :time_offset,
    :'+' => :locale_dt2
  }

  def format(str)
    h = to_human()
    pattern = /%([#{FormatMethod.keys.join}])/o
    str.gsub(pattern) do |which|
      h.__send__ FormatMethod[$1.to_sym]
    end
  end

  def to_s
    if offset != 0
      format "%a %b %d %H:%M:%S %Z %Y"
    else
      format "%a %b %d %H:%M:%S UTC %Y"
    end
  end

  alias_method :inspect, :to_s
end
