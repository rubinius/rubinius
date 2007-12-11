require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel.time" do
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
      # MRI does not allow explicit sleep of nil
      sleep
      5
    end
    th.run
    th.value.should == 5
  end

  it "should return the time spent sleeping" do
    th = Thread.new { sleep }
    th.run
    (th.value < 0.01).should == true
  end

  it "should return the time spent sleeping even if less then requested sleep time" do
    th = Thread.new { sleep(0.5) }
    th.run
    (th.value < 0.01).should == true
  end
end
