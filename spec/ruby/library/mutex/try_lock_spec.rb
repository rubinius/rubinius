require File.expand_path('../../../spec_helper', __FILE__)
require 'thread'

describe "Mutex#try_lock" do
  it "locks the mutex if it can" do
    m = Mutex.new
    m.try_lock

    m.locked?.should be_true
    lambda { m.try_lock.should be_false }.should_not raise_error(ThreadError)
  end

  it "returns false if lock can not be aquired immediately" do
    m1 = Mutex.new
    m2 = Mutex.new

    m2.lock
    th = Thread.new do
      m1.lock
      m2.lock
    end

    Thread.pass while th.status and th.status != "sleep"

    # th owns m1 so try_lock should return false
    m1.try_lock.should be_false
    m2.unlock
    th.join
    # once th is finished m1 should be released
    m1.try_lock.should be_true
  end
end
