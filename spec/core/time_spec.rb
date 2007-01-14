require File.dirname(__FILE__) + '/../spec_helper'

# class methods
# _load, apply_offset, at, gm, httpdate, local, make_time, mktime,
# month_days, new, now, parse, rfc2822, times, utc, xmlschema,
# zone_offset, zone_utc?

# +, -, <=>, _dump, asctime, ctime, day, dst?, eql?, getgm, getlocal,
# getutc, gmt?, gmt_offset, gmtime, gmtoff, hash, hour, httpdate,
# inspect, isdst, iso8601, localtime, marshal_dump, marshal_load,
# mday, min, mon, month, rfc2822, rfc822, sec, strftime, succ, to_a,
# to_f, to_i, to_s, tv_sec, tv_usec, usec, utc, utc?, utc_offset,
# wday, xmlschema, yday, year, zone

context "Time class method" do
  specify "at should convert to time object" do
    example do
      Time.at( 1168475924 ).inspect
    end.should == 'Thu Jan 11 00:38:44 +0000 2007'
  end
end

context "Time instance method" do
end
