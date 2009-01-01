require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Thread#alive?" do
  it "returns true as long as the thread is alive" do
    c = Channel.new
    t = Thread.new { c.receive }
    begin
      t.alive?.should == true
    ensure
      c << nil
    end
  end

  it "returns false when the thread is finished" do
    t = Thread.new { }
    t.join
    t.alive?.should == false
  end
end
