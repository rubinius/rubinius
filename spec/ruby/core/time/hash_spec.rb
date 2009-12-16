require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'

describe "Time#hash" do
  it "returns a Fixnum" do
    Time.at(100).hash.should be_kind_of(Fixnum)
  end  
  
  it "is stable" do
    Time.at(1234).hash.should == Time.at(1234).hash
  end  
end
