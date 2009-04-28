require File.dirname(__FILE__) + '/../../spec_helper'
require 'thread'

describe "ConditionVariable#wait" do
  it "should return self" do
    m = Mutex.new
    cv = ConditionVariable.new

    th = Thread.new do
      m.synchronize do
        cv.wait(m).should == cv
      end
    end

    # ensures that th grabs m before current thread
    Thread.pass while th.status and th.status != "sleep"

    m.synchronize { cv.signal }
    th.join
  end
end
