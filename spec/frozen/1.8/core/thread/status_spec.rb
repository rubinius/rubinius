require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Thread#status" do
  it "reports threads as running and returns false on correct termination" do
    t = Thread.new { Thread.current.status }
    t.value.should == 'run'
    t.status.should == false
  end

  it "returns nil if thread terminates with exception" do
    t = Thread.new { raise "death to the unbelievers" }
    lambda { t.join }.should raise_error(StandardError)
    t.status.should == nil
  end
end
