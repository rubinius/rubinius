require 'benchmark'
require 'benchmark/ips'
require 'date'

Benchmark.ips do |x|
  iso_8601_date_only_locale_neutral = "2011-03-08"
  iso_8601_date_only_modified = "2011-Mar-08"
  rfc2822_date = "08 Mar 11"

  format_c = "Fri Jan 02 2004"
  format_d = "Fri, 02 Jan 2004"
  format_e = "2011/03/08"

  x.report "parse '#{iso_8601_date_only_locale_neutral}'" do |times|
    i = 0
    while i < times
      Date.parse(iso_8601_date_only_locale_neutral)
      i += 1
    end
  end

  x.report "parse '#{iso_8601_date_only_modified}'" do |times|
    i = 0
    while i < times
      Date.parse(iso_8601_date_only_modified)
      i += 1
    end
  end

  x.report "parse '#{rfc2822_date}'" do |times|
    i = 0
    while i < times
      Date.parse(rfc2822_date)
      i += 1
    end
  end

  x.report "parse '#{format_c}'" do |times|
    i = 0
    while i < times
      Date.parse(format_c)
      i += 1
    end
  end

  x.report "parse '#{format_d}'" do |times|
    i = 0
    while i < times
      Date.parse(format_d)
      i += 1
    end
  end

  x.report "parse '#{format_e}'" do |times|
    i = 0
    while i < times
      Date.parse(format_e)
      i += 1
    end
  end

end
