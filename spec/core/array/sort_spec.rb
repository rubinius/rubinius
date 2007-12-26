require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#sort" do
  it "returns a new array from sorting elements using <=> on the pivot" do
    d = D.new

    [1, 1, 5, -5, 2, -10, 14, 6].sort.should == [-10, -5, 1, 1, 2, 5, 6, 14]
    [d, 1].sort.should == [1, d]
  end

  it "raises an ArgumentError if the comparison cannot be completed" do
    d = D.new

    # Fails essentially because of 1.<=>(d) whereas d.<=>(1) would work
    lambda { [1, d].sort.should == [1, d] }.should raise_error(ArgumentError)
  end
  
  it "may take a block which is used to determine the order of objects a and b described as -1, 0 or +1" do
    a = [5, 1, 4, 3, 2]
    a.sort.should == [1, 2, 3, 4, 5]
    a.sort {|x, y| y <=> x}.should == [5, 4, 3, 2, 1]
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

  compliant :ruby, :jruby do
    it "raises TypeError on a frozen array" do
      lambda { ArraySpecs.frozen_array.sort! }.should raise_error(TypeError)
    end
  end
end
