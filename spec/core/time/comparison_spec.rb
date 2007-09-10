require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'

describe "Time#<=>" do
  it "returns 1 if the first argument is a point in time after the second argument" do
    (Time.now <=> Time.at(0)).should == 1
    (Time.at(0, 100) <=> Time.at(0, 0)).should == 1
  end
  
  it "returns 0 if time is the same as other" do
    (Time.at(100, 100) <=> Time.at(100, 100)).should == 0
    (Time.at(100, 100) <=> Time.at(100, 99)).should == 1
    (Time.at(100, 100) <=> Time.at(101, 100)).should == -1
  end
  
  it "returns -1 if the first argument is a point in time before the second argument" do
    (Time.at(0) <=> Time.now).should == -1
    (Time.at(0, 0) <=> Time.at(0, 100)).should == -1
  end  
end
