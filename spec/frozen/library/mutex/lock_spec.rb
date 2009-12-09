require File.dirname(__FILE__) + '/../../spec_helper'
require 'thread'

describe "Mutex#lock" do
  it "returns self" do
    m = Mutex.new
    m.lock.should == m
    m.unlock
  end

  it "waits if the lock is not available" do
    m = Mutex.new

    m.lock
    v = 0

    th = Thread.new do
      m.lock
      v = 1
    end

    Thread.pass while th.status and th.status != "sleep"

    v.should == 0
    m.unlock
    th.join
    v.should == 1
  end

  it "raises ThreadError if the current thread already locks this monitor" do
    mutex = Mutex.new
    mutex.lock
    lambda { mutex.lock }.should raise_error(ThreadError)
  end
end
