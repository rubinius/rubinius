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

def with_timezone(name, offset)
  # TZ convention is backwards
  offset = -offset

  zone = name.dup
  zone << offset.to_s
  zone << ":00:00"

  old = ENV["TZ"]
  ENV["TZ"] = zone

  begin
    yield
  ensure
    ENV["TZ"] = old
  end
end

def localtime(seconds)
  if RUBY_PLATFORM =~ /darwin/
    `date -r #{seconds} +'%a %b %d %H:%M:%S %z %Y'`.chomp
  elsif RUBY_PLATFORM =~ /linux/
    `date -d @#{seconds} +'%a %b %d %H:%M:%S %z %Y'`.chomp
  else
    `date -j -f "%s" #{seconds} "+%a %b %d %H:%M:%S %z %Y"`.chomp
  end
end

describe "Time.at" do
  it "converts to time object" do
    Time.at( 1168475924 ).inspect.should == localtime(1168475924)
  end
  
  it "creates a new time object with the value given by time" do
    t = Time.now
    Time.at(t).inspect.should == t.inspect
  end
  
  it "creates a dup time object with the value given by time" do
    t1 = Time.new
    t2 = Time.at(t1)
    t1.object_id.should_not == t2.object_id
  end
end

time_gm = shared "Time.gm" do |cmd|
  describe "Time.#{cmd}" do
    it "creates a time based on given values, interpreted as UTC (GMT)" do
      Time.send(cmd, 2000,"jan",1,20,15,1).inspect.should == "Sat Jan 01 20:15:01 UTC 2000"
    end

    it "creates a time based on given C-style gmtime arguments, interpreted as UTC (GMT)" do
      Time.send(cmd, 1, 15, 20, 1, 1, 2000, :ignored, :ignored, :ignored, :ignored).inspect.should == "Sat Jan 01 20:15:01 UTC 2000"
    end
  end
end

describe "Time.gm" do
  it_behaves_like(time_gm, :gm)
end

describe "Time.utc" do
  it_behaves_like(time_gm, :utc)
end

time_local = shared "Time.local" do |cmd|
  describe "Time.#{cmd}" do
    it "local should create a time based on given values, interpreted in the local time zone" do
      with_timezone("PST", -8) do
        Time.send(cmd, 2000,"jan",1,20,15,1).inspect.should == "Sat Jan 01 20:15:01 -0800 2000"
      end
    end

    it "local should create a time based on given C-style gmtime arguments, interpreted in the local time zone" do
      with_timezone("PST", -8) do
        Time.send(cmd, 1, 15, 20, 1, 1, 2000, :ignored, :ignored, :ignored, :ignored).inspect.should == "Sat Jan 01 20:15:01 -0800 2000"
      end
    end
  end
end

describe "Time.local" do
  it_behaves_like(time_local, :local)
end

describe "Time.mktime" do
  it_behaves_like(time_local, :mktime)
end

describe "Time#+" do
  it "increments the time by the specified amount" do
    (Time.at(0) + 100).should == Time.at(100)
  end
end

describe "Time#-" do
  it "decrements the time by the specified amount" do
    (Time.at(100) - 100).should == Time.at(0)
    (Time.at(100) - Time.at(99)).should == 1.0
  end
end

describe "Time#succ" do
  it "returns a new time one second later than time" do
    Time.at(100).succ.should == Time.at(101)
  end
  
  it "returns a new instance" do
    t1 = Time.at(100)
    t2 = t1.succ
    t1.object_id.should_not == t2.object_id
  end
end

time_asctime = shared "Time#asctime" do |cmd|
  describe "Time##{cmd}" do
    it "returns a canonical string representation of time" do
      t = Time.now
      t.send(cmd).should == t.strftime("%a %b %e %H:%M:%S %Y")
    end
  end
end

describe "Time#asctime" do
  it_behaves_like(time_asctime, :asctime)
end

describe "Time#ctime" do
  it_behaves_like(time_asctime, :ctime)
end

describe "Time#sec" do
  it "returns the second of the minute(0..60) for time" do
    Time.at(0).sec.should == 0
  end
end

time_to_i = shared "Time#to_i" do |cmd|
  describe "Time##{cmd}" do
    it "returns the value of time as an integer number of seconds since epoch" do
      Time.at(0).send(cmd).should == 0
    end
  end
end

describe "Time#to_i" do
  it_behaves_like(time_to_i, :to_i)
end

describe "Time#tv_sec" do
  it_behaves_like(time_to_i, :tv_sec)
end

describe "Time#hour" do
  it "returns the hour of the day (0..23) for time" do
    with_timezone("CET", 1) do
      Time.at(0).hour.should == 1
    end
  end
end

describe "Time#min" do
  it "returns the minute of the hour (0..59) for time" do
    with_timezone("CET", 1) do
      Time.at(0).min.should == 0
    end
  end
end

time_day = shared "Time#day" do |cmd|
  describe "Time##{cmd}" do
    it "returns the day of the month (1..n) for time" do
      with_timezone("CET", 1) do
        Time.at(0).send(cmd).should == 1
      end
    end  
  end
end

describe "Time#day" do
  it_behaves_like(time_day, :day)
end

describe "Time#mday" do
  it_behaves_like(time_day, :mday)
end

describe "Time#<=>" do
  it "returns 1 if the first argument is a point in time after the second argument" do
    (Time.now <=> Time.at(0)).should == 1
    (Time.at(0, 100) <=> Time.at(0, 0)).should == 1
  end
  
  it "returns 0 if time is the same as other" do
    (Time.at(100, 100) <=> Time.at(100, 100)).should == 0
    (Time.at(100, 100) <=> Time.at(100, 99)).should == 1
    (Time.at(100, 100) <=> Time.at(101, 100)).should == -1
  end
  
  it "returns -1 if the first argument is a point in time before the second argument" do
    (Time.at(0) <=> Time.now).should == -1
    (Time.at(0, 0) <=> Time.at(0, 100)).should == -1
  end  
end

describe "Time#eql?" do
  it "returns true iff time is equal in seconds and usecs to other time" do
    Time.at(100, 100).eql?(Time.at(100, 100)).should == true
    Time.at(100, 100).eql?(Time.at(100, 99)).should == false
    Time.at(100, 100).eql?(Time.at(99, 100)).should == false
  end  
end

describe "Time#utc?" do
  it "returns true if time represents a time in UTC (GMT)" do
    Time.now.utc?.should == false
  end  
end

describe "Time#year" do
  it "returns the four digit year for time as an integer" do
    with_timezone("CET", 1) do
      Time.at(0).year.should == 1970
    end
  end  
end

describe "Time#strftime" do
  it "formats time according to the directives in the given format string" do
    Time.at(0).strftime("There is %M minutes in epoch").should == "There is 00 minutes in epoch"
  end
end

describe "Time#wday" do
  it "returns an integer representing the day of the week, 0..6, with Sunday being 0" do
    with_timezone("GMT", 0) do
      Time.at(0).wday.should == 4
    end
  end
end

describe "Time#yday" do
  it "returns an integer representing the day of the year, 1..366" do
    Time.at(9999999).yday.should == 116
  end
end

describe "Time#zone" do
  it "returns the time zone used for time" do
    # Testing with Asia/Kuwait here because it doesn't have DST.
    with_timezone("AST", 3) do
      Time.now.zone.should == "AST"
    end
  end
end

time_month = shared "Time#month" do |cmd|
  describe "Time##{cmd}" do
    it "returns the month of the year" do
      Time.at(99999).mon.should == 1
    end
  end
end

describe "Time#mon" do
  it_behaves_like(time_month, :mon)
end

describe "Time#month" do
  it_behaves_like(time_month, :month)
end

describe "Time#gmt?" do
  it "returns true if time represents a time in UTC (GMT)" do
    Time.now.gmt?.should == false
    Time.now.gmtime.gmt?.should == true
  end
end

describe "Time#usec" do
  it "returns the microseconds for time" do
    Time.at(0).usec.should == 0
  end
end

time_gmt_offset = shared "Time#gmt_offset" do |cmd|
  describe "Time##{cmd}" do
    it "returns the offset in seconds between the timezone of time and UTC" do
      with_timezone("AST", 3) do
        Time.new.send(cmd).should == 10800
      end
    end
  end
end

describe "Time#gmt_offset" do
  it_behaves_like(time_gmt_offset, :gmt_offset)
end

describe "Time#utc_offset" do
  it_behaves_like(time_gmt_offset, :utc_offset)
end

describe "Time#gmtoff" do
  it_behaves_like(time_gmt_offset, :gmtoff)
end

describe "Time#localtime" do
  it "returns the local representation of time" do
    # Testing with America/Regina here because it doesn't have DST.
    with_timezone("CST", -6) do
      t = Time.gm(2007, 1, 9, 12, 0, 0)
      t.localtime
      t.should == Time.local(2007, 1, 9, 6, 0, 0)
    end
  end
end

time_gmtime = shared "Time#gmtime" do |cmd|
  describe "Time##{cmd}" do
    it "returns the utc representation of time" do
      # Testing with America/Regina here because it doesn't have DST.
      with_timezone("CST", -6) do
        t = Time.local(2007, 1, 9, 6, 0, 0)
        t.send(cmd)
        t.should == Time.gm(2007, 1, 9, 12, 0, 0)
      end
    end
  end
end

describe "Time#gmtime" do
  it_behaves_like(time_gmtime, :gmtime)
end

describe "Time#utc" do
  it_behaves_like(time_gmtime, :utc)
end  

describe "Time#getlocal" do
  it "returns a new time which is the local representation of time" do
    # Testing with America/Regina here because it doesn't have DST.
    with_timezone("CST", -6) do
      t = Time.gm(2007, 1, 9, 12, 0, 0)
      t.localtime.should == Time.local(2007, 1, 9, 6, 0, 0)
    end
  end
end

time_getgm = shared "Time#getgm" do |cmd|
  describe "Time##{cmd}" do
    it "returns a new time which is the utc representation of time" do
      # Testing with America/Regina here because it doesn't have DST.
      with_timezone("CST", -6) do
        t = Time.local(2007, 1, 9, 6, 0, 0)
        t.send(cmd).should == Time.gm(2007, 1, 9, 12, 0, 0)
      end
    end
  end
end

describe "Time#getgm" do
  it_behaves_like(time_getgm, :getgm)
end

describe "Time#getutc" do
  it_behaves_like(time_getgm, :getutc)
end

time_isdst = shared "Time#isdst" do |cmd|
  describe "Time##{cmd}" do
    it "dst? returns whether time is during daylight saving time" do
      Time.local(2007, 9, 9, 0, 0, 0).send(cmd).should == true
      Time.local(2007, 1, 9, 0, 0, 0).send(cmd).should == false
    end
  end
end

describe "Time#dst?" do
  it_behaves_like(time_isdst, :dst?)
end

describe "Time#isdst" do
  it_behaves_like(time_isdst, :isdst)
end  

describe "Time#to_f" do
  it "returns the float number of seconds + usecs since the epoch" do
    Time.at(100, 100).to_f.should == 100.0001
  end
end

describe "Time#hash" do
  it "returns a unique integer for each time" do
    Time.at(100).hash.should == 100
    Time.at(100, 123456).hash.should == 123428
    Time.gm(1980).hash.should == 315532800
  end
end

describe "Time#to_a" do
  it "returns a 10 element array representing the deconstructed time" do
    # Testing with America/Regina here because it doesn't have DST.
    with_timezone("CST", -6) do
      Time.at(0).to_a.should == [0, 0, 18, 31, 12, 1969, 3, 365, false, "CST"]
    end
  end
end

describe "Time#dup" do
  it "returns a Time object that represents the same time" do
	  t = Time.at(100)
	  t.dup.tv_sec.should == t.tv_sec
  end

  it "should copy the gmt state flag" do
	  Time.now.gmtime.dup.gmt?.should == true
  end

  it "returns an independent Time object" do
	  t = Time.now
	  t2 = t.dup
	  t.gmtime

	  t2.gmt?.should == false
  end
end
