require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#sort" do
  it "returns a new array sorted based on comparing elements with <=>" do
    [1, 1, 5, -5, 2, -10, 14, 6].sort.should == [-10, -5, 1, 1, 2, 5, 6, 14]
  end

  it "does not affect the original Array" do
    a = [1, 3, 2, 5]
    b = a.sort
    a.should == [1, 3, 2, 5]
    b.should == [1, 2, 3, 5]
  end

  it "sorts already-sorted Arrays" do
    [0, 1, 2, 3].sort.should == [0, 1, 2, 3]
  end

  it "sorts reverse-sorted Arrays" do
    [3, 2, 1, 0].sort.should == [0, 1, 2, 3]
  end

  it "does not deal with failures from incorrect/incompatible use of <=>" do
    o = Object.new

    lambda { [o, 1].sort }.should raise_error
  end

  it "may leave the Array partially sorted if the sorting fails at any point" do
    true.should == true   # Muhaha.
  end

  it "may take a block which is used to determine the order of objects a and b described as -1, 0 or +1" do
    a = [5, 1, 4, 3, 2]
    a.sort.should == [1, 2, 3, 4, 5]
    a.sort {|x, y| y <=> x}.should == [5, 4, 3, 2, 1]
  end

  it "completes when supplied a block that always returns the same result" do
    a = [2, 3, 5, 1, 4]
    a.sort {  1 }.class.should == Array
    a.sort {  0 }.class.should == Array
    a.sort { -1 }.class.should == Array
  end

  it "returns subclass instance on Array subclasses" do
    ary = MyArray[1, 2, 3]
    ary.sort.class.should == MyArray
  end
end

describe "Array#sort!" do
  it "sorts array in place using <=>" do
    a = [1, 9, 7, 11, -1, -4]
    a.sort!
    a.should == [-4, -1, 1, 7, 9, 11]
  end

  it "sorts array in place using block value" do
    a = [1, 3, 2, 5, 4]
    a.sort! { |x, y| y <=> x }
    a.should == [5, 4, 3, 2, 1]
  end

  compliant_on :ruby, :jruby do
    it "raises a TypeError on a frozen array" do
      lambda { ArraySpecs.frozen_array.sort! }.should raise_error(TypeError)
    end
  end
end
