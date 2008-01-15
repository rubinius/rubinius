require 'date' 
require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/parse'
require File.dirname(__FILE__) + '/shared/parse_us'
require File.dirname(__FILE__) + '/shared/parse_eu'

describe "Date#+" do


  # The space separator is also different, doesn't work for only numbers
  it "add a number of days to a Date" do
    d = Date.civil(2007,2,27) + 10
    d.should == Date.civil(2007, 3, 9)
  end
  
  # The space separator is also different, doesn't work for only numbers
  it "add a negative number of days to a Date" do
    d = Date.civil(2007,2,27).+(-10)
    d.should == Date.civil(2007, 2, 17)
  end

  it "shoud raise an error on non numeric parameters" do
    lambda { Date.civil(2007,2,27) + :hello }.should raise_error(TypeError)
    lambda { Date.civil(2007,2,27) + "hello" }.should raise_error(TypeError)
    lambda { Date.civil(2007,2,27) + Date.new }.should raise_error(TypeError)
    lambda { Date.civil(2007,2,27) + Object.new }.should raise_error(TypeError)
  end
  
end