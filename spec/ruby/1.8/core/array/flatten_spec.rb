require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#flatten" do
  it "returns a one-dimensional flattening recursively" do
    [[[1, [2, 3]],[2, 3, [4, [4, [5, 5]], [1, 2, 3]]], [4]], []].flatten.should == [1, 2, 3, 2, 3, 4, 4, 5, 5, 1, 2, 3, 4]
  end

  it "does not call flatten on elements" do
    obj = mock('[1,2]')
    def obj.flatten() [1, 2] end
    [obj, obj].flatten.should == [obj, obj]

    obj = [5, 4]
    def obj.flatten() [1, 2] end
    [obj, obj].flatten.should == [5, 4, 5, 4]
  end
  
  it "raises an ArgumentError on recursive arrays" do
    x = []
    x << x
    lambda { x.flatten }.should raise_error(ArgumentError)
  
    x = []
    y = []
    x << y
    y << x
    lambda { x.flatten }.should raise_error(ArgumentError)
  end
  
  it "returns subclass instance for Array subclasses" do
    ArraySpecs::MyArray[].flatten.class.should == ArraySpecs::MyArray
    ArraySpecs::MyArray[1, 2, 3].flatten.class.should == ArraySpecs::MyArray
    ArraySpecs::MyArray[1, [2], 3].flatten.class.should == ArraySpecs::MyArray
    [ArraySpecs::MyArray[1, 2, 3]].flatten.class.should == Array
  end
end  

describe "Array#flatten!" do
  it "modifies array to produce a one-dimensional flattening recursively" do
    a = [[[1, [2, 3]],[2, 3, [4, [4, [5, 5]], [1, 2, 3]]], [4]], []]
    a.flatten!.equal?(a).should == true
    a.should == [1, 2, 3, 2, 3, 4, 4, 5, 5, 1, 2, 3, 4]
  end

  it "returns nil if no modifications took place" do
    a = [1, 2, 3]
    a.flatten!.should == nil
  end

  it "raises an ArgumentError on recursive arrays" do
    x = []
    x << x
    lambda { x.flatten! }.should raise_error(ArgumentError)
  
    x = []
    y = []
    x << y
    y << x
    lambda { x.flatten! }.should raise_error(ArgumentError)
  end

  compliant_on :ruby, :jruby do
    it "raises a TypeError on frozen arrays when modification would take place" do
      nested_ary = [1, 2, []]
      nested_ary.freeze
      lambda { nested_ary.flatten! }.should raise_error(TypeError)
    end

    it "does not raise on frozen arrays when no modification would take place" do
      ArraySpecs.frozen_array.flatten! # ok, already flat
    end
  end
end
