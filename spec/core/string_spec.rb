require File.dirname(__FILE__) + '/../spec_helper'

# %, *, +, <<, <=>, ==, =~, [], []=, capitalize, capitalize!,
# casecmp, center, chomp, chomp!, chop, chop!, concat, count, crypt,
# delete, delete!, downcase, downcase!, dump, each, each_byte,
# each_line, empty?, eql?, gsub, gsub!, hash, hex, include?, index,
# initialize_copy, insert, inspect, intern, length, ljust, lstrip,
# lstrip!, match, next, next!, oct, replace, reverse, reverse!,
# rindex, rjust, rstrip, rstrip!, scan, size, slice, slice!, split,
# squeeze, squeeze!, strip, strip!, sub, sub!, succ, succ!, sum,
# swapcase, swapcase!, to_f, to_i, to_s, to_str, to_sym, tr, tr!,
# tr_s, tr_s!, unpack, upcase, upcase!, upto

context "String" do
  
  specify "<< should concatenate the other object" do
    example do
      @a = 'hello '
      p [@a << 'world', @a << 33]
    end.should == '["hello world", "hello world!"]'
  end
  
  specify "index with substring arg should return substring when it exists" do
    example do
      p "hello".index('e')
    end.should == "1"
  end

  specify "underlying storage should have the correct size (space for last \0 and multiple of 4)" do
    example do
      p "hell".data.size
    end.should == "8"
  end
  
  specify "to_i should convert the string to an integer base (2, 8, 10, or 16)" do
    example do
      p [ "12345".to_i,
          "99 red balloons".to_i,
          "0a".to_i,
          "0a".to_i(16),
          "hello".to_i,
          "1100101".to_i(2),
          "1100101".to_i(8),
          "1100101".to_i(10),
          "1100101".to_i(16) ]
    end.should == '[12345, 99, 0, 10, 0, 101, 294977, 1100101, 17826049]'
  end
  
  specify "substring should return the portion of string specified by index, length" do
    example do
      p [ "blah".substring(0, 2),
          "blah".substring(0, 4),
          "blah".substring(2, 2) ]
    end.should == '["bl", "blah", "ah"]'
  end
  
  specify "[] with index should " do
  end
  
  specify "[] with start, length should " do
  end
  
  specify "[] with range should " do
  end
  
  specify "[] with regexp should " do
  end
  
  specify "[] with regexp, length should " do
  end
  
  specify "[] with string should " do
  end
  
  specify "prefix? should " do
  end
  
  specify "index with fixnum should " do
  end
  
  specify "index with string should " do
  end
  
  specify "slice should be a synonym for []" do
  end
  
  specify "split should " do
  end
end
