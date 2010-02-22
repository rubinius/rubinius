require 'date' 
require File.expand_path('../../../spec_helper', __FILE__)

describe "Date#jd" do

  it "should be able to construct a Date object based on the Julian day" do
    Date.jd(2454482).should == Date.civil(2008, 1, 16)
  end

  it "should be able to determine the Julian day for a Date object" do
    Date.civil(2008, 1, 16).jd.should == 2454482
  end
  
end

describe "Date#julian?" do

  it "should mark a day before the calendar reform as Julian" do
    Date.civil(1007, 2, 27).julian?.should == true
    Date.civil(1907, 2, 27, Date.civil(2000, 1, 1).jd).julian?.should == true
  end
  
  it "should mark a day after the calendar reform as Julian" do
    Date.civil(2007, 2, 27).julian?.should == false
    Date.civil(1007, 2, 27, Date.civil(1000, 1, 1).jd).julian?.should == false
  end
  
end

describe "Date#julian_leap?" do

  it "should be able to determine whether a year is a leap year in the Julian calendar" do
    Date.julian_leap?(1900).should == true
    Date.julian_leap?(1999).should == false
    Date.julian_leap?(2000).should == true
    Date.julian_leap?(2002).should == false
    Date.julian_leap?(2004).should == true
  end

end

ruby_version_is "" ... "1.9" do
  describe "Date#valid_jd?" do

    it "should be able to determine if a day number is a valid Julian day number, true for all numbers" do
      # This might need to check the type of the jd parameter. Date.valid_jd?(:number) is of course
      # bogus but returns itself with the current implementation
      Date.valid_jd?(-100).should == -100
      Date.valid_jd?(0).should    ==    0
      Date.valid_jd?(100).should  ==  100
    end

  end
end

ruby_version_is "1.9" do
  describe "Date#valid_jd?" do

    it "should be able to determine if a day number is a valid Julian day number, true for all numbers" do
      # This might need to check the type of the jd parameter. Date.valid_jd?(:number) is of course
      # bogus but returns itself with the current implementation
      Date.valid_jd?(-100).should == true
      Date.valid_jd?(0).should    == true
      Date.valid_jd?(100).should  == true
    end

  end
end