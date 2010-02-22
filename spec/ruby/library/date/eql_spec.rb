require 'date' 
require File.expand_path('../../../spec_helper', __FILE__)

describe "Date#eql?" do
  it "should be able determine equality between date objects" do
    Date.civil(2007, 10, 11).should eql(Date.civil(2007, 10, 11))
    Date.civil(2007, 10, 11).should eql(Date.civil(2007, 10, 12) - 1)
    Date.civil(2007, 10, 11).should_not eql(Date.civil(2007, 10, 12))
  end
end