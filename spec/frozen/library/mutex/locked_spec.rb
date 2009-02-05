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

    m2.lock # hold th with only m1 locked

    th = Thread.new do
      m1.lock
      m2.lock
    end

    Thread.pass until th.status == "sleep"

    m1.locked?.should be_true
    m2.unlock # release th
    th.join
    m1.locked?.should be_false
  end
end
