require File.dirname(__FILE__) + '/../spec_helper'

def block_spec_method(*args)
  yield(*args)
end

describe "A block with a 'rest' arg" do
  it "should collect all of the yielded arguments" do
    ret = nil
    block_spec_method(1,2,3) {|*args| ret = args}
    ret.should == [1,2,3]
  end
end

describe "A block with an anonymous 'rest' arg" do
  it "should ignore all of the yielded arguments" do
    ret = [1].each {|*| }
    ret.should == [1]
  end
end

