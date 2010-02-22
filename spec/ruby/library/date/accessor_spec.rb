require 'date' 
require File.expand_path('../../../spec_helper', __FILE__)

describe "Date#ajd" do
  it "should be able to determine the Astronomical Julian day for a date" do
    Date.civil(2007, 1, 17).ajd.should == 4908235.to_r / 2
  end
end

describe "Date#amjd" do
  it "should be able to determine the Astronomical Modified Julian day for a date" do
    Date.civil(2007, 1, 17).amjd.should == 54117
  end
end

describe "Date#day_fraction" do
  it "should be able to determine the day fraction for a date" do
    Date.civil(2007, 1, 17).day_fraction.should == 0
  end
end

describe "Date#mjd" do
  it "should be able to determine the Modified Julian day for a date" do
    Date.civil(2007, 1, 17).mjd.should == 54117
  end 
end

describe "Date#ld" do
  it "should be able to determine the Modified Julian day for a date" do
    Date.civil(2007, 1, 17).ld.should == 154958
  end 
end

describe "Date#year" do
  it "should be able to determine the year for a date" do
    Date.civil(2007, 1, 17).year.should == 2007
  end 
end

describe "Date#yday" do
  it "should be able to determine the year for a date" do
    Date.civil(2007,  1, 17).yday.should == 17
    Date.civil(2008, 10, 28).yday.should == 302
  end 
end

describe "Date#mon" do
  it "should be able to determine the month for a date" do
    Date.civil(2007,  1, 17).mon.should == 1
    Date.civil(2008, 10, 28).mon.should == 10
  end 
end

describe "Date#mday" do
  it "should be able to determine the day of the month for a date" do
    Date.civil(2007,  1, 17).mday.should == 17
    Date.civil(2008, 10, 28).mday.should == 28
  end
end

describe "Date#wday" do
  it "should be able to determine the week day for a date" do
    Date.civil(2007,  1, 17).wday.should == 3
    Date.civil(2008, 10, 26).wday.should == 0
  end
end

describe "Date#cwyear" do
  it "should be able to determine the commercial year for a date" do
    Date.civil(2007,  1, 17).cwyear.should == 2007
    Date.civil(2008, 10, 28).cwyear.should == 2008
    Date.civil(2007, 12, 31).cwyear.should == 2008
    Date.civil(2010,  1,  1).cwyear.should == 2009
  end
end

describe "Date#cweek" do
  it "should be able to determine the commercial week for a date" do
    Date.civil(2007,  1, 17).cweek.should == 3
    Date.civil(2008, 10, 28).cweek.should == 44
    Date.civil(2007, 12, 31).cweek.should == 1
    Date.civil(2010,  1,  1).cweek.should == 53
  end
end

describe "Date#cwday" do
  it "should be able to determine the commercial week day for a date" do
    Date.civil(2007,  1, 17).cwday.should == 3
    Date.civil(2008, 10, 26).cwday.should == 7
  end  
end