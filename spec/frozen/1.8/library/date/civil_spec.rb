require 'date' 
require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/civil'

describe "Date#civil" do

  it_behaves_like(:date_civil, :civil)

end

describe "Date#valid_civil?" do

  it "should be able to determine if a date is valid" do
    Date.valid_civil?(1582, 10, 14).should == nil
    Date.valid_civil?(1582, 10, 15).should == Date.civil(1582, 10, 15).jd
    Date.valid_civil?(1582, 10, 14, Date::ENGLAND).should_not == nil
    Date.valid_civil?(1582, 10, 14, Date::ENGLAND).should == Date.civil(1582, 10, 14, Date::ENGLAND).jd
  end
  
  it "should be able to handle negative months and days" do
    Date.valid_civil?(1582, -3, -18).should == nil
    Date.valid_civil?(1582, -3, -17).should == Date.civil(1582, 10, 15).jd
    
    Date.valid_civil?(2007, -11, -10).should == Date.civil(2007, 2, 19).jd
    Date.valid_civil?(2008, -11, -10).should == Date.civil(2008, 2, 20).jd
  end

end
