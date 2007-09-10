require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Thread.new" do
  it "creates a thread executing the given block" do
    c = Channel.new
    Thread.new { c << true }.join
    c << false
    c.receive.should == true
  end
end
