require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Thread#keys" do
  it "returns an array of the names of the thread-local variables as symbols" do
    th = Thread.new do
      Thread.current["cat"] = 'woof'
      Thread.current[:cat] = 'meow'
      Thread.current[:dog] = 'woof'
    end
    th.join
    th.keys.sort_by {|x| x.to_s}.should == [:cat,:dog]
  end
end
