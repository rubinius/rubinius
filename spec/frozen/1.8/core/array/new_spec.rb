require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array.new" do
  it "returns a new array when not passed arguments" do
    a = Array.new
    a.class.should == Array
    b = ArraySpecs::MyArray.new
    b.class.should == ArraySpecs::MyArray
  end
  
  it "raises an ArgumentError when passed a negative size" do
    lambda { Array.new(-1) }.should raise_error(ArgumentError)
  end
  
  it "returns a new array of size with nil elements" do
    Array.new(5).should == [nil, nil, nil, nil, nil]
    a = ArraySpecs::MyArray.new(5)
    a.class.should == ArraySpecs::MyArray
    a.inspect.should == [nil, nil, nil, nil, nil].inspect
  end

  it "tries to convert the passed arguments to Arrays using #to_ary" do
    obj = mock('[:foo]')
    obj.should_receive(:to_ary).and_return([:foo])
    Array.new(obj).should == [:foo]
  end
  
  it "checks whether the passed argument responds to #to_ary" do
    obj = mock('[:foo]')
    obj.should_receive(:respond_to?).with(:to_ary).any_number_of_times.and_return(true)
    obj.should_receive(:method_missing).with(:to_ary).any_number_of_times.and_return([:foo])
    Array.new(obj).should == [:foo]
  end
  
  it "tries to convert the passed arguments to Integers using #to_int when they don't respond to #to_ary" do
    obj = mock('3')
    obj.should_receive(:to_int).and_return(3)
    Array.new(obj).should == [nil, nil, nil]
  end

  it "checks whether the passed argument responds to #to_int if they don't respond to #to_ary" do
    obj = mock('3')
    obj.should_receive(:respond_to?).with(:to_ary).and_return(false)
    obj.should_receive(:respond_to?).with(:to_int).any_number_of_times.and_return(true)
    obj.should_receive(:method_missing).with(:to_int).and_return(3)
    Array.new(obj).should == [nil, nil, nil]
  end

  it "returns a new array of size default objects" do
    Array.new(4, true).should == [true, true, true, true]

    # Shouldn't copy object
    str = "x"
    ary = Array.new(4, str)
    str << "y"
    ary.should == [str, str, str, str]

    a = ArraySpecs::MyArray.new(4, true)
    a.class.should == ArraySpecs::MyArray
    a.inspect.should == [true, true, true, true].inspect
  end
  
  it "does not call to_ary on Array subclasses when passed an array-like argument" do
    Array.new(ArraySpecs::ToAryArray[5, 6, 7]).should == [5, 6, 7]
  end
  
  it "calls to_ary on an argument before to_int" do
    obj = mock('[1,2,3]')
    def obj.to_ary() [1, 2, 3] end
    def obj.to_int() 3 end

    Array.new(obj).should == [1, 2, 3]
  end
    
  it "returns an array of size elements from the result of passing each index to block" do
    Array.new(5) { |i| i + 1 }.should == [1, 2, 3, 4, 5]
    
    a = ArraySpecs::MyArray.new(5) { |i| i + 1 }
    a.class.should == ArraySpecs::MyArray
    a.inspect.should == [1, 2, 3, 4, 5].inspect
  end  

  it "will fail if a to_ary is supplied as the first argument and a second argument is given" do
    lambda { Array.new([1, 2], 1) }.should raise_error(TypeError)
  end
end
