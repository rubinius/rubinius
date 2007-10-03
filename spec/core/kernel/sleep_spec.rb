require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel.time" do
  it "should wait 0.1 second" do
    t0 = Time.now
    sleep 0.1
    t1 = Time.now
    diff = (t1 - t0 - 0.1).abs
    (diff < 0.01).should == true

    # this is a base case -- it doesn't check anything concerning
    # sleep calls in interrupted threads
  end
end
