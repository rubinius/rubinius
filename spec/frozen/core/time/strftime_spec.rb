require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'

describe "Time#strftime" do
  it "formats time according to the directives in the given format string" do
    with_timezone("GMT", 0) do
      Time.at(0).strftime("There is %M minutes in epoch").should == "There is 00 minutes in epoch"
    end
  end

  it "supports week of year format with %U and %W" do
    # start of the yer
    saturday_first = Time.local(2000,1,1,14,58,42)
    saturday_first.strftime("%U").should == "00"
    saturday_first.strftime("%W").should == "00"

    sunday_second = Time.local(2000,1,2,14,58,42)
    sunday_second.strftime("%U").should == "01"
    sunday_second.strftime("%W").should == "00"

    monday_third = Time.local(2000,1,3,14,58,42)
    monday_third.strftime("%U").should == "01"
    monday_third.strftime("%W").should == "01"

    sunday_9th = Time.local(2000,1,9,14,58,42)
    sunday_9th.strftime("%U").should == "02"
    sunday_9th.strftime("%W").should == "01"

    monday_10th = Time.local(2000,1,10,14,58,42)
    monday_10th.strftime("%U").should == "02"
    monday_10th.strftime("%W").should == "02"

    # middle of the year
    some_sunday = Time.local(2000,8,6,4,20,00)
    some_sunday.strftime("%U").should == "32"
    some_sunday.strftime("%W").should == "31"
    some_monday = Time.local(2000,8,7,4,20,00)
    some_monday.strftime("%U").should == "32"
    some_monday.strftime("%W").should == "32"

    # end of year, and start of next one
    saturday_30th = Time.local(2000,12,30,14,58,42)
    saturday_30th.strftime("%U").should == "52"
    saturday_30th.strftime("%W").should == "52"

    sunday_last = Time.local(2000,12,31,14,58,42)
    sunday_last.strftime("%U").should == "53"
    sunday_last.strftime("%W").should == "52"

    monday_first = Time.local(2001,1,1,14,58,42)
    monday_first.strftime("%U").should == "00"
    monday_first.strftime("%W").should == "01"
  end

  it "supports mm/dd/yy formatting with %D" do
    now = Time.now
    mmddyy = now.strftime('%m/%d/%y')
    now.strftime('%D').should == mmddyy
  end

  it "supports HH:MM:SS formatting with %T" do
    now = Time.now
    hhmmss = now.strftime('%H:%M:%S')
    now.strftime('%T').should == hhmmss
  end

  it "supports timezone formatting with %z" do
    with_timezone("UTC", 0) do
      time = Time.utc(2005, 2, 21, 17, 44, 30)
      time.strftime("%z").should == "+0000"
    end
  end
end
