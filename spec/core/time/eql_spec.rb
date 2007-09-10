require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'

describe "Time#eql?" do
  it "returns true iff time is equal in seconds and usecs to other time" do
    Time.at(100, 100).eql?(Time.at(100, 100)).should == true
    Time.at(100, 100).eql?(Time.at(100, 99)).should == false
    Time.at(100, 100).eql?(Time.at(99, 100)).should == false
  end  
end
