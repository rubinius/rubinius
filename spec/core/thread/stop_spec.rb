require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Thread.stop" do
  it "should cause the current thread to sleep indefinitely" do
    c = Channel.new
    t = Thread.new { c << true; Thread.stop }
    c.receive
    t.status.should == 'sleep'
    t.run
    t.value.should == nil
  end

  # FIXME: test to make sure that Thread.critical is set to false
end
