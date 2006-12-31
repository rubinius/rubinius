require File.dirname(__FILE__) + '/../spec_helper'

# &, *, +, -, <<, <=>, ==, [], []=, assoc, at, clear,
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
    example do
      p [ 1, 1, 3, 5 ] & [ 1, 2, 3 ]
    end.should == "[1, 3]"
  end
  
  specify "& should create an array with no duplicates" do
    example do
      p(([ 1, 1, 3, 5 ] & [ 1, 2, 3 ]).uniq!)
    end.should == "nil"
  end
  
  specify "* with a string should be equivalent to self.join(str)" do
    example do
      puts [ 1, 2, 3 ] * ","
    end.should == "1,2,3"
  end
  
  specify "* with an int should concatenate n copies of the array" do
    example do
      p [ 1, 2, 3 ] * 3
    end.should == "[1, 2, 3, 1, 2, 3, 1, 2, 3]"
  end
  
  specify "+ should concatenate two arrays" do
    example do
      p [ 1, 2, 3 ] + [ 4, 5 ]
    end.should == "[1, 2, 3, 4, 5]"
  end

  specify "- should create an array minus any items from other array" do
    example do
      p [ 1, 1, 2, 2, 3, 3, 4, 5 ] - [ 1, 2, 4 ]
    end.should == "[3, 3, 5]"
  end
  
  specify "<< should push the object onto the end of the array" do
    example do
      p [ 1, 2 ] << "c" << "d" << [ 3, 4 ]
    end.should == '[1, 2, "c", "d", [3, 4]]'
  end
  
  specify "<=> should be 1 if the array is greater than the other array" do
    example do
      p [ 1, 2, 3, 4, 5, 6 ] <=> [ 1, 2 ]
    end.should == "1"
  end
  
  specify "<=> should be 0 if the arrays are equal" do
    example do
      p [] <=> []
    end.should == "0"
  end
  
  specify "<=> should be -1 if the array is less than the other array" do
    example do
      p [ "a", "a", "c" ] <=> [ "a", "b", "c" ]
    end.should == "-1"
  end
  
  specify "== should be true if each element is == to the corresponding element in the other array" do
    example do
      p [ "a", "c", 7 ] == [ "a", "c", 7 ]
    end.should == "true"
  end
  
  specify "== should be false if any element is not == to the corresponding element in the other the array" do
    example do
      p [ "a", "c" ] == [ "a", "c", 7 ]
    end.should == "false"
  end
  
  specify "[] should provide the element at the specified index" do
    example do
      puts [ "a", "b", "c", "d", "e" ][1]
    end.should == "b"
  end
  
  specify "[] should provide the element from the end of the array for a negative index" do
    example do
      puts [ "a", "b", "c", "d", "e" ][-2]
    end.should == "d"
  end
  
  specify "[] should provide a subarray from start containing length elements" do
    example do
      p [ "a", "b", "c", "d", "e" ][2, 3]
    end.should == '["c", "d", "e"]'
  end
  
  specify "[] should provide a subarray specified by range" do
    example do
      p [ "a", "b", "c", "d", "e" ][1..3]
    end.should == '["b", "c", "d"]'
  end
  
  specify "[] should not return nil if any elements in the requested range exist" do
    example do
      p [ "a", "b", "c", "d", "e" ][4..7]
    end.should == '["e"]'
  end
  
  specify "[] should provide nil for a requested index not in the array" do
    example do
      puts [ "a", "b", "c", "d", "e" ][5]
    end.should == "nil"
  end
  
  specify "[] should return nil if no requested index is in the array" do
    example do
      p [ "a", "b", "c", "d", "e" ][6..10]
    end.should == "nil"
  end
  
  specify "[]= should set the value of the element at index" do
    example do
      @a = [1, 2, 3, 4]
      @a[2] = 5
      @a[-1] = 6
      @a[5] = 3
      p @a
    end.should == '[1, 2, 5, 6, nil, 3]'
  end
  
  specify "[]= should remove the section defined by start, length when set to nil" do
    example do
      @a = ['a', 'b', 'c', 'd', 'e']
      @a[1, 3] = nil
      p @a
    end.should == '["a", "e"]'
  end
  
  specify "[]= should set the section defined by start, length to other" do
    example do
      @a = [1, 2, 3, 4, 5, 6]
      @a[0, 1] = 2
      @a[3, 2] = ['a', 'b', 'c', 'd']
      p @a
    end.should == '[2, 2, 3, "a", "b", "c", "d", 6]'
  end
  
  specify "[]= should remove the section defined by range when set to nil" do
    example do
      @a = [1, 2, 3, 4, 5]
      @a[0..1] = nil
      p @a
    end.should == '[3, 4, 5]'
  end
  
  specify "[]= should set the section defined by range to other" do
    example do
      @a = [6, 5, 4, 3, 2, 1]
      @a[1...2] = 9
      @a[3..6] = [6, 6, 6]
      p @a
    end.should == '[6, 9, 4, 6, 6, 6]'
  end
  
  specify "assoc should return the first contained array that the argument as the first element" do
    example do
      @s1 = [ "colors", "red", "blue", "green" ] 
      @s2 = [ "letters", "a", "b", "c" ] 
      @s3 = "foo" 
      @a = [ @s1, @s2, @s3 ] 
      p [@a.assoc("letters"), @a.assoc("foo")]
    end.should == '[["letters", "a", "b", "c"], nil]'
  end
  
  specify "at should return the element at index" do
    example do
      @a = [1, 2, 3, 4, 5, 6]
      p [@a.at(0), @a.at(-2), @a.at(10)]
    end.should == '[1, 5, nil]'
  end
  
  specify "clear should remove all elements" do
    example do
      @a = [1, 2, 3, 4]
      @a.clear
      p @a
    end.should == '[]'
  end
  
  specify "collect should return a copy of array with each element replaced by the value returned by block" do
    example do
      @a = ['a', 'b', 'c', 'd']
      @b = @a.collect { |i| i + '!' }
      p @b
    end.should == '["a!", "b!", "c!", "d!"]'
  end
  
  specify "collect! should replace each element with the value returned by block" do
    example do
      @a = [7, 9, 3, 5]
      @a.collect! { |i| i - 1 }
      p @a
    end.should == '[6, 8, 2, 4]'
  end
  
  specify "compact should return a copy of array with all nil elements removed" do
    example do
      @a = [1, nil, 2, nil, 4, nil]
      p @a.compact
    end.should == '[1, 2, 4]'
  end
  
  specify "compact! should remove all nil elements" do
    example do
      @a = ['a', nil, 'b', nil, nil, 'c']
      p @a.compact!
    end.should == '["a", "b", "c"]'
  end
  
  specify "compact! should return nil if there are no nil elements to remove" do
    example do
      p [1, 2, 3].compact!
    end.should == 'nil'
  end
  
  specify "concat should append the elements in the other array" do
    example do
      p [1, 2, 3].concat([9, 10, 11])
    end.should == '[1, 2, 3, 9, 10, 11]'
  end
  
  specify "delete should remove elements equal to object" do
    example do
      @a = [1, 2, 3, 3, 4, 3, 5]
      @a.delete(3)
      p @a
    end.should == '[1, 2, 4, 5]'
  end
  
  specify "delete should return nil if no elements match object" do
    example do
      p [1, 2, 4, 5].delete(3)
    end.should == 'nil'
  end
  
  specify "delete_at should remove the element at the specified index" do
    example do
      @a = [1, 2, 3, 4]
      @a.delete_at(2)
      p @a
    end.should == '[1, 2, 4]'
  end
  
  specify "delete_at should return nil if the index is out of range" do
    example do
      @a = [1, 2]
      p @a.delete_at(3)
    end.should == 'nil'
  end
  
  specify "delete_if should remove each element for which block returns true" do
    example do
      @a = [ "a", "b", "c" ] 
      @a.delete_if { |x| x >= "b" }
      p @a
    end.should == '["a"]'
  end
  
  specify "each should yield each element to the block" do
    example do
      [ 1,2,3 ].each { |item| puts item }
    end.should == "1\n2\n3"
  end
  
  specify "each_index should pass the index of each element to the block" do
    example do
      ['a', 'b', 'c', 'd'].each_index { |i| puts i }
    end.should == "1\n2\n3\n4"
  end
  
  specify "empty? should return true if the array has no elements" do
    example do
      p [[].empty?, [1].empty?, [1, 2].empty?]
    end.should == '[true, false, false]'
  end
  
  specify "eql? should return true if other is the same array" do
    example do
      @a = []
      @b = []
      p [@a.eql?(@a), @a.eql?(@b)]
    end.should == '[true, false]'
  end
  
  specify "eql? should return true if other has the same length and elements" do
    example do
      @a = [1, 2, 3, 4]
      @b = [1, 2, 3, 4]
      @c = [1, 2]
      @d = ['a', 'b', 'c', 'd']
      p [@a.eql?(@b), @a.eql?(@c), @a.eql?(@d)]
    end.should == '[true, false, false]'
  end
  
  specify "partition should return two arrays" do
    example do
      puts [].partition.inspect
    end.should == "[[], []]"
  end
  
  specify "partition should return in the left array values for which the block evaluates to true" do
    example do
      p [ 0,1,2,3,4,5 ].partition { |i| i % 2 == 0 }
    end.should == "[[0, 2, 4], [1, 3, 5]]"
  end
  
  specify "uniq should return an array with no duplicates" do
    example do
      p [ "a", "a", "b", "b", "c" ].uniq
    end.should == '["a", "b", "c"]'
  end
  
  specify "uniq! should return the same array" do
    example do
      @a = [ "a", "a", "b", "b", "c" ]
      puts @a.equal?(@a.uniq!)
    end.should == 'true'
  end
  
  specify "uniq! should return the array with no duplicates" do
    example do
      p [ "a", "a", "b", "b", "c" ].uniq!
    end.should == '["a", "b", "c"]'
  end
  
  specify "uniq! should return nil if no changes are made to the array" do
    example do
      p [ "a", "b", "c" ].uniq!
    end.should == 'nil'
  end
  
end
