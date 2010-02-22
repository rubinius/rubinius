require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

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
