require File.dirname(__FILE__) + '/../../../spec_helper'

describe "Hash::Iterator#reset" do
  it "resets the next index to the beginning" do
    a = [1, 2, 3, nil, 4, nil, 5, nil, nil, nil, 6]
    iter = Hash::Iterator.new a, a.size

    iter.next.should == 1
    iter.next.should == 2
    iter.next.should == 3

    iter.reset
    iter.next.should == 1
  end
end
