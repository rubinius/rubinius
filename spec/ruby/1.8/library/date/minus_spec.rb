require 'date' 
require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/parse'
require File.dirname(__FILE__) + '/shared/parse_us'
require File.dirname(__FILE__) + '/shared/parse_eu'

describe "Date#-" do


  # The space separator is also different, doesn't work for only numbers
  it "substract a number of days from a Date" do
    d = Date.civil(2007, 5 ,2) - 13
    d.should == Date.civil(2007, 4, 19)
  end
  
  # The space separator is also different, doesn't work for only numbers
  it "substract a negative number of days from a Date" do
    d = Date.civil(2007, 4, 19).-(-13)
    d.should == Date.civil(2007, 5 ,2) 
  end

  it "should be able to compute the different between two dates" do
    (Date.civil(2007,2,27) - Date.civil(2007,2,27)).should == 0
    (Date.civil(2007,2,27) - Date.civil(2007,2,26)).should == 1
    (Date.civil(2006,2,27) - Date.civil(2007,2,27)).should == -365
    (Date.civil(2008,2,27) - Date.civil(2007,2,27)).should == 365
    
  end

  it "shoud raise an error on non numeric parameters" do
    lambda { Date.civil(2007,2,27) - :hello }.should raise_error(TypeError)
    lambda { Date.civil(2007,2,27) - "hello" }.should raise_error(TypeError)
    lambda { Date.civil(2007,2,27) - Object.new }.should raise_error(TypeError)
  end
  
end