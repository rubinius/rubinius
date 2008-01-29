require 'date' 
require File.dirname(__FILE__) + '/../../spec_helper'

describe "Date#eql?" do
  
  it "should be able determine equality between date objects" do
    
    Date.civil(2007, 10, 11).eql?(Date.civil(2007, 10, 11)).should     == true
    Date.civil(2007, 10, 11).eql?(Date.civil(2007, 10, 12)).should_not == true

    Date.civil(2007, 10, 11).eql?(Date.civil(2007, 10, 12) - 1).should == true
    
  end
  
end