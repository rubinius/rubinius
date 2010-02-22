require 'date' 
require File.expand_path('../../../spec_helper', __FILE__)

describe "Date#gregorian?" do

  it "should mark a day before the calendar reform as Julian" do
    Date.civil(1007, 2, 27).gregorian?.should == false
    Date.civil(1907, 2, 27, Date.civil(2000, 1, 1).jd).gregorian?.should == false
  end
  
  it "should mark a day after the calendar reform as Julian" do
    Date.civil(2007, 2, 27).gregorian?.should == true
    Date.civil(1007, 2, 27, Date.civil(1000, 1, 1).jd).gregorian?.should == true
  end
  
end

describe "Date#gregorian_leap?" do

  it "should be able to determine whether a year is a leap year in the Gregorian calendar" do
    Date.gregorian_leap?(1900).should == false
    Date.gregorian_leap?(1999).should == false
    Date.gregorian_leap?(2000).should == true
    Date.gregorian_leap?(2002).should == false
    Date.gregorian_leap?(2004).should == true
  end

end