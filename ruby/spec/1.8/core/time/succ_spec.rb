require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'

describe "Time#succ" do
  it "returns a new time one second later than time" do
    Time.at(100).succ.should == Time.at(101)
  end
  
  it "returns a new instance" do
    t1 = Time.at(100)
    t2 = t1.succ
    t1.object_id.should_not == t2.object_id
  end
end
