require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#<<" do
  it "pushes the object onto the end of the array" do
    ([ 1, 2 ] << "c" << "d" << [ 3, 4 ]).should == [1, 2, "c", "d", [3, 4]]
  end

  it "returns self to allow chaining" do
    a = []
    b = a
    (a << 1).equal?(b).should == true
    (a << 2 << 3).equal?(b).should == true
  end

  it "correctly resizes the Array" do
    a = []
    a.size.should == 0
    a << :foo
    a.size.should == 1
    a << :bar << :baz
    a.size.should == 3
  end  

  compliant :mri do
    it "raises TypeError on a frozen array" do
      should_raise(TypeError) { @frozen_array << 5 }
    end
  end
end
