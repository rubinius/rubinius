require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#reverse" do
  it "returns a new array with the elements in reverse order" do
    [].reverse.should == []
    [1, 3, 5, 2].reverse.should == [2, 5, 3, 1]
  end

  it "returns subclass instance on Array subclasses" do
    ArraySpecs::MyArray[1, 2, 3].reverse.class.should == ArraySpecs::MyArray
  end
end

describe "Array#reverse!" do
  it "reverses the elements in place" do
    a = [6, 3, 4, 2, 1]
    a.reverse!.should equal(a)
    a.should == [1, 2, 4, 3, 6]
    [].reverse!.should == []
  end

  compliant_on :ruby, :jruby do
    it "raises a TypeError on a frozen array" do
      lambda { ArraySpecs.frozen_array.reverse! }.should raise_error(TypeError)
    end
  end
end
