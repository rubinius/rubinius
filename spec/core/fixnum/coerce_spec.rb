require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#coerce when given a Fixnum" do
  it "returns an array containing two Fixnums" do
    1.coerce(2).should == [2, 1]
    1.coerce(2).map { |i| i.class }.should == [Fixnum, Fixnum]
  end
end

describe "Fixnum#coerce when given a String" do
  it "raises an ArgumentError when trying to coerce with a non-number String" do
    should_raise(ArgumentError) do
      1.coerce(":)")
    end
  end

  it "returns  an array containing two Floats" do
    1.coerce("2").should == [2.0, 1.0]
    1.coerce("-2").should == [-2.0, 1.0]
  end
end

describe "Fixnum#coerce" do
  it "raises a TypeError when trying to coerce with nil" do
    should_raise(TypeError, "can't convert nil into Float") do
      1.coerce(nil)
    end
  end

  it "tries to convert the given Object into a Float by using #to_f" do
    (obj = Object.new).should_receive(:to_f, :returning => 1.0)
    2.coerce(obj).should == [1.0, 2.0]
    
    (obj = Object.new).should_receive(:to_f, :returning => '0')
    should_raise(TypeError, "Object#to_f should return Float") do
      2.coerce(obj).should == [1.0, 2.0]
    end
  end

  it "raises a TypeError when given an Object that does not respond to #to_f" do
    should_raise(TypeError, "can't convert Object into Float") do
      1.coerce(Object.new)
    end
    
    should_raise(TypeError, "can't convert Range into Float") do
      1.coerce(1..4)
    end
    
    should_raise(TypeError, "can't convert Symbol into Float") do
      1.coerce(:test)
    end
  end
end
