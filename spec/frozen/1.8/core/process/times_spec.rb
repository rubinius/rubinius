require File.dirname(__FILE__) + '/../../spec_helper'

describe "Process.times" do
  it "returns current cpu times" do

    t = Process::times
    t.class.should == Struct::Tms

    # stall for 1 second, but do work instead of sleeping because process
    # times won't increase
    start = Time.now
    1 while (Time.now - start) < 1.1

    # ensure times is at least one second larger
    t2 = Process::times
    diff = (t2.utime + t2.stime) - (t.utime + t.stime)
    (diff > 1).should == true
  end
end
