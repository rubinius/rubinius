require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

# Do not use #should_receive(:eql?) mocks in these specs
# because MSpec uses Hash for mocks and Hash calls #eql?.

describe "Array#eql?" do
  it "returns true if other is the same array" do
    a = [1]
    a.should eql(a)
  end

  it "returns true if corresponding elements are #eql?" do
    [].should eql([])
    [1, 2, 3, 4].should eql([1, 2, 3, 4])

    obj = mock("eql? to anything")
    def obj.eql?(o) true end
    [obj].should eql([1])
  end

  it "returns false if other is shorter than self" do
    [1, 2, 3, 4].should_not eql([1, 2, 3])
  end

  it "returns false if other is longer than self" do
    [1, 2, 3, 4].should_not eql([1, 2, 3, 4, 5])
  end

  it "returns false if any corresponding elements are not #eql?" do
    [1, 2, 3, 4].should_not eql([1, 2, 3, 4.0])

    obj = mock('what is eql to any object')
    def obj.eql?(o) false end

    [1, 2, obj].should_not eql([1, 2, obj])
  end

  ruby_bug "#", "1.8.6.277" do
    it "properly handles recursive arrays" do
      empty = ArraySpecs.empty_recursive_array
      empty2 = []; empty2 << empty2
      empty.should eql(empty.dup)
      empty.should_not eql(empty2)

      array = ArraySpecs.recursive_array
      array.should eql(array)
      array.should eql(array.dup)

      array.should_not eql(empty)
      array.should_not eql([1, 2])
    end
  end

  it "does not call #to_ary on its argument" do
    obj = mock('to_ary')
    obj.should_not_receive(:to_ary)

    [1, 2, 3].should_not eql(obj)
  end

  it "does not call #to_ary on Array subclasses" do
    ary = ArraySpecs::ToAryArray[5, 6, 7]
    ary.should_not_receive(:to_ary)
    [5, 6, 7].should eql(ary)
  end

  it "ignores array class differences" do
    ArraySpecs::MyArray[1, 2, 3].should eql([1, 2, 3])
    ArraySpecs::MyArray[1, 2, 3].should eql(ArraySpecs::MyArray[1, 2, 3])
    [1, 2, 3].should eql(ArraySpecs::MyArray[1, 2, 3])
  end
end
