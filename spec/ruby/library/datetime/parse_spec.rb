require File.expand_path('../../../spec_helper', __FILE__)
require 'date'

describe "DateTime.parse" do
  it "needs to be reviewed for spec completeness"

  it "can parse a day name into a DateTime object" do
    d = DateTime.parse("friday")
    d.should == DateTime.commercial(d.cwyear, d.cweek, 5)
  end

  it "can parse a month name into a Date object" do
    d = DateTime.parse("october")
    d.should == DateTime.civil(DateTime.send(:today).year, 10)
  end

  it "can parse a month day into a Date object" do
    d = DateTime.parse("5th")
    d.should == DateTime.civil(DateTime.send(:today).year, DateTime.send(:today).month, 5)
  end

  # Specs using numbers
  it "can't handle a single digit" do
    lambda{ DateTime.parse("1") }.should raise_error(ArgumentError)
  end

  it "can handle DD as month day number" do
    d = DateTime.parse("10")
    d.should == DateTime.civil(DateTime.send(:today).year, DateTime.send(:today).month, 10)
  end

  it "can handle DDD as year day number" do
    d = DateTime.parse("100")
    if DateTime.gregorian_leap?(DateTime.send(:today).year)
      d.should == DateTime.civil(DateTime.send(:today).year, 4, 9)
    else
      d.should == DateTime.civil(DateTime.send(:today).year, 4, 10)
    end
  end

  it "can handle MMDD as month and day" do
    d = DateTime.parse("1108")
    d.should == DateTime.civil(DateTime.send(:today).year, 11, 8)
  end

  it "can handle YYYYMMDD as year, month and day" do
    d = DateTime.parse("20121108")
    d.should == DateTime.civil(2012, 11, 8)
  end

  context "YYYY-MM-DDTHH:MM:SS format" do
    it "can handle valid values" do
      d = DateTime.parse("2012-11-08T15:43:59")
      d.should == DateTime.civil(2012, 11, 8, 15, 43, 59)
    end

    it "can't handle invalid month values" do
      lambda{DateTime.parse("2012-13-08T15:43:59")}.should raise_error(ArgumentError)
    end

    it "can't handle invalid day values" do
      lambda{DateTime.parse("2012-12-32T15:43:59")}.should raise_error(ArgumentError)
    end

    it "can't handle invalid hour values" do
      lambda{DateTime.parse("2012-12-31T25:43:59")}.should raise_error(ArgumentError)
    end

    it "can't handle invalid minute values" do
      lambda{DateTime.parse("2012-12-31T25:43:59")}.should raise_error(ArgumentError)
    end

    #this seems like unexpected behaviour why not raise an ArgumentError?
    #also according to ISO8601 seconds can be 60 to allow for a leap second
    it "truncates seconds down to 59" do
      d = DateTime.parse("2012-11-08T15:43:61")

      d.strftime('%c').should == DateTime.civil(2012, 11, 8, 15, 43, 59).strftime('%c')
    end

  end

  ruby_version_is "" ... "1.9" do
    it "can handle YYDDD as year and day number" do
      d = DateTime.parse("10100")
      d.should == DateTime.civil(10, 4, 10)
    end

    it "can handle YYMMDD as year month and day" do
      d = DateTime.parse("201023")
      d.should == DateTime.civil(20, 10, 23)
    end
  end

  ruby_version_is "1.9" do
    it "can handle YYDDD as year and day number in 1969--2068" do
      d = DateTime.parse("10100")
      d.should == DateTime.civil(2010, 4, 10)
    end

    it "can handle YYMMDD as year month and day in 1969--2068" do
      d = DateTime.parse("201023")
      d.should == DateTime.civil(2020, 10, 23)
    end
  end

  it "can handle YYYYDDD as year and day number" do
    d = DateTime.parse("1910100")
    d.should == DateTime.civil(1910, 4, 10)
  end

  it "can handle YYYYMMDD as year and day number" do
    d = DateTime.parse("19101101")
    d.should == DateTime.civil(1910, 11, 1)
  end
end

ruby_version_is "1.8.7" do
  describe "DateTime.parse(.)" do
    it "parses a YYYY.MM.DD string into a Date object" do
      d = DateTime.parse("2007.10.01")
      d.year.should  == 2007
      d.month.should == 10
      d.day.should   == 1
    end

    it "parses a DD.MM.YYYY string into a Date object" do
      d = DateTime.parse("10.01.2007")
      d.year.should  == 2007
      d.month.should == 1
      d.day.should   == 10
    end

    ruby_version_is "" ... "1.9" do
      it "parses a YY.MM.DD string into a Date object" do
        d = DateTime.parse("10.01.07")
        d.year.should  == 10
        d.month.should == 1
        d.day.should   == 7
      end
    end

    ruby_version_is "1.9" do
      it "parses a YY.MM.DD string into a Date object" do
        d = DateTime.parse("10.01.07")
        d.year.should  == 2010
        d.month.should == 1
        d.day.should   == 7
      end
    end

    it "parses a YY.MM.DD string into a Date object using the year digits as 20XX" do
      d = DateTime.parse("10.01.07", true)
      d.year.should  == 2010
      d.month.should == 1
      d.day.should   == 7
    end
  end

end
