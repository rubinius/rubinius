require File.expand_path('../../../spec_helper', __FILE__)
require 'date'

describe "Date#gregorian?" do

  it "marks a day before the calendar reform as Julian" do
    Date.civil(1007, 2, 27).gregorian?.should == false
    Date.civil(1907, 2, 27, Date.civil(1930, 1, 1).jd).gregorian?.should == false
  end

  it "marks a day after the calendar reform as Julian" do
    Date.civil(2007, 2, 27).gregorian?.should == true
    Date.civil(1607, 2, 27, Date.civil(1582, 1, 1).jd).gregorian?.should == true
  end

end

describe "Date#gregorian_leap?" do
  it "returns true if a year is a leap year in the Gregorian calendar" do
    Date.gregorian_leap?(2000).should == true
    Date.gregorian_leap?(2004).should == true
  end

  it "returns false if a year is not a leap year in the Gregorian calendar" do
    Date.gregorian_leap?(1900).should == false
    Date.gregorian_leap?(1999).should == false
    Date.gregorian_leap?(2002).should == false
  end
end

describe "Date.gregorian?" do
  ruby_version_is "" ... "1.9" do

    it "marks the date as Gregorian if using the Gregorian calendar" do
      Date.gregorian?(Date.civil(1007, 2, 27).jd, Date::GREGORIAN).should == true
    end

    it "marks the date as not Gregorian if using the Julian calendar" do
      Date.gregorian?(Date.civil(1007, 2, 27).jd, Date::JULIAN).should == false
    end

    it "marks the date before the English Day of Calendar Reform as not Gregorian" do
      Date.gregorian?(Date.civil(1752, 9, 13).jd, Date::ENGLAND).should == false
    end

    it "marks the date after the English Day of Calendar Reform as Gregorian" do
      Date.gregorian?(Date.civil(1752, 9, 14).jd, Date::ENGLAND).should == true
    end

    it "marks the date before the Italian Day of Calendar Reform as not Gregorian" do
      Date.gregorian?(Date.civil(1582, 10, 4).jd, Date::ITALY).should == false
    end

    it "marks the date after the Italian Day of Calendar Reform as Gregorian" do
      Date.gregorian?(Date.civil(1582, 10, 15).jd, Date::ITALY).should == true
    end

  end

end
