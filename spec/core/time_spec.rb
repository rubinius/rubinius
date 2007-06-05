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
  
  def localtime(seconds)
    if RUBY_PLATFORM =~ /darwin/
      `date -r #{seconds} +'%a %b %d %H:%M:%S %z %Y'`.chomp
    elsif RUBY_PLATFORM =~ /linux/
      `date -d @#{seconds} +'%a %b %d %H:%M:%S %z %Y'`.chomp
    else
      `date -j -f "%s" #{seconds} "+%a %b %d %H:%M:%S %z %Y"`.chomp
    end
  end
  
  def utctime(seconds)
    if RUBY_PLATFORM =~ /darwin/
      `date -u -r #{seconds} +'%a %b %d %H:%M:%S %z %Y'`.chomp
    elsif RUBY_PLATFORM =~ /linux/
      `date -u -d @#{seconds} +'%a %b %d %H:%M:%S %z %Y'`.chomp
    else
      `date -u -j -f "%s" #{seconds} "+%a %b %d %H:%M:%S %z %Y"`.chomp
    end
  end

  def with_timezone(zone)
    old = ENV["TZ"]
    ENV["TZ"] = zone

    yield

    ENV["TZ"] = old
  end
  
  # at
  
  specify "at should convert to time object" do
    Time.at( 1168475924 ).inspect.should == localtime(1168475924)
  end
  
  specify "at should create a new time object with the value given by time" do
    Time.at(Time.now).inspect.should == Time.now.inspect
  end
  
  specify "at should create a dup time object with the value given by time" do
    t1 = Time.new
    t2 = Time.at(t1)
    t1.object_id.should_not == t2.object_id
  end
  
  # gm/utc
  
  specify "gm should create a time based on given values, interpreted as UTC (GMT)" do
    Time.gm(2000,"jan",1,20,15,1).inspect.should == utctime(946757701)
  end
  
  specify "gm should create a time based on given C-style gmtime arguments, interpreted as UTC (GMT)" do
    Time.gm(1, 15, 20, 1, 1, 2000, :ignored, :ignored, :ignored, :ignored).inspect.should == utctime(946757701)
  end
  
  specify "utc is a synonym for Time.gm" do
    Time.utc(2000,"jan",1,20,15,1).inspect.should == Time.gm(2000,"jan",1,20,15,1).inspect
  end
  
  #local/mktime
  
  specify "local should create a time based on given values, interpreted in the local time zone" do
    Time.local(2000,"jan",1,20,15,1).inspect.should == localtime(946786501)
  end
  
  specify "local should create a time based on given C-style gmtime arguments, interpreted in the local time zone" do
    Time.local(1, 15, 20, 1, 1, 2000, :ignored, :ignored, :ignored, :ignored).inspect.should == localtime(946782901)
  end

  specify "mktime is a synonym for Time.local" do
    Time.mktime(2000,"jan",1,20,15,1).inspect.should == Time.local(2000,"jan",1,20,15,1).inspect
  end
  
  # rfc2822
  
  specify "rfc2822 should parse RFC-822 strings" do
    t1 = (Time.utc(1976, 8, 26, 14, 30) + 4 * 3600)
    t2 = Time.rfc2822("26 Aug 76 14:30 EDT")
    t1.should == t2
    
    t3 = Time.utc(1976, 8, 27, 9, 32) + 7 * 3600
    t4 = Time.rfc2822("27 Aug 76 09:32 PDT")
    t3.should == t4
  end
  
  specify "rfc2822 should parse RFC-2822 strings" do
    t1 = Time.utc(1997, 11, 21, 9, 55, 6) + 6 * 3600
    t2 = Time.rfc2822("Fri, 21 Nov 1997 09:55:06 -0600")
    t1.should == t2
    
    t3 = Time.utc(2003, 7, 1, 10, 52, 37) - 2 * 3600
    t4 = Time.rfc2822("Tue, 1 Jul 2003 10:52:37 +0200")
    t3.should == t4
    
    t5 = Time.utc(1997, 11, 21, 10, 1, 10) + 6 * 3600
    t6 = Time.rfc2822("Fri, 21 Nov 1997 10:01:10 -0600")
    t5.should == t6
    
    t7 = Time.utc(1997, 11, 21, 11, 0, 0) + 6 * 3600
    t8 = Time.rfc2822("Fri, 21 Nov 1997 11:00:00 -0600")
    t7.should == t8
    
    t9 = Time.utc(1997, 11, 24, 14, 22, 1) + 8 * 3600
    t10 = Time.rfc2822("Mon, 24 Nov 1997 14:22:01 -0800")
    t9.should == t10
    
    begin
      Time.at(-1)
    rescue ArgumentError
      # ignore
    else
      t11 = Time.utc(1969, 2, 13, 23, 32, 54) + 3 * 3600 + 30 * 60
      t12 = Time.rfc2822("Thu, 13 Feb 1969 23:32:54 -0330")
      t11.should == t12
      
      t13 = Time.utc(1969, 2, 13, 23, 32, 0) + 3 * 3600 + 30 * 60
      t14 = Time.rfc2822(" Thu,
      13
        Feb
          1969
      23:32
               -0330 (Newfoundland Time)")
      t13.should == t14
    end
    
    t15 = Time.utc(1997, 11, 21, 9, 55, 6)
    t16 = Time.rfc2822("21 Nov 97 09:55:06 GMT")
    t15.should == t16
    
    t17 = Time.utc(1997, 11, 21, 9, 55, 6) + 6 * 3600
    t18 = Time.rfc2822("Fri, 21 Nov 1997 09 :   55  :  06 -0600")
    t17.should == t18
    
    should_raise(ArgumentError) do
      # inner comment is not supported.
      Time.rfc2822("Fri, 21 Nov 1997 09(comment):   55  :  06 -0600")
    end
  end
  
  specify "rfc822 should be an alias of rfc2822" do
    Time.rfc822("Fri, 21 Nov 1997 09:55:06 -0600").should == Time.rfc2822("Fri, 21 Nov 1997 09:55:06 -0600")
  end
  
  # httpdate
  
  specify "httpdate should parse RFC-2616 strings" do
    t = Time.utc(1994, 11, 6, 8, 49, 37)
    
    t.should == Time.httpdate("Sun, 06 Nov 1994 08:49:37 GMT")
    
    # relies on Time.parse (not yet implemented)
    # t.should == Time.httpdate("Sunday, 06-Nov-94 08:49:37 GMT")
    
    t.should == Time.httpdate("Sun Nov  6 08:49:37 1994")
    
    Time.utc(1995, 11, 15, 6, 25, 24).should == Time.httpdate("Wed, 15 Nov 1995 06:25:24 GMT")
    
    Time.utc(1995, 11, 15, 4, 58, 8).should == Time.httpdate("Wed, 15 Nov 1995 04:58:08 GMT")
    
    Time.utc(1994, 11, 15, 8, 12, 31).should == Time.httpdate("Tue, 15 Nov 1994 08:12:31 GMT")
    
    Time.utc(1994, 12, 1, 16, 0, 0).should == Time.httpdate("Thu, 01 Dec 1994 16:00:00 GMT")
                 
    Time.utc(1994, 10, 29, 19, 43, 31).should == Time.httpdate("Sat, 29 Oct 1994 19:43:31 GMT")
    
    Time.utc(1994, 11, 15, 12, 45, 26).should == Time.httpdate("Tue, 15 Nov 1994 12:45:26 GMT")
    
    Time.utc(1999, 12, 31, 23, 59, 59).should == Time.httpdate("Fri, 31 Dec 1999 23:59:59 GMT")   
  end
  
  # xmlschema
  
  specify "xmlschema should parse ISO-8601 strings" do
    t = Time.utc(1985, 4, 12, 23, 20, 50, 520000)
    s = "1985-04-12T23:20:50.52Z"
    t.should == Time.xmlschema(s)
    #s.should == t.xmlschema(2)

    t = Time.utc(1996, 12, 20, 0, 39, 57)
    s = "1996-12-19T16:39:57-08:00"
    t.should == Time.xmlschema(s)
    # There is no way to generate time string with arbitrary timezone.
    s = "1996-12-20T00:39:57Z"
    t.should == Time.xmlschema(s)
    #assert_equal(s, t.xmlschema)

    t = Time.utc(1990, 12, 31, 23, 59, 60)
    s = "1990-12-31T23:59:60Z"
    t.should == Time.xmlschema(s)
    # leap second is representable only if timezone file has it.
    s = "1990-12-31T15:59:60-08:00"
    t.should == Time.xmlschema(s)

    begin
      Time.at(-1)
    rescue ArgumentError
      # ignore
    else
      t = Time.utc(1937, 1, 1, 11, 40, 27, 870000)
      s = "1937-01-01T12:00:27.87+00:20"
      t.should == Time.xmlschema(s)
    end
    
    # more
    
    # (Time.utc(1999, 5, 31, 13, 20, 0) + 5 * 3600).should == Time.xmlschema("1999-05-31T13:20:00-05:00")
    # (Time.local(2000, 1, 20, 12, 0, 0)).should == Time.xmlschema("2000-01-20T12:00:00")
    # (Time.utc(2000, 1, 20, 12, 0, 0)).should == Time.xmlschema("2000-01-20T12:00:00Z")
    # (Time.utc(2000, 1, 20, 12, 0, 0) - 12 * 3600).should == Time.xmlschema("2000-01-20T12:00:00+12:00")
    # (Time.utc(2000, 1, 20, 12, 0, 0) + 13 * 3600).should == Time.xmlschema("2000-01-20T12:00:00-13:00")
    # (Time.utc(2000, 3, 4, 23, 0, 0) - 3 * 3600).should == Time.xmlschema("2000-03-04T23:00:00+03:00")
    # (Time.utc(2000, 3, 4, 20, 0, 0)).should == Time.xmlschema("2000-03-04T20:00:00Z")
    # (Time.local(2000, 1, 15, 0, 0, 0)).should == Time.xmlschema("2000-01-15T00:00:00")
    # (Time.local(2000, 2, 15, 0, 0, 0)).should == Time.xmlschema("2000-02-15T00:00:00")
    # (Time.local(2000, 1, 15, 12, 0, 0)).should == Time.xmlschema("2000-01-15T12:00:00")
    # (Time.utc(2000, 1, 16, 12, 0, 0)).should == Time.xmlschema("2000-01-16T12:00:00Z")
    # (Time.local(2000, 1, 1, 12, 0, 0)).should == Time.xmlschema("2000-01-01T12:00:00")
    # (Time.utc(1999, 12, 31, 23, 0, 0)).should == Time.xmlschema("1999-12-31T23:00:00Z")
    # (Time.local(2000, 1, 16, 12, 0, 0)).should == Time.xmlschema("2000-01-16T12:00:00")
    # (Time.local(2000, 1, 16, 0, 0, 0)).should == Time.xmlschema("2000-01-16T00:00:00")
    # (Time.utc(2000, 1, 12, 12, 13, 14)).should == Time.xmlschema("2000-01-12T12:13:14Z")
    # (Time.utc(2001, 4, 17, 19, 23, 17, 300000)).should == Time.xmlschema("2001-04-17T19:23:17.3Z")
  end
  
  specify "iso8601 should be an alias of xmlschema" do
    Time.iso8601("1985-04-12T23:20:50.52Z").should == Time.xmlschema("1985-04-12T23:20:50.52Z")
  end
end

context "Time instance method" do
  specify "+ should increment the time by the specified amount" do
    (Time.at(0) + 100).seconds.should == Time.at(100).seconds
  end
  
  specify "- should decrement the time by the specified amount" do
    (Time.at(100) - 100).seconds.should == Time.at(0).seconds
  end
  
  specify "succ should return a new time one second later than time" do
    Time.at(100).succ.should == Time.at(101)
  end
  
  specify "succ should return a new instance" do
    t1 = Time.at(100)
    t2 = t1.succ
    t1.object_id.should_not == t2.object_id
  end
  
  specify "asctime should return a canonical string representation of time" do
    Time.now.asctime.should == Time.now.strftime("%a %b %d %H:%M:%S %Y")
  end
  
  specify "ctime is a synonym for Time#acstime " do
    Time.now.ctime.should == Time.now.asctime
  end
  
  specify "sec should return the second of the minute(0..60) for time" do
    Time.at(0).sec.should == 0
  end
  
  specify "to_i should return the value of time as an integer number of seconds since epoch" do
    Time.at(0).to_i.should == 0
  end
  
  specify "tv_sec is a synonym for Time#to_i" do
    Time.at(0).tv_sec.should == Time.at(0).to_i
  end
  
  specify "hour should return the hour of the day (0..23) for time" do
    Time.at(0).hour.should == 16
  end
  
  specify "min should return the minute of the hour (0..59) for time" do
    Time.at(0).min.should == 0
  end
  
  specify "day should return the day of the month (1..n) for time" do
    Time.at(0).day.should == 31
  end
  
  specify "mday is a synonym for Time#day" do
    Time.new.mday.should == Time.new.day
  end
  
  specify "<=> should return 1 if the first argument is a point in time after the second argument" do
    (Time.now <=> Time.at(0)).should == 1
    (Time.at(0, 100) <=> Time.at(0, 0)).should == 1
  end
  
  specify "<=> should return 0 iff time is the same as other" do
    (Time.at(100, 100) <=> Time.at(100, 100)).should == 0
    (Time.at(100, 100) <=> Time.at(100, 99)).should == 1
    (Time.at(100, 100) <=> Time.at(101, 100)).should == -1
  end
  
  specify "<=> should return -1 if the first argument is a point in time before the second argument" do
    (Time.at(0) <=> Time.now).should == -1
    (Time.at(0, 0) <=> Time.at(0, 100)).should == -1
  end
  
  specify "eql? should return true iff time is equal in seconds and usecs to other time" do
    Time.at(100, 100).eql?(Time.at(100, 100)).should == true
    Time.at(100, 100).eql?(Time.at(100, 99)).should == false
    Time.at(100, 100).eql?(Time.at(99, 100)).should == false
  end
  
  specify "utc? should return true if time represents a time in UTC (GMT)" do
    Time.now.utc?.should == false
  end
  
  specify "year should return the four digit year for time as an integer" do
    Time.at(0).year.should == 1969
  end
  
  specify "strftime should format time according to the directives in the given format string" do
    Time.at(0).strftime("There is %M minutes in epoch").should == "There is 00 minutes in epoch"
  end
  
  specify "wday should return an integer representing the day of the week, 0..6, with Sunday being 0" do
    Time.at(0).wday.should == 3
  end
  
  specify "yday should return an integer representing the day of the year, 1..366" do
    Time.at(9999999).yday.should == 116
  end
  
  specify "zone should return the time zone used for time" do
    with_timezone("PDT") do
      Time.now.zone.should == "PDT"
    end
  end
  
  specify "mon should return the month of the year" do
    Time.at(99999).mon.should == 1
  end
  
  specify "month is a synonym for Time#mon" do
    Time.at(99999).month.should == Time.at(99999).mon
  end
  
  specify "gmt? should return true if time represents a time in UTC (GMT)" do
    Time.now.gmt?.should == false
    Time.now.gmtime.gmt?.should == true
  end
  
  specify "usec should return the microseconds for time" do
    Time.at(0).usec.should == 0
  end
  
  specify "gmt_offset should return the offset in seconds between the timezone of time and UTC" do
    with_timezone("PDT") do
      Time.new.gmt_offset.should == -25200
    end
  end
  
  specify "utc_offset should be an alias for gmt_offset" do
    with_timezone("PDT") do
      Time.new.utc_offset.should == -25200
    end
  end
  
  specify "gmtoff should be an alias for gmt_offset" do
    with_timezone("PDT") do
      Time.new.gmtoff.should == -25200
    end
  end
  
  specify "localtime should return the local representation of time" do
    t = Time.gm(2007, 1, 9, 12, 0, 0)
    t.localtime
    t.should == Time.local(2007, 1, 9, 4, 0, 0)
  end
  
  specify "gmtime should return the utc representation of time" do
    t = Time.local(2007, 1, 9, 4, 0, 0)
    t.gmtime
    t.should == Time.gm(2007, 1, 9, 12, 0, 0)
  end
  
  specify "getlocal should return a new time which is the local representation of time" do
    t = Time.gm(2007, 1, 9, 12, 0, 0)
    t.localtime.should == Time.local(2007, 1, 9, 4, 0, 0)
  end
  
  specify "getgm should return a new time which is the utc representation of time" do
    t = Time.local(2007, 1, 9, 4, 0, 0)
    t.getgm.should == Time.gm(2007, 1, 9, 12, 0, 0)
  end
  
  specify "getutc should be an alias for getgm" do
    t = Time.local(2007, 1, 9, 4, 0, 0)
    t.getutc.should == Time.gm(2007, 1, 9, 12, 0, 0)
  end
  
  specify "utc should be an alias of gmtime" do
    t = Time.local(2007, 1, 9, 4, 0, 0)
    t.utc
    t.should == Time.gm(2007, 1, 9, 12, 0, 0)
  end
  
  specify "dst? should return whether time is during daylight saving time" do
    Time.local(2007, 9, 9, 0, 0, 0).dst?.should == true
    Time.local(2007, 1, 9, 0, 0, 0).dst?.should == false
  end
  
  specify "isdst should be an alias of dst?" do
    Time.local(2007, 9, 9, 0, 0, 0).isdst.should == true
    Time.local(2007, 1, 9, 0, 0, 0).isdst.should == false
  end
  
  specify "to_f should return the float number of seconds + usecs since the epoch" do
    Time.at(100, 100).to_f.should == 100.0001
  end
  
  specify "to_i should return the integer number of seconds since the epoch" do
    Time.at(100, 100).to_i.should == 100
  end
  
  specify "hash should return a unique integer for each time" do
    Time.at(100).hash.should == 100
    Time.at(100, 123456).hash.should == 123428
    Time.gm(1980).hash.should == 315532800
  end
  
  specify "to_a should return a 10 element array representing the deconstructed time" do
    Time.at(0).to_a.should == [0, 0, 16, 31, 12, 1969, 3, 365, false, "PST"]
  end
end







__END__










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
  setup do
    @origtz = ENV['TZ']
    ENV['TZ'] = 'GMT'
  end

  specify "at should convert to time object" do
    Time.at( 1168475924 ).inspect.should == "Thu Jan 11 00:38:44 +0000 2007"
  end
  
  specify "at should create a new time object with the value given by time" do
    Time.at(Time.now).inspect.should == Time.now.inspect
  end
  
  specify "gm should create a time based on given values, interpreted as UTC (GMT)" do
    Time.gm(2000,"jan",1,20,15,1).inspect.should == "Sat Jan 01 20:15:01 UTC 2000"
  end
  
  specify "utc is a synonym for Time.gm" do
    Time.utc(2000,"jan",1,20,15,1).inspect.should == "Sat Jan 01 20:15:01 UTC 2000"
  end
  
  specify "local should create a time based on given values, interpreted in the local time zone" do
    Time.local(2000,"jan",1,20,15,1).inspect.should == "Sat Jan 01 20:15:01 +0000 2000"
  end
  
  specify "mktime is a synonym for Time.local" do
    Time.mktime(2000,"jan",1,20,15,1).inspect.should == "Sat Jan 01 20:15:01 +0000 2000"
  end
end

context "Time instance method" do
  specify "asctime should return a canonical string representation of time" do
    Time.now.asctime.should == Time.now.strftime("%a %b %d %H:%M:%S %Y")
  end
  
  specify "ctime is a synonym for Time#acstime " do
    Time.now.ctime.should == Time.now.asctime
  end
  
  specify "sec should return the second of the minute(0..60) for time" do
    Time.at(0).sec.should == 0
  end
  
  specify "to_i should return the value of time as an integer number of seconds since epoch" do
    Time.at(0).to_i.should == 0
  end
  
  specify "tv_sec is a synonym for Time#to_i" do
    Time.at(0).tv_sec.should == Time.at(0).to_i
  end
  
  specify "hour should return the hour of the day (0..23) for time" do
    Time.at(0).hour.should == 0
  end
  
  specify "min should return the minute of the hour (0..59) for time" do
    Time.at(0).min.should == 0
  end
  
  specify "day should return the day of the month (1..n) for time" do
    Time.at(0).day.should == 1
  end
  
  specify "mday is a synonym for Time#day" do
    Time.new.mday.should == Time.new.day
  end
  
  specify "<=> should return 1 if the first argument is a point in time after the second argument" do
    (Time.now <=> Time.at(0)).should == 1
  end
  
  specify "<=> should return -1 if the first argument is a point in time before the second argument" do
    (Time.at(0) <=> Time.now).should == -1
  end
  
  specify "utc? should return true if time represents a time in UTC (GMT)" do
    Time.now.utc?.should == false
  end
  
  specify "year should return the name year for time" do
    Time.at(0).year.should == 1970
  end
  
  specify "strftime should format time according to the directives in the given format string" do
    Time.at(0).strftime("There is %M minutes in epoch").should == "There is 00 minutes in epoch"
  end
  
  specify "wday should return an integer representing the day of the week, 0..6, with Sunday being 0" do
    Time.at(0).wday.should == 4
  end
  
  specify "yday should return an integer representing the day of the year, 1..366" do
    Time.at(9999999).yday.should == 116
  end
  
  specify "zone should return the time zone used for time" do
    Time.now.zone.should == 'GMT'
  end
  
  specify "mon should return the month of the year" do
    Time.at(99999).mon.should == 1
  end
  
  specify "month is a synonym for Time#mon" do
    Time.at(99999).month.should == Time.at(99999).mon
  end
  
  specify "gmt? should return true is time represents a time in UTC (GMT)" do
    Time.now.gmt?.should == false
  end
  
  specify "usec should return the microseconds for time" do
    Time.at(0).usec.should == 0
  end
  
  specify "gmt_offset should return the offset in seconds between the timezone of time and UTC" do
    Time.new.gmt_offset.should == 0
  end
end

