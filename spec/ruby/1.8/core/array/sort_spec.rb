require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#sort" do
  module ArraySpecs
    class SortSame
      def <=>(other); 0; end
      def ==(other); true; end
    end
  end

  it "returns a new array sorted based on comparing elements with <=>" do
    a = [1, -2, 3, 9, 1, 5, -5, 1000, -5, 2, -10, 14, 6, 23, 0]
    a.sort.should == [-10, -5, -5, -2, 0, 1, 1, 2, 3, 5, 6, 9, 14, 23, 1000]
  end

  it "does not affect the original Array" do
    a = [0, 15, 2, 3, 4, 6, 14, 5, 7, 12, 8, 9, 1, 10, 11, 13]
    b = a.sort
    a.should == [0, 15, 2, 3, 4, 6, 14, 5, 7, 12, 8, 9, 1, 10, 11, 13]
    b.should == (0..15).to_a
  end

  it "sorts already-sorted Arrays" do
    (0..15).to_a.sort.should == (0..15).to_a
  end

  it "sorts reverse-sorted Arrays" do
    (0..15).to_a.reverse.sort.should == (0..15).to_a
  end

  it "sorts Arrays that consist entirely of equal elements" do
    a = [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
    a.sort.should == a
    b = Array.new(15).map { ArraySpecs::SortSame.new }
    b.sort.should == b
  end

  it "sorts Arrays that consist mostly of equal elements" do
    a = [1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1]
    a.sort.should == [0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
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
    a = [1, -2, 3, 9, 1, 5, -5, 1000, -5, 2, -10, 14, 6, 23, 0]
    a.sort!
    a.should == [-10, -5, -5, -2, 0, 1, 1, 2, 3, 5, 6, 9, 14, 23, 1000]
  end

  it "sorts array in place using block value" do
    a = [0, 15, 2, 3, 4, 6, 14, 5, 7, 12, 8, 9, 1, 10, 11, 13]
    a.sort! { |x, y| y <=> x }.should == (0..15).to_a.reverse
  end

  compliant_on :ruby, :jruby do
    it "raises a TypeError on a frozen array" do
      lambda { ArraySpecs.frozen_array.sort! }.should raise_error(TypeError)
    end
  end
end
