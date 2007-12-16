require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#sleep" do
  it "pauses execution for approximately the duration requested" do
    start = Time.now
    sleep 0.1
    (Time.now - start).should be_close(0.1, 0.1)
  end
  
  it "returns the rounded number of seconds asleep" do
    sleep(0.1).should be_kind_of(Integer)
  end
  
  it "raises a TypeError when passed a non-numeric duration" do
    lambda { sleep(nil)   }.should raise_error(TypeError)
    lambda { sleep('now') }.should raise_error(TypeError)
    lambda { sleep('2')   }.should raise_error(TypeError)
  end
  
  it "pauses execution indefinitely if not given a duration" do
    flag = nil
    t = Thread.new do
      sleep
      flag = 5
    end
    
    flag.should == nil
    Thread.pass until t.status == 'sleep'
    flag.should == nil
    t.run
    flag.should == 5
  end
end
