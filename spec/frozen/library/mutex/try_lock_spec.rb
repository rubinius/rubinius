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
    t1 = Thread.new do
      m1.lock
      m2.lock
    end

    m1.try_lock.should be_false
    m2.unlock
    t1.join
    m1.try_lock.should be_true
  end
end
