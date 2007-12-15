require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel.sleep" do
  it "should wait 0.1 second" do
    t0 = Time.now
    sleep 0.1
    t1 = Time.now
    diff = (t1 - t0 - 0.1).abs
    (diff < 0.01).should == true

    # this is a base case -- it doesn't check anything concerning
    # sleep calls in interrupted threads
  end  
end

describe "Thread.sleep" do
  it "should wait for indefinite seconds" do
    th = Thread.new do
      sleep
      5
    end
    
    Thread.pass while th.status == 'run'
    th.run
    th.value.should == 5
  end

  it "should return immediately if sleep(0.0)" do
    th = Thread.new { sleep(0.0) }
    (th.value == 0.0).should == true
  end
  
  it "should return the time spent sleeping" do
    th = Thread.new { sleep }
    
    Thread.pass while th.status == 'run'
    th.run
    (th.value < 0.01).should == true
  end

  it "should return the time spent sleeping even if less then requested sleep time" do
    th = Thread.new { sleep(0.5) }
    
    Thread.pass while th.status == 'run'
    th.run
    (th.value < 0.01).should == true
  end  
  
  it "should be possible to see if a thread is alive? while sleeping" do
    th = Thread.new { sleep }
    
    Thread.pass while th.status == 'run'
    th.alive?.should == true
    th.run
    th.alive?.should == false
  end
  
  noncompliant(:rubinius) do
    it "should allow sleep of duration nil" do
      th = Thread.new { sleep(nil) }
      
      Thread.pass while th.status == 'run'
      th.run
      (th.value < 0.01).should == true
    end
  end
end
