require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#index" do
  it "returns the index of the first element == to object" do
    x = mock('3')
    def x.==(obj) 3 == obj; end

    [2, x, 3, 1, 3, 1].index(3).should == 1
    [2, 3.0, 3, x, 1, 3, 1].index(x).should == 1
  end

  it "returns 0 if first element == to object" do
    [2, 1, 3, 2, 5].index(2).should == 0
  end

  it "returns size-1 if only last element == to object" do
    [2, 1, 3, 1, 5].index(5).should == 4
  end

  it "returns nil if no element == to object" do
    [2, 1, 1, 1, 1].index(3).should == nil
  end
end
