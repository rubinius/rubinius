require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#eql?" do
  it "returns true if other is the same array" do
    a, b = [1], [2]

    a.should_not eql(b)
    a.should eql(a)
  end
  
  it "returns true if other has the same length and elements" do
    a = [1, 2, 3, 4]
    b = [1, 2, 3, 4]
    c = [1, 2]
    d = ['a', 'b', 'c', 'd']

    a.should eql(b)
    a.should_not eql(c)
    a.should_not eql(d)
    [].should eql([])
  end

  it "ignores array class differences" do
    ArraySpecs::MyArray[1, 2, 3].should eql([1, 2, 3])
    ArraySpecs::MyArray[1, 2, 3].should eql(ArraySpecs::MyArray[1, 2, 3])
    [1, 2, 3].should eql(ArraySpecs::MyArray[1, 2, 3])
  end
end
