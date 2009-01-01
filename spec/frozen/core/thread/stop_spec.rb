require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Thread.stop" do
  it "causes the current thread to sleep indefinitely" do
    t = Thread.new { Thread.stop; 5 }
    Thread.pass until t.status == 'sleep'
    t.status.should == 'sleep'
    t.run
    t.value.should == 5
  end

  it "resets Thread.critical to false" do
    t = Thread.new { Thread.critical = true; Thread.stop }
    Thread.pass until t.status == 'sleep'
    Thread.critical.should == false
    t.run
    t.join
  end
end

describe "Thread#stop?" do
  it "reports if a thread has stopped due to sleeping" do
    t = Thread.new { Thread.stop }
    Thread.pass until t.status == 'sleep'
    t.stop?.should == true
    t.run
    t.join
    t.stop?.should == true
  end
end
