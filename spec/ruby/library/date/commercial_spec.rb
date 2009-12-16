require 'date' 
require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/commercial'

describe "Date#commercial" do

  it_behaves_like(:date_commercial, :commercial)

end

describe "Date#valid_commercial?" do

  it "should be able to determine if the date is a valid commercial date" do
    Date.valid_commercial?(1582, 41, 4).should == nil
    Date.valid_commercial?(1582, 41, 5).should == Date.civil(1582, 10, 15).jd
    # valid_commercial? can't handle dates before the Gregorian calendar
    Date.valid_commercial?(1582, 41, 4, Date::ENGLAND).should == nil
    Date.valid_commercial?(1752, 37, 4, Date::ENGLAND).should == Date.civil(1752, 9, 14, Date::ENGLAND).jd
  end

  it "should be able to handle negative week and day numbers" do
    Date.valid_commercial?(1582, -12, -4).should == nil
    Date.valid_commercial?(1582, -12, -3).should == Date.civil(1582, 10, 15).jd
    
    Date.valid_commercial?(2007, -44, -2).should == Date.civil(2007, 3, 3).jd
    Date.valid_commercial?(2008, -44, -2).should == Date.civil(2008, 3, 1).jd
  end

end