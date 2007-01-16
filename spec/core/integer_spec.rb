require File.dirname(__FILE__) + '/../spec_helper'

# class methods
# induced_from

# ceil, chr, downto, floor, integer?,
# next, numerator, prime_division, round, succ, times, to_i,
# to_int, truncate, upto

context "Integer class method" do
  specify "induced_from should convert other to Integer" do
    example do
      [ Integer.induced_from(2.5), 
        Integer.induced_from(-3.14), 
        Integer.induced_from(1.233450999123389e+12)]
    end.should == [2, -3, 1233450999123]
  end
end

context "Integer instance method" do
  specify "ceil should be a synonym for to_i" do
    example do
      @a = 1
      [@a.ceil, @a.ceil.eql?(@a)]
    end.should == [1, true]
  end
  
  specify "chr should return a string containing the ASCII character represented by self" do
    example do
       [ 82.chr, 117.chr, 98.chr, 105.chr, 110.chr, 105.chr, 117.chr, 115.chr,
         32.chr, 
         114.chr, 111.chr, 99.chr, 107.chr, 115.chr ]
    end.should == ["R", "u", "b", "i", "n", "i", "u", "s", " ", "r", "o", "c", "k", "s"]
  end

  specify "chr should return a new string" do
    example do
       82.chr.equal?(82.chr)
    end.should == false
  end
  
  specify "downto should pass block decreasing values from self down to and including other Integer" do
    example do
      @a = []
      3.downto(-1) { |i| @a << i }
      -1.downto(3) { |i| @a << i }
      @a
    end.should == [3, 2, 1, 0, -1]
  end
  
  specify "floor should return the largest integer less than or equal to" do
    example do
      [0.floor, -1.floor, 1.floor, 0xffffffff.floor]
    end.should == [0, -1, 1, 4294967295]
  end
  
  specify "integer? should return true" do
    example do
      [0.integer?, 0xffffffff.integer?, -1.integer?]
    end.should == [true, true, true]
  end
  
  specify "next should return the Integer equal to self + 1" do
    example do
      [0.next, -1.next, 0xffffffff.next, 20.next]
    end.should == [1, 0, 4294967296, 21]
  end
  
  specify "round should be a synonym for to_i" do
    example do
      @a = 1
      [@a.round, @a.round.eql?(@a)]
    end.should == [1, true]
  end
  
  specify "succ should be a synonym for next" do
    example do
      [0.next, -1.succ, 0xffffffff.succ, 20.succ]
    end.should == [1, 0, 4294967296, 21]
  end
  
  specify "times should pass block values from 0 to self - 1" do
    example do
      @a = []
      9.times { |i| @a << i }
      -2.times { |i| @a << i }
      @a
    end.should == [0, 1, 2, 3, 4, 5, 6, 7, 8]
  end
  
  specify "to_i should return self" do
    example do
      @a = 1
      [@a.to_i, @a.to_i.eql?(@a)]
    end.should == [1, true]
  end
  
  specify "to_int should be a synonym for to_i" do
    example do
      @a = 1
      [@a.to_int, @a.to_int.eql?(@a)]
    end.should == [1, true]
  end
  
  specify "truncate should be a synonym for to_i" do
    example do
      @a = 1
      [@a.truncate, @a.truncate.eql?(@a)]
    end.should == [1, true]
  end
  
  specify "upto should pass block values from self up to and including other Integer" do
    example do
      @a = []
      9.upto(13) { |i| @a << i }
      2.upto(-1) { |i| @a << i }
      @a
    end.should == [9, 10, 11, 12, 13]
  end
end
