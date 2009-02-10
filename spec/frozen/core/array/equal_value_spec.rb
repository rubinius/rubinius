require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#==" do
  it "return true if the other is self" do
    a = [1, 2, 3]
    a.should == a
  end

  it "returns true if corresponding elements are #==" do
    [].should == []
    ["a", "c", 7].should == ["a", "c", 7]

    [1, 2, 3].should == [1.0, 2.0, 3.0]

    obj = mock('5')
    obj.should_receive(:==).and_return(true)
    [obj].should == [5]
  end

  it "returns false if the other is shorter than self" do
    a = [1, 2, 3]
    b = [1, 2]
    a.should_not == b
  end

  it "returns false if the other is longer than self" do
    a = [1, 2, 3]
    b = [1, 2, 3, 4]
    a.should_not == b
  end

  it "returns false if any corresponding elements are not #==" do
    a = ["a", "b", "c"]
    b = ["a", "b", "not equal value"]
    a.should_not == b

    c = mock("c")
    c.should_receive(:==).and_return(false)
    ["a", "b", c].should_not == a
  end

  it "returns false immediately when sizes of the arrays differ" do
    obj = mock('1')
    obj.should_not_receive(:==)

    [].should_not == [obj]
    [obj].should_not == []
  end

  ruby_bug "#", "1.8.6.277" do
    it "properly handles recursive arrays" do
      empty = ArraySpecs.empty_recursive_array
      empty2 = []; empty2 << empty2
      (empty == empty.dup).should be_true
      (empty == empty2).should be_false

      array = ArraySpecs.recursive_array
      (array == array).should be_true
      (array == array.dup).should be_true
      (array == empty).should be_false
      (array == [1, 2]).should be_false
    end
  end

  it "does not call #to_ary on its argument" do
    obj = mock('to_ary')
    obj.should_not_receive(:to_ary)

    [1, 2, 3].should_not == obj
  end

  it "does not call #to_ary on Array subclasses" do
    ary = ArraySpecs::ToAryArray[5, 6, 7]
    ary.should_not_receive(:to_ary)
    [5, 6, 7].should == ary
  end

  it "ignores array class differences" do
    ArraySpecs::MyArray[1, 2, 3].should == [1, 2, 3]
    ArraySpecs::MyArray[1, 2, 3].should == ArraySpecs::MyArray[1, 2, 3]
    [1, 2, 3].should == ArraySpecs::MyArray[1, 2, 3]
  end

  it "does not call #== on its argument" do
    bad_array = ArraySpecs::MyArray[1, 2, 3]
    bad_array.should_not_receive(:==)

    [1, 2, 3].should == bad_array
  end
end
