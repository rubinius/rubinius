require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Thread#wakeup" do
  it "wakes up a sleeping thread" do
    a = 0
    t = Thread.new do
      a = 1
      Thread.stop
      a = 2
    end

    Thread.pass until t.status == "sleep" || !t.alive?
    a.should == 1

    t.wakeup
    t.join
    a.should == 2
  end
end
