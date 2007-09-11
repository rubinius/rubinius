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
  
  it "calls to_int on start and length" do
    x = Object.new
    def x.to_int() 2 end
    [1, 2, 3, 4, 5].fill('a', x, x).should == [1, 2, "a", "a", 5]
    
#    x = Object.new
#    x.should_receive(:respond_to?, :count => 2, :with => [:to_int], :returning => true)
#    x.should_receive(:method_missing, :count => 2, :with => [:to_int], :returning => 2)
#    [1, 2, 3, 4, 5].fill('a', x, x).should == [1, 2, "a", "a", 5]
  end

  it "starts at 0 if the negative index is before the start of the array" do
    [1, 2, 3, 4, 5].fill('a', -25, 3).should == ['a', 'a', 'a', 4, 5]
    [1, 2, 3, 4, 5].fill('a', -10, 10).should == %w|a a a a a a a a a a|
  end
  
  it "does not change the Array with an index and a count of < 1" do
    [1, 2, 3].fill('a', 1, -3).should == [1, 2, 3]
    [1, 2, 3].fill('a', 1, 0).should == [1, 2, 3]
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

  it "raises ArgumentError if the wrong number of arguments is given" do
    should_raise(ArgumentError) { [].fill('a', 1, 2, true) }
    should_raise(ArgumentError) { [].fill('a', 1, true) {|i|} }
  end

  it "raises TypeError if the index is not numeric" do
    should_raise(TypeError) { [].fill 'a', true }
  end

  it "raises TypeError with range and length argument" do
    should_raise(TypeError) { [].fill('x', 0 .. 2, 5) }
  end

  it "ignores length if it is nil" do
    a = [1, 2, 3]
    a.fill('x', 1, nil)
    a.should == [1, 'x', 'x']
  end
  
  it "raises TypeError on a frozen array" do
    should_raise(TypeError) { frozen_array.fill('x') }
  end
end
