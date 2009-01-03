require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'

describe "Time.at" do
  it "converts to time object" do
    # the #chomp calls are necessary because of RSpec
    Time.at(1168475924).inspect.chomp.should == localtime(1168475924).chomp
  end
  
  it "creates a new time object with the value given by time" do
    t = Time.now
    Time.at(t).inspect.should == t.inspect
  end
  
  it "creates a dup time object with the value given by time" do
    t1 = Time.new
    t2 = Time.at(t1)
    t1.object_id.should_not == t2.object_id
  end
  
  it "is able to create a time object with a float" do
    t = Time.at(10.5)
    t.usec.should == 500000.0
    t.should_not == Time.at(10)
  end

  it "is able to create a time object with a microseconds" do
    t = Time.at(10, 500000)
    t.usec.should == 500000.0
    t.should_not == Time.at(10)
  end

end
