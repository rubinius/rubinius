require File.dirname(__FILE__) + '/../../spec_helper'
require 'thread'

describe "Mutex#unlock" do
  it "raises ThreadError unless Mutex is locked" do
    mutex = Mutex.new
    lambda { mutex.unlock }.should raise_error(ThreadError)
  end

  it "raises ThreadError unless thread owns Mutex" do
    mutex = Mutex.new
    wait = Mutex.new
    wait.lock
    th = Thread.new do
      mutex.lock
      wait.lock
    end

    # avoid race on mutex.lock
    Thread.pass while th.status and th.status != "sleep"

    lambda { mutex.unlock }.should raise_error(ThreadError)

    wait.unlock
    th.join
  end

  it "returns nil if successful" do
    mutex = Mutex.new
    mutex.lock
    mutex.unlock.should.be_nil
  end
end
