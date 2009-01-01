require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/slice'

describe "Array#[]" do
  it_behaves_like(:array_slice, :[])
end

describe "Array.[]" do
  it "[] should return a new array populated with the given elements" do
    array = Array[1, 'a', nil]
    array[0].should == 1
    array[1].should == 'a'
    array[2].should == nil
  end

  it "when applied to a literal nested array, unpacks its elements into the containing array" do
    Array[1, 2, *[3, 4, 5]].should == [1, 2, 3, 4, 5]
  end

  it "when applied to a nested referenced array, unpacks its elements into the containing array" do
    splatted_array = Array[3, 4, 5]
    Array[1, 2, *splatted_array].should == [1, 2, 3, 4, 5]
  end

  ruby_version_is '1.9' do
    it 'can unpack 2 or more nested referenced array' do
      splatted_array = Array[3, 4, 5]
      splatted_array2 = Array[6, 7, 8]
      eval("Array[1, 2, *splatted_array, *splatted_array2]").should == [1, 2, 3, 4, 5, 6, 7, 8]
    end

    it 'constructs a nested Hash for tailing key-value pairs' do
      eval(<<-EOS).should == [1, 2, { 3=> 4, 5 => 6 }]
        Array[1, 2, 3 => 4, 5 => 6]
      EOS
    end
  end
  
  it "returns an instance of the subtype when called on an Array subclass" do
    ArraySub = Class.new Array
    ArraySub[1,2].class.should == ArraySub
  end
end
