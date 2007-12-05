require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#rindex" do
  it "returns the first index backwards from the end where element == to object" do
    key = 3    
    ary = Array.new(3) { Object.new }
    ary[2].should_receive(:==).with(key).and_return(false)
    ary[1].should_receive(:==).with(key).and_return(true)
    ary[0].should_not_receive(:==)

    ary.rindex(key).should == 1
  end

  it "returns size-1 if last element == to object" do
    [2, 1, 3, 2, 5].rindex(5).should == 4
  end

  it "returns 0 if only first element == to object" do
    [2, 1, 3, 1, 5].rindex(2).should == 0
  end

  it "returns nil if no element == to object" do
    [1, 1, 3, 2, 1, 3].rindex(4).should == nil
  end

  it "does not fail when removing elements from block" do
    sentinel = Object.new
    ary = [0, 0, 1, 1, 3, 2, 1, sentinel]

    sentinel.instance_variable_set(:@ary, ary)
    def sentinel.==(o) @ary.slice!(1..-1); false; end
    
    ary.rindex(0).should == 0
  end
end
