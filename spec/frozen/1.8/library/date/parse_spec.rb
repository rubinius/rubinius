require 'date' 
require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/parse'
require File.dirname(__FILE__) + '/shared/parse_us'
require File.dirname(__FILE__) + '/shared/parse_eu'

describe "Date#parse" do


  # The space separator is also different, doesn't work for only numbers
  it "can parse a day name into a Date object" do
    d = Date.parse("friday")
    d.should == Date.commercial(d.cwyear, d.cweek, 5)
  end

  it "can parse a month name into a Date object" do
    d = Date.parse("october")
    d.should == Date.civil(Date.today.year, 10)
  end

  it "can parse a month day into a Date object" do
    d = Date.parse("5th")
    d.should == Date.civil(Date.today.year, Date.today.month, 5)
  end

  # Specs using numbers
  it "can't handle a single digit" do
    lambda{ Date.parse("1") }.should raise_error(ArgumentError)
  end

  it "can handle DD as month day number" do
    d = Date.parse("10")
    d.should == Date.civil(Date.today.year, Date.today.month, 10)
  end

  it "can handle DDD as year day number" do
    d = Date.parse("100")
    d.should == Date.civil(Date.today.year, 4, 9)
  end

  it "can handle MMDD as month and day" do
    d = Date.parse("1108")
    d.should == Date.civil(Date.today.year, 11, 8)
  end

  it "can handle YYDDD as year and day number" do
    d = Date.parse("10100")
    d.should == Date.civil(10, 4, 10)
  end

  it "can handle YYMMDD as year month and day" do
    d = Date.parse("201023")
    d.should == Date.civil(20, 10, 23)
  end

  it "can handle YYYYDDD as year and day number" do
    d = Date.parse("1910100")
    d.should == Date.civil(1910, 4, 10)
  end

  it "can handle YYYYMMDD as year and day number" do
    d = Date.parse("19101101")
    d.should == Date.civil(1910, 11, 1)
  end

  # The combination of using the - separator and using names for
  # months doesn't work, so - is excluded here.
  it_behaves_like(:date_parse, '.')
  it_behaves_like(:date_parse, '/')
  it_behaves_like(:date_parse, ' ')

  # Numeric date parsing doesn't work when using spaces
  it_behaves_like(:date_parse_us, '/')
  it_behaves_like(:date_parse_eu, '-')
  ruby_version_is "" ... "1.8.7" do
    it_behaves_like(:date_parse_us, '.')
  end
end

ruby_version_is "1.8.7" do
  describe "Date#parse(.)" do
    it "parses a YYYY.MM.DD string into a Date object" do
      d = Date.parse("2007.10.01")
      d.year.should  == 2007
      d.month.should == 10
      d.day.should   == 1
    end

    it "parses a DD.MM.YYYY string into a Date object" do
      d = Date.parse("10.01.2007")
      d.year.should  == 2007
      d.month.should == 1
      d.day.should   == 10
    end

    it "parses a YY.MM.DD string into a Date object" do
      d = Date.parse("10.01.07")
      d.year.should  == 10
      d.month.should == 1
      d.day.should   == 7
    end

    it "parses a YY.MM.DD string into a Date object using the year digits as 20XX" do
      d = Date.parse("10.01.07", true)
      d.year.should  == 2010
      d.month.should == 1
      d.day.should   == 7
    end
  end
end