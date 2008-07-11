require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#fill" do
  it "replaces all elements in the array with object" do
    ary = ['a', 'b', 'c', 'duh']
    ary.fill(8).should == [8, 8, 8, 8]

    str = "x"
    ary.fill(str).should == [str, str, str, str]
    str << "y"
    ary.should == [str, str, str, str]
  end
  
  it "replaces length elements with object beginning with start index" do
    [1, 2, 3, 4, 5].fill('a', 2, 2).should == [1, 2, "a", "a", 5]
    [1, 2, 3, 4, 5].fill('a', 2, 5).should == [1, 2, "a", "a", "a", "a", "a"]
    [1, 2, 3, 4, 5].fill('a', 2, -2).should == [1, 2, 3, 4, 5]
    [1, 2, 3, 4, 5].fill('a', 2, 0).should == [1, 2, 3, 4, 5]

    [1, 2, 3, 4, 5].fill('a', -2, 2).should == [1, 2, 3, "a", "a"]    
    [1, 2, 3, 4, 5].fill('a', -2, 4).should == [1, 2, 3, "a", "a", "a", "a"]    
    [1, 2, 3, 4, 5].fill('a', -2, -2).should == [1, 2, 3, 4, 5]
    [1, 2, 3, 4, 5].fill('a', -2, 0).should == [1, 2, 3, 4, 5]
  end
  
  it "makes no modifications if given an index greater than end and no size" do
    [1, 2, 3, 4, 5].fill('a', 5).should == [1, 2, 3, 4, 5]
  end
  
  it "tries to convert the passed arguments to Integers using #to_int" do
    obj = mock('to_int')
    obj.should_receive(:to_int).and_return(2, 2)
    [1, 2, 3, 4, 5].fill('a', obj, obj).should == [1, 2, "a", "a", 5]
  end
  
  it "checks whether the passed arguments respond to #to_int" do
    obj = mock('method_missing to_int')
    obj.should_receive(:respond_to?).with(:to_int).any_number_of_times.and_return(true)
    obj.should_receive(:method_missing).with(:to_int).twice.and_return(2)
    [1, 2, 3, 4, 5].fill('a', obj, obj).should == [1, 2, "a", "a", 5]
  end

  it "starts at 0 if the negative index is before the start of the array" do
    [1, 2, 3, 4, 5].fill('a', -25, 3).should == ['a', 'a', 'a', 4, 5]
    [1, 2, 3, 4, 5].fill('a', -10, 10).should == %w|a a a a a a a a a a|
  end
  
  it "does not change the Array given an index and a count of 0" do
    [1, 2, 3].fill('a', 1, 0).should == [1, 2, 3]
  end

  it "does not raise an exception if given an index and a negative count whose absolute value does not exceed the index" do
    lambda { [1, 2, 3].fill('a', 3, -1)}.should_not raise_error(ArgumentError)
    lambda { [1, 2, 3].fill('a', 3, -2)}.should_not raise_error(ArgumentError)
    lambda { [1, 2, 3].fill('a', 3, -3)}.should_not raise_error(ArgumentError)
  end

  # See: http://blade.nagaokaut.ac.jp/cgi-bin/scat.rb/ruby/ruby-core/17481
  it "returns self when given an index and a negative count" do
    [1, 2, 3].fill('a', 3, -2).should == [1, 2, 3]
    [1, 2, 3].fill('a', 3, -4).should == [1, 2, 3]
    [1, 2, 3].fill('a', 2, -1).should == [1, 2, 3]
    [1, 2, 3].fill('a', 1, -3).should == [1, 2, 3]
    [1, 2, 3].fill('a', 1, -1).should == [1, 2, 3]
    [1, 2, 3].fill('a', 1, -300000).should == [1, 2, 3]
  end

  it "replaces elements in range with object" do
    [1, 2, 3, 4, 5, 6].fill(8, 0..3).should == [8, 8, 8, 8, 5, 6]
    [1, 2, 3, 4, 5, 6].fill(8, 0...3).should == [8, 8, 8, 4, 5, 6]
    [1, 2, 3, 4, 5, 6].fill('x', 4..6).should == [1, 2, 3, 4, 'x', 'x', 'x']
    [1, 2, 3, 4, 5, 6].fill('x', 4...6).should == [1, 2, 3, 4, 'x', 'x']
    [1, 2, 3, 4, 5, 6].fill('x', -2..-1).should == [1, 2, 3, 4, 'x', 'x']
    [1, 2, 3, 4, 5, 6].fill('x', -2...-1).should == [1, 2, 3, 4, 'x', 6]
    [1, 2, 3, 4, 5, 6].fill('x', -2...-2).should == [1, 2, 3, 4, 5, 6]
    [1, 2, 3, 4, 5, 6].fill('x', -2..-2).should == [1, 2, 3, 4, 'x', 6]
    [1, 2, 3, 4, 5, 6].fill('x', -2..0).should == [1, 2, 3, 4, 5, 6]
    [1, 2, 3, 4, 5, 6].fill('x', 0...0).should == [1, 2, 3, 4, 5, 6]
    [1, 2, 3, 4, 5, 6].fill('x', 1..1).should == [1, 'x', 3, 4, 5, 6]
  end
  
  it "replaces all elements with the value of block (index given to block)" do
    [nil, nil, nil, nil].fill { |i| i * 2 }.should == [0, 2, 4, 6]
  end
  
  it "replaces length elements beginning with start with the value of block" do
    [true, false, true, false, true, false, true].fill(1, 4) { |i| i + 3 }.should == [true, 4, 5, 6, 7, false, true]
  end
  
  it "replaces all elements in range with the value of block" do
    [1, 1, 1, 1, 1, 1].fill(1..6) { |i| i + 1 }.should == [1, 2, 3, 4, 5, 6, 7]
  end

  it "increases the Array size when necessary" do
    a = [1, 2, 3]
    a.size.should == 3
    a.fill 'a', 0, 10
    a.size.should == 10 
  end

  it "raises an ArgumentError if the wrong number of arguments is given" do
    lambda { [].fill('a', 1, 2, true) }.should raise_error(ArgumentError)
    lambda { [].fill('a', 1, true) {|i|} }.should raise_error(ArgumentError)
  end

  it "raises a TypeError if the index is not numeric" do
    lambda { [].fill 'a', true }.should raise_error(TypeError)
  end
  
  it "raises an ArgumentError or RangeError for too-large sizes" do
    arr = [1, 2, 3]
    lambda { arr.fill(10, 1, 2**31 - 1) }.should raise_error(ArgumentError)
    lambda { arr.fill(10, 1, 2**31) }.should raise_error(RangeError)
  end

  it "raises a TypeError with range and length argument" do
    lambda { [].fill('x', 0 .. 2, 5) }.should raise_error(TypeError)
  end

  it "ignores length if it is nil" do
    a = [1, 2, 3]
    a.fill('x', 1, nil)
    a.should == [1, 'x', 'x']
  end

  compliant_on :ruby, :jruby do
    it "raises a TypeError on a frozen array" do
      lambda { ArraySpecs.frozen_array.fill('x') }.should raise_error(TypeError)
    end
  end
end
