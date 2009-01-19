require File.dirname(__FILE__) + '/../../spec_helper'
require 'thread'

describe "ConditionVariable#signal" do
  it "should return self if nothing to signal" do
    cv = ConditionVariable.new
    cv.signal.should == cv
  end

  it "should return self if something is waiting for signal" do
    m = Mutex.new
    cv = ConditionVariable.new
    th = Thread.new do
      m.synchronize do
        cv.wait(m)
      end
    end

    cv.signal.should == cv
  end

  it "wakes up the first thread waiting in line for this resource" do
    m = Mutex.new
    cv = ConditionVariable.new
    threads = []
    r1 = []
    r2 = []

    # large number to attempt to cause race conditions
    100.times do |i|
      threads << Thread.new(i) do |tid|
        r1 << tid
        m.synchronize do
          cv.wait(m)
          r2 << tid
        end
      end
    end

    r1.size.should == threads.size
    r2.should be_empty
    threads.each { cv.signal }
    threads.each {|t| t.join }
    r2.should == r1
  end
end
