require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Thread.new" do
  it "creates a thread executing the given block" do
    c = Channel.new
    Thread.new { c << true }.join
    c << false
    c.receive.should == true
  end
  
  it "can pass arguments to the thread block" do
    arr = []
    a, b, c = 1, 2, 3
    t = Thread.new(a,b,c) {|d,e,f| arr << d << e << f }
    t.join
    arr.should == [a,b,c]
  end
end
