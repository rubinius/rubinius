require 'date'
require File.expand_path('../../../spec_helper', __FILE__)

describe "Date#jd" do

  it "constructs a Date object based on the Julian day" do
    Date.jd(2454482).should == Date.civil(2008, 1, 16)
  end

  it "determines the Julian day for a Date object" do
    Date.civil(2008, 1, 16).jd.should == 2454482
  end

end

describe "Date#julian?" do

  it "marks a day before the calendar reform as Julian" do
    Date.civil(1007, 2, 27).julian?.should == true
    Date.civil(1907, 2, 27, Date.civil(1930, 1, 1).jd).julian?.should == true
  end

  it "marks a day after the calendar reform as Julian" do
    Date.civil(2007, 2, 27).julian?.should == false
    Date.civil(1607, 2, 27, Date.civil(1582, 1, 1).jd).julian?.should == false
  end

end

describe "Date#julian_leap?" do

  it "determines whether a year is a leap year in the Julian calendar" do
    Date.julian_leap?(1900).should == true
    Date.julian_leap?(1999).should == false
    Date.julian_leap?(2000).should == true
    Date.julian_leap?(2002).should == false
    Date.julian_leap?(2004).should == true
  end

end

describe "Date#valid_jd?" do
  ruby_version_is "" ... "1.9" do

    it "determines if a day number is a valid Julian day number, true for all numbers" do
      # This might need to check the type of the jd parameter. Date.valid_jd?(:number) is of course
      # bogus but returns itself with the current implementation
      Date.valid_jd?(-100).should == -100
      Date.valid_jd?(0).should    ==    0
      Date.valid_jd?(100).should  ==  100
    end
  end

end

describe "Date.julian?" do
  ruby_version_is "" ... "1.9" do

    it "marks the date as not Julian if using the Gregorian calendar" do
      Date.julian?(Date.civil(1007, 2, 27).jd, Date::GREGORIAN).should == false
    end

    it "marks the date as Julian if using the Julian calendar" do
      Date.julian?(Date.civil(1007, 2, 27).jd, Date::JULIAN).should == true
    end

    it "marks the date before the English Day of Calendar Reform as Julian" do
      Date.julian?(Date.civil(1752, 9, 13).jd, Date::ENGLAND).should == true
    end

    it "marks the date after the English Day of Calendar Reform as not Julian" do
      Date.julian?(Date.civil(1752, 9, 14).jd, Date::ENGLAND).should == false
    end

    it "marks the date before the Italian Day of Calendar Reform as Julian" do
      Date.julian?(Date.civil(1582, 10, 4).jd, Date::ITALY).should == true
    end

    it "marks the date after the Italian Day of Calendar Reform as not Julian" do
      Date.julian?(Date.civil(1582, 10, 15).jd, Date::ITALY).should == false
    end

  end

end

describe "Date#valid_jd?" do
  ruby_version_is "1.9" do

    it "determines if a day number is a valid Julian day number, true for all numbers" do
      # This might need to check the type of the jd parameter. Date.valid_jd?(:number) is of course
      # bogus but returns itself with the current implementation
      Date.valid_jd?(-100).should == true
      Date.valid_jd?(0).should    == true
      Date.valid_jd?(100).should  == true
    end

  end
end

describe "Date.julian?" do
  it "needs to be reviewed for spec completeness"
end
