require File.dirname(__FILE__) + '/../../spec_helper'
require 'thread'

describe "Mutex#synchronize" do
  it "wraps the lock/unlock pair in an ensure" do
    m1 = Mutex.new
    m2 = Mutex.new
    m2.lock

    t1 = Thread.new do
      lambda do
        m1.synchronize do
          m2.lock
          raise Exception
        end
      end.should raise_error(Exception)
    end

    m1.locked?.should be_true
    m2.unlock
    t1.join
    m1.locked?.should be_false
  end
end
