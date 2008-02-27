require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#eql?" do
  it "returns true if other is the same array" do
    a, b = [1], [2]

    a.eql?(b).should == false
    a.eql?(a).should == true
  end
  
  it "returns true if other has the same length and elements" do
    a = [1, 2, 3, 4]
    b = [1, 2, 3, 4]
    c = [1, 2]
    d = ['a', 'b', 'c', 'd']

    a.eql?(b).should == true
    a.eql?(c).should == false
    a.eql?(d).should == false
    [].eql?([]).should == true
  end

  it "ignores array class differences" do
    ArraySpecs::MyArray[1, 2, 3].eql?([1, 2, 3]).should == true
    ArraySpecs::MyArray[1, 2, 3].eql?(ArraySpecs::MyArray[1, 2, 3]).should == true
    [1, 2, 3].eql?(ArraySpecs::MyArray[1, 2, 3]).should == true
  end
end
