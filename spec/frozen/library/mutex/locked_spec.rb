require File.dirname(__FILE__) + '/../../spec_helper'
require 'thread'

describe "Mutex#locked?" do
  it "should return true if locked" do
    m = Mutex.new
    m.lock
    m.locked?.should be_true
  end

  it "should return false if unlocked" do
    m = Mutex.new
    m.locked?.should be_false
  end

  it "should return the status of the lock" do
    m1 = Mutex.new
    m2 = Mutex.new
    m2.lock

    t1 = Thread.new do
      m1.lock
      m2.lock
    end

    m1.locked?.should be_true
    m2.unlock
    t1.join
    m1.locked?.should be_false
  end
end
