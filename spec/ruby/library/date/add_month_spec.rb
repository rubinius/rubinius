require 'date' 
require File.expand_path('../../../spec_helper', __FILE__)

describe "Date#>>" do

  it "should add a number of months to a Date" do
    d = Date.civil(2007,2,27) >> 10
    d.should == Date.civil(2007, 12, 27)
  end


  it "should result in the last day of a month if the day doesn't exist" do
    d = Date.civil(2008,3,31) >> 1
    d.should == Date.civil(2008, 4, 30)
  end

  it "should raise an error on non numeric parameters" do
    lambda { Date.civil(2007,2,27) >> :hello }.should raise_error(TypeError)
    lambda { Date.civil(2007,2,27) >> "hello" }.should raise_error(TypeError)
    lambda { Date.civil(2007,2,27) >> Date.new }.should raise_error(TypeError)
    lambda { Date.civil(2007,2,27) >> Object.new }.should raise_error(TypeError)
  end
  
end