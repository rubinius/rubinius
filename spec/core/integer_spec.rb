require File.dirname(__FILE__) + '/../spec_helper'

# class methods
# induced_from

# ceil, chr, downto, floor, integer?,
# next, numerator, prime_division, round, succ, times, to_i,
# to_int, truncate, upto

describe "Integer.induced_from" do
  it "converts other to Integer" do
    Integer.induced_from(2.5).should == 2 
    Integer.induced_from(-3.14).should == -3 
    Integer.induced_from(1.233450999123389e+12).should == 1233450999123
  end
end

integer_to_i = shared "" do |cmd|
  describe "Integer##{cmd}" do
    it "ceil should be a synonym for to_i" do
      a = 1
      a.send(cmd).should == 1 
      a.send(cmd).eql?(a).should == true
    end
  end
end

describe "Integer#ceil" do
  it_behaves_like(integer_to_i, :ceil)
end

describe "Integer#chr" do
  it "returns a string containing the ASCII character represented by self" do
    [82.chr, 117.chr, 98.chr, 105.chr, 110.chr, 105.chr, 117.chr, 115.chr, 
     32.chr, 
     114.chr, 111.chr, 99.chr, 107.chr, 115.chr].should == 
      ["R", "u", "b", "i", "n", "i", "u", "s", " ", "r", "o", "c", "k", "s"]
  end

  it "returns a new string" do
     82.chr.equal?(82.chr).should == false
  end
end

describe "Integer#downto" do
  it "passes block decreasing values from self down to and including other Integer" do
    a = []
    3.downto(-1) { |i| a << i }
    -1.downto(3) { |i| a << i }
    a.should == [3, 2, 1, 0, -1]
  end
end

describe "Integer#floor" do
  it "returns the largest integer less than or equal to self" do
    0.floor.should == 0 
    -1.floor.should == -1 
    1.floor.should == 1 
    0xffffffff.floor.should == 4294967295
  end
end

describe "Integer#integer?" do
  it "returns true" do
    0.integer?.should == true 
    0xffffffff.integer?.should == true
    -1.integer?.should == true
  end
end

integer_next = shared "Integer#next | Integer#succ" do |cmd|
  describe "Integer##{cmd}" do
    it "returns the Integer equal to self + 1" do
      0.send(cmd).should == 1 
      -1.send(cmd).should == 0
      0xffffffff.send(cmd).should == 4294967296 
      20.send(cmd).should == 21
    end
  end
end

describe "Integer#next" do
  it_behaves_like(integer_next, :next)
end

describe "Integer#round" do
  it_behaves_like(integer_to_i, :round)
end

describe "Integer#succ" do
  it_behaves_like(integer_next, :succ)
end

describe "Integer#times" do
  it "passes block values from 0 to self - 1" do
    a = []
    9.times { |i| a << i }
    -2.times { |i| a << i }
    a.should == [0, 1, 2, 3, 4, 5, 6, 7, 8]
  end
end

describe "Integer#to_i" do
  it_behaves_like(integer_to_i, :to_i)
end

describe "Integer#to_int" do
  it_behaves_like(integer_to_i, :to_int)
end

describe "Integer#truncate" do
  it_behaves_like(integer_to_i, :truncate)
end

describe "Integer#upto" do
  it "passes block values from self up to and including other Integer" do
    a = []
    9.upto(13) { |i| a << i }
    2.upto(-1) { |i| a << i }
    a.should == [9, 10, 11, 12, 13]
  end
end
