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
    rubinius(<<-CODE
        puts [ 1, 1, 3, 5 ] & [ 1, 2, 3 ]
      CODE
    ).should == "[1, 3]"
  end
  
  specify "& should create an array with no duplicates" do
    rubinius(<<-CODE
        puts d
      CODE
    ).should == "nil"
  end

  specify "- should create an array minus any items from other array" do
    rubinius(<<-CODE
      puts ([ 1, 1, 2, 2, 3, 3, 4, 5 ] - [ 1, 2, 4 ]).inspect
      CODE
    ).should == "[3, 3, 5]"
  end
  specify "each should yield each element to the block" do
    rubinius(<<-CODE
        [ 1,2,3 ].each { |item| puts item }
      CODE
    ).should == "1\n2\n3"
  end

  specify "partition should return two arrays" do
    rubinius(<<-CODE
        puts [].partition.inspect
      CODE
    ).should == "[[], []]"
  end
  
  specify "partition should return in the left array values for which the block evaluates to true" do
    rubinius(<<-CODE
        puts [ 0,1,2,3,4,5 ].partition { |i| i % 2 == 0 }.inspect
      CODE
    ).should == "[[0, 2, 4], [1, 3, 5]]"
  end
  
  specify "uniq should return an array with no duplicates" do
    rubinius(<<-CODE
        puts [ "a", "a", "b", "b", "c" ].uniq.inspect
      CODE
    ).should == '["a", "b", "c"]'
  end
  
  specify "uniq! should return the same array" do
    rubinius(<<-CODE
        a = [ "a", "a", "b", "b", "c" ]
        puts a.equal?(a.uniq!)
      CODE
    ).should == 'true'
  end
  
  specify "uniq! should return the array with no duplicates" do
    rubinius(<<-CODE
        puts [ "a", "a", "b", "b", "c" ].uniq!.inspect
      CODE
    ).should == '["a", "b", "c"]'
  end
  
  specify "uniq! should return nil if no changes are made to the array" do
    rubinius(<<-CODE
        puts [ "a", "b", "c" ].uniq!.inspect
      CODE
    ).should == 'nil'
  end
end
