require File.dirname(__FILE__) + '/../../spec_helper'

describe "Process.times" do
  it "returns a Struct::Tms" do
    Process::times.class.should == Struct::Tms
  end

  it "returns current cpu times" do

    t = Process::times

    # Stall for 1 second, but do work instead of sleeping
    # because process times won't increase.
    start = Time.now
    1 until (Time.now - start) > 1.2

    # Ensure times is larger. NOTE that there is no 
    # guarantee of an upper bound since anything may be 
    # happening at the OS level, so we ONLY check that at 
    # least an interval has elapsed. Also, we are assuming 
    # there is a correlation between wall clock time and 
    # process time. In practice, there is an observed 
    # discrepancy often 10% or greater. In other words, 
    # this is a very fuzzy test.
    t2 = Process::times
    diff = (t2.utime + t2.stime) - (t.utime + t.stime)
    diff.should > 0
  end
end
