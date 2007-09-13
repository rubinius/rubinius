require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#flatten" do
  it "returns a one-dimensional flattening recursively" do
    [[[1, [2, 3]],[2, 3, [4, [4, [5, 5]], [1, 2, 3]]], [4]], []].flatten.should == [1, 2, 3, 2, 3, 4, 4, 5, 5, 1, 2, 3, 4]
  end

  it "does not call flatten on elements" do
    obj = Object.new
    def obj.flatten() [1, 2] end
    [obj, obj].flatten.should == [obj, obj]

    obj = [5, 4]
    def obj.flatten() [1, 2] end
    [obj, obj].flatten.should == [5, 4, 5, 4]
  end
  
  it "raises ArgumentError on recursive arrays" do
    x = []
    x << x
    should_raise(ArgumentError) { x.flatten }
  
    x = []
    y = []
    x << y
    y << x
    should_raise(ArgumentError) { x.flatten }
  end
  
  it "returns subclass instance for Array subclasses" do
    MyArray[].flatten.class.should == MyArray
    MyArray[1, 2, 3].flatten.class.should == MyArray
    MyArray[1, [2], 3].flatten.class.should == MyArray
    [MyArray[1, 2, 3]].flatten.class.should == Array
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

  it "raises ArgumentError on recursive arrays" do
    x = []
    x << x
    should_raise(ArgumentError) { x.flatten! }
  
    x = []
    y = []
    x << y
    y << x
    should_raise(ArgumentError) { x.flatten! }
  end

compliant :r18 do
  it "raises TypeError on frozen arrays when modification would take place" do
    nested_ary = [1, 2, []]
    nested_ary.freeze
    should_raise(TypeError) { nested_ary.flatten! }
  end

  it "does not raise on frozen arrays when no modification would take place" do
    @frozen_array.flatten! # ok, already flat
  end
end

noncompliant :rubinius do
  it "always raises TypeError on frozen arrays" do
    should_raise(TypeError) { @frozen_array.flatten! }
    nested_ary = [1, 2, []]
    nested_ary.freeze
    should_raise(TypeError) { nested_ary.flatten! }
  end
end
end
