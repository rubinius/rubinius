require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/eql'

describe "Array#==" do
  it_behaves_like :array_eql, :==

  it "returns false if any corresponding elements are not #==" do
    a = ["a", "b", "c"]
    b = ["a", "b", "not equal value"]
    a.should_not == b

    c = mock("c")
    c.should_receive(:==).and_return(false)
    ["a", "b", c].should_not == a
  end

  it "returns true if corresponding elements are #==" do
    [].should == []
    ["a", "c", 7].should == ["a", "c", 7]

    [1, 2, 3].should == [1.0, 2.0, 3.0]

    obj = mock('5')
    obj.should_receive(:==).and_return(true)
    [obj].should == [5]
  end

  # As per bug #1720
  it "returns false for [NaN] == [NaN]" do
    [nan_value].should_not == [nan_value]
  end
end
