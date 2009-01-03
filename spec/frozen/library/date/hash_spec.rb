require 'date' 
require File.dirname(__FILE__) + '/../../spec_helper'

describe "Date#hash" do
  
  it "should be able determine the hash value for a date" do
    Date.civil(2004, 7, 12).respond_to?(:hash).should == true
  end
  
  it "should be the case that the same date results in the same hash" do 
    Date.civil(2004, 7, 12).hash.should == Date.civil(2004, 7, 12).hash
  end
  
end