require File.dirname(__FILE__) + '/../../spec_helper'
require 'thread'

describe "Mutex#unlock" do
  it "should raise ThreadError unless Mutex is locked" do
    mutex = Mutex.new
    lambda { mutex.unlock }.should raise_error(ThreadError)
  end

  it "should raise ThreadError unless thread owns Mutex" do
    mutex = Mutex.new
    t1 = Thread.new do
      mutex.lock
    end
    t1.join
    lambda { mutex.unlock }.should raise_error(ThreadError)
  end

  it "should return nil if successful" do
    mutex = Mutex.new
    mutex.lock
    mutex.unlock.should.be_nil
  end
end
