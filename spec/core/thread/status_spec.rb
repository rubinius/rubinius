require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Thread#status" do
  it "reports the thread's current state" do
    v = nil
    t = Thread.new {
      v = Thread.current.status
    }
    t.join
    v.should == 'run'
    t.status.should == false
  end
end
