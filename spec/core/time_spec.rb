require File.dirname(__FILE__) + '/../spec_helper'

# class methods
# _load, apply_offset, at, gm, local, make_time, mktime,
# month_days, new, now, parse, times, utc,

# +, -, <=>, _dump, asctime, ctime, day, dst?, eql?, getgm, getlocal,
# getutc, gmt?, gmt_offset, gmtime, gmtoff, hash, hour,
# inspect, isdst, localtime, marshal_dump, marshal_load,
# mday, min, mon, month, sec, strftime, succ, to_a,
# to_f, to_i, to_s, tv_sec, tv_usec, usec, utc, utc?, utc_offset,
# wday, yday, year, zone

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

  def with_timezone(zone)
    old = ENV["TZ"]
    ENV["TZ"] = zone

    begin
      yield
    ensure
      ENV["TZ"] = old
    end
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
    Time.gm(2000,"jan",1,20,15,1).inspect.should == "Sat Jan 01 20:15:01 UTC 2000"
  end
  
  specify "gm should create a time based on given C-style gmtime arguments, interpreted as UTC (GMT)" do
    Time.gm(1, 15, 20, 1, 1, 2000, :ignored, :ignored, :ignored, :ignored).inspect.should == "Sat Jan 01 20:15:01 UTC 2000"
  end
  
  specify "utc is a synonym for Time.gm" do
    Time.utc(2000,"jan",1,20,15,1).inspect.should == Time.gm(2000,"jan",1,20,15,1).inspect
  end
  
  #local/mktime
  
  specify "local should create a time based on given values, interpreted in the local time zone" do
    with_timezone("US/Pacific") do
      Time.local(2000,"jan",1,20,15,1).inspect.should == "Sat Jan 01 20:15:01 -0800 2000"
    end
  end
  
  specify "local should create a time based on given C-style gmtime arguments, interpreted in the local time zone" do
    with_timezone("US/Pacific") do
      Time.local(1, 15, 20, 1, 1, 2000, :ignored, :ignored, :ignored, :ignored).inspect.should == "Sat Jan 01 19:15:01 -0800 2000"
    end
  end

  specify "mktime is a synonym for Time.local" do
    Time.mktime(2000,"jan",1,20,15,1).inspect.should == Time.local(2000,"jan",1,20,15,1).inspect
  end
end

context "Time instance method" do
  specify "+ should increment the time by the specified amount" do
    (Time.at(0) + 100).should == Time.at(100)
  end
  
  specify "- should decrement the time by the specified amount" do
    (Time.at(100) - 100).should == Time.at(0)
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
    Time.now.asctime.should == Time.now.strftime("%a %b %e %H:%M:%S %Y")
  end
  
  specify "ctime is a synonym for Time#asctime" do
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
    with_timezone("CET") do
      Time.at(0).hour.should == 1
    end
  end
  
  specify "min should return the minute of the hour (0..59) for time" do
    with_timezone("CET") do
      Time.at(0).min.should == 0
    end
  end
  
  specify "day should return the day of the month (1..n) for time" do
    with_timezone("CET") do
      Time.at(0).day.should == 1
    end
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
    with_timezone("CET") do
      Time.at(0).year.should == 1970
    end
  end
  
  specify "strftime should format time according to the directives in the given format string" do
    Time.at(0).strftime("There is %M minutes in epoch").should == "There is 00 minutes in epoch"
  end
  
  specify "wday should return an integer representing the day of the week, 0..6, with Sunday being 0" do
    with_timezone("GMT") do
      Time.at(0).wday.should == 4
    end
  end
  
  specify "yday should return an integer representing the day of the year, 1..366" do
    Time.at(9999999).yday.should == 116
  end
  
  specify "zone should return the time zone used for time" do
    # Testing with Asia/Kuwait here because it doesn't have DST.
    with_timezone("Asia/Kuwait") do
      Time.now.zone.should == "AST"
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
    with_timezone("Asia/Kuwait") do
      Time.new.gmt_offset.should == 10800
    end
  end
  
  specify "utc_offset should be an alias for gmt_offset" do
    with_timezone("Asia/Kuwait") do
      Time.new.utc_offset.should == 10800
    end
  end
  
  specify "gmtoff should be an alias for gmt_offset" do
    with_timezone("Asia/Kuwait") do
      Time.new.gmtoff.should == 10800
    end
  end
  
  specify "localtime should return the local representation of time" do
    # Testing with America/Regina here because it doesn't have DST.
    with_timezone("America/Regina") do
      t = Time.gm(2007, 1, 9, 12, 0, 0)
      t.localtime
      t.should == Time.local(2007, 1, 9, 6, 0, 0)
    end
  end
  
  specify "gmtime should return the utc representation of time" do
    # Testing with America/Regina here because it doesn't have DST.
    with_timezone("America/Regina") do
      t = Time.local(2007, 1, 9, 6, 0, 0)
      t.gmtime
      t.should == Time.gm(2007, 1, 9, 12, 0, 0)
    end
  end
  
  specify "getlocal should return a new time which is the local representation of time" do
    # Testing with America/Regina here because it doesn't have DST.
    with_timezone("America/Regina") do
      t = Time.gm(2007, 1, 9, 12, 0, 0)
      t.localtime.should == Time.local(2007, 1, 9, 6, 0, 0)
    end
  end
  
  specify "getgm should return a new time which is the utc representation of time" do
    # Testing with America/Regina here because it doesn't have DST.
    with_timezone("America/Regina") do
      t = Time.local(2007, 1, 9, 6, 0, 0)
      t.getgm.should == Time.gm(2007, 1, 9, 12, 0, 0)
    end
  end
  
  specify "getutc should be an alias for getgm" do
    # Testing with America/Regina here because it doesn't have DST.
    with_timezone("America/Regina") do
      t = Time.local(2007, 1, 9, 6, 0, 0)
      t.getutc.should == Time.gm(2007, 1, 9, 12, 0, 0)
    end
  end
  
  specify "utc should be an alias of gmtime" do
    # Testing with America/Regina here because it doesn't have DST.
    with_timezone("America/Regina") do
      t = Time.local(2007, 1, 9, 6, 0, 0)
      t.utc
      t.should == Time.gm(2007, 1, 9, 12, 0, 0)
    end
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
    # Testing with America/Regina here because it doesn't have DST.
    with_timezone("America/Regina") do
      Time.at(0).to_a.should == [0, 0, 18, 31, 12, 1969, 3, 365, false, "CST"]
    end
  end
end
