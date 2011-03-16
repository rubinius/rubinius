require 'benchmark'
require 'benchmark/ips'
require 'time'

Benchmark.ips do |x|
  iso_8601_date_only_locale_neutral = "2011-03-08"
  iso_8601_hours_minutes = "2011-03-08T10:09"
  iso_8601_hours_minutes_seconds = "2011-03-08T10:09:08"
  iso_8601_hours_minutes_seconds_secfraction = "2011-03-08T10:09:08.467"

  iso_8601_date_only_modified = "2011-Mar-08"
  iso_8601_hours_minutes_modified = "2011-Mar-08T10:09"
  iso_8601_hours_minutes_seconds_modified = "2011-Mar-08T10:09:08"
  iso_8601_hours_minutes_seconds_secfraction_modified = "2011-Mar-08T10:09:08.467"
  
  rfc2822_date = "08 Mar 11"
  rfc2822_date_time = "08 Mar 11 10:09:08 GMT"
  rfc2822_date_time_cst = "08 Mar 11 10:09:08 CST"

  format_c = "Fri Jan 02 08:10:00 -0600 2004"
  

  x.report "parse '#{iso_8601_date_only_locale_neutral}'" do |times|
    i = 0
    while i < times
      Time.parse(iso_8601_date_only_locale_neutral)
      i += 1
    end
  end

  x.report "parse '#{iso_8601_hours_minutes}'" do |times|
    i = 0
    while i < times
      Time.parse(iso_8601_hours_minutes)
      i += 1
    end
  end

  x.report "parse '#{iso_8601_hours_minutes_seconds}'" do |times|
    i = 0
    while i < times
      Time.parse(iso_8601_hours_minutes_seconds)
      i += 1
    end
  end

  x.report "parse '#{iso_8601_hours_minutes_seconds_secfraction}'" do |times|
    i = 0
    while i < times
      Time.parse(iso_8601_hours_minutes_seconds_secfraction)
      i += 1
    end
  end

  x.report "parse '#{iso_8601_hours_minutes_modified}'" do |times|
    i = 0
    while i < times
      Time.parse(iso_8601_hours_minutes_modified)
      i += 1
    end
  end

  x.report "parse '#{iso_8601_hours_minutes_seconds_modified}'" do |times|
    i = 0
    while i < times
      Time.parse(iso_8601_hours_minutes_seconds_modified)
      i += 1
    end
  end

  x.report "parse '#{iso_8601_hours_minutes_seconds_secfraction_modified}'" do |times|
    i = 0
    while i < times
      Time.parse(iso_8601_hours_minutes_seconds_secfraction_modified)
      i += 1
    end
  end

  x.report "parse '#{rfc2822_date}'" do |times|
    i = 0
    while i < times
      Time.parse(rfc2822_date)
      i += 1
    end
  end

  x.report "parse '#{rfc2822_date_time}'" do |times|
    i = 0
    while i < times
      Time.parse(rfc2822_date_time)
      i += 1
    end
  end

  x.report "parse '#{rfc2822_date_time_cst}'" do |times|
    i = 0
    while i < times
      Time.parse(rfc2822_date_time_cst)
      i += 1
    end
  end

  x.report "parse '#{format_c}'" do |times|
    i = 0
    while i < times
      Time.parse(format_c)
      i += 1
    end
  end

end
