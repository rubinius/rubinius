require File.dirname(__FILE__) + '/../../spec_helper'
require 'thread'

describe "ConditionVariable#broadcast" do
  it "should return self if nothing to broadcast to" do
    cv = ConditionVariable.new
    cv.broadcast.should == cv
  end

  it "should return self if something is waiting for a broadcast" do
    m = Mutex.new
    cv = ConditionVariable.new
    th = Thread.new do
      m.synchronize do
        cv.wait(m)
      end
    end

    cv.broadcast.should == cv
  end

  it "wakes up all threads waiting in line for this resource" do
    m = Mutex.new
    cv = ConditionVariable.new
    threads = []
    r = []

    # large number to attempt to cause race conditions
    100.times do |i|
      threads << Thread.new(i) do |tid|
        m.synchronize do
          cv.wait(m)
          r << tid
        end
      end
    end

    r.should == []
    threads.each { cv.broadcast }
    threads.each {|t| t.join }
    r.size.should == threads.size
  end
end
