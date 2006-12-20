require File.dirname(__FILE__) + '/../spec_helper'

# &, *, +, -, <<, <=>, ==, [], []=, abbrev, assoc, at, clear,
# collect, collect!, compact, compact!, concat, delete, delete_at,
# delete_if, each, each_index, empty?, eql?, fetch, fill, first,
# flatten, flatten!, frozen?, hash, include?, index, indexes,
# indices, initialize_copy, insert, inspect, join, last, length, map,
# map!, nitems, pack, pop, push, rassoc, reject, reject!, replace,
# reverse, reverse!, reverse_each, rindex, select, shift, size,
# slice, slice!, sort, sort!, to_a, to_ary, to_s, transpose, uniq,
# uniq!, unshift, values_at, zip, |

context "Array" do
  
  specify "& should create an array with elements common to both arrays" do
    example(<<-CODE
      p [ 1, 1, 3, 5 ] & [ 1, 2, 3 ]
    CODE
    ).should == "[1, 3]"
  end
  
  specify "& should create an array with no duplicates" do
    example(<<-CODE
      p ([ 1, 1, 3, 5 ] & [ 1, 2, 3 ]).uniq!
    CODE
    ).should == "nil"
  end
  
  specify "* with a string should be equivalent to self.join(str)" do
    example(<<-CODE
      puts [ 1, 2, 3 ] * ","
    CODE
    ).should == "1,2,3"
  end
  
  specify "* with an int should concatenate n copies of the array" do
    example(<<-CODE
      p [ 1, 2, 3 ] * 3
    CODE
    ).should == "[1, 2, 3, 1, 2, 3, 1, 2, 3]"
  end
  
  specify "+ should concatenate two arrays" do
    example(<<-CODE
      p [ 1, 2, 3 ] + [ 4, 5 ]
    CODE
    ).should == "[1, 2, 3, 4, 5]"
  end

  specify "- should create an array minus any items from other array" do
    example(<<-CODE
      p [ 1, 1, 2, 2, 3, 3, 4, 5 ] - [ 1, 2, 4 ]
    CODE
    ).should == "[3, 3, 5]"
  end
  
  specify "<< should push the object onto the end of the array" do
    example(<<-CODE
      p [ 1, 2 ] << "c" << "d" << [ 3, 4 ]
    CODE
    ).should == '[1, 2, "c", "d", [3, 4]]'
  end
  
  specify "<=> should be 1 if the array is greater than the other array" do
    example(<<-CODE
      p [ 1, 2, 3, 4, 5, 6 ] <=> [ 1, 2 ]
    CODE
    ).should == "1"
  end
  
  specify "<=> should be 0 if the arrays are equal" do
    example(<<-CODE
      p [] <=> []
    CODE
    ).should == "0"
  end
  
  specify "<=> should be -1 if the array is less than the other array" do
    example(<<-CODE
      p [ "a", "a", "c" ] <=> [ "a", "b", "c" ]
    CODE
    ).should == "-1"
  end
  
  specify "== should be true if each element is == to the corresponding element in the other array" do
    example(<<-CODE
      p [ "a", "c", 7 ] == [ "a", "c", 7 ]
    CODE
    ).should == "true"
  end
  
  specify "== should be false if any element is not == to the corresponding element in the other the array" do
    example(<<-CODE
      p [ "a", "c" ] == [ "a", "c", 7 ]
    CODE
    ).should == "false"
  end
  
  specify "[] should provide the element at the specified index" do
    example(<<-CODE
      a = [ "a", "b", "c", "d", "e" ]
      puts a[1]
    CODE
    ).should == "b"
  end
  
  specify "[] should provide the element from the end of the array for a negative index" do
    example(<<-CODE
      a = [ "a", "b", "c", "d", "e" ]
      puts a[-2]
    CODE
    ).should == "d"
  end
  
  specify "[] should provide a subarray from start containing length elements" do
    example(<<-CODE
      a = [ "a", "b", "c", "d", "e" ]
      start = 2
      length = 3
      p a[start, length]
    CODE
    ).should == '["c", "d", "e"]'
  end
  
  specify "[] should provide a subarray specified by range" do
    example(<<-CODE
      a = [ "a", "b", "c", "d", "e" ]
      p a[1..3]
    CODE
    ).should == '["b", "c", "d"]'
  end
  
  specify "[] should not return nil if any elements in the requested range exist" do
    example(<<-CODE
      a = [ "a", "b", "c", "d", "e" ]
      p a[4..7]
    CODE
    ).should == '["e"]'
  end
  
  specify "[] should provide nil for a requested index not in the array" do
    example(<<-CODE
      a = [ "a", "b", "c", "d", "e" ]
      puts a[5]
    CODE
    ).should == "nil"
  end
  
  specify "[] should return nil if no requested index is in the array" do
    example(<<-CODE
      a = [ "a", "b", "c", "d", "e" ]
      p a[6..10]
    CODE
    ).should == "nil"
  end
  
  specify "each should yield each element to the block" do
    example(<<-CODE
      [ 1,2,3 ].each { |item| puts item }
    CODE
    ).should == "1\n2\n3"
  end

  specify "partition should return two arrays" do
    example(<<-CODE
      puts [].partition.inspect
    CODE
    ).should == "[[], []]"
  end
  
  specify "partition should return in the left array values for which the block evaluates to true" do
    example(<<-CODE
      p [ 0,1,2,3,4,5 ].partition { |i| i % 2 == 0 }
    CODE
    ).should == "[[0, 2, 4], [1, 3, 5]]"
  end
  
  specify "uniq should return an array with no duplicates" do
    example(<<-CODE
      p [ "a", "a", "b", "b", "c" ].uniq
    CODE
    ).should == '["a", "b", "c"]'
  end
  
  specify "uniq! should return the same array" do
    example(<<-CODE
      a = [ "a", "a", "b", "b", "c" ]
      puts a.equal?(a.uniq!)
    CODE
    ).should == 'true'
  end
  
  specify "uniq! should return the array with no duplicates" do
    example(<<-CODE
      p [ "a", "a", "b", "b", "c" ].uniq!
    CODE
    ).should == '["a", "b", "c"]'
  end
  
  specify "uniq! should return nil if no changes are made to the array" do
    example(<<-CODE
      p [ "a", "b", "c" ].uniq!
    CODE
    ).should == 'nil'
  end
  
end
