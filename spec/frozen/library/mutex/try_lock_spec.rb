require File.dirname(__FILE__) + '/../../spec_helper'
require 'thread'

describe "Mutex#try_lock" do
  it "should return true if lock can be aquired immediately" do
    m = Mutex.new
    m.try_lock.should be_true
  end

  it "should return false if lock can not be aquired immediately" do
    m1 = Mutex.new
    m2 = Mutex.new

    m2.lock
    th = Thread.new do
      m1.lock
      m2.lock
    end

    Thread.pass until th.status == "sleep"

    # th owns m1 so try_lock should return false
    m1.try_lock.should be_false
    m2.unlock
    th.join
    # once th is finished m1 should be released
    m1.try_lock.should be_true
  end
end
