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

context "Array class method" do
  specify "new without arguments should return a new array" do
    example do
      a = Array.new
      a.class
    end.should == Array
  end
  
  specify "new with size should return a new array of size with nil elements" do
    example do
      Array.new(5)
    end.should == [nil, nil, nil, nil, nil]
  end
  
  specify "new with size and default object should return a new array of size objects" do
    example do
      Array.new(4, true)
    end.should == [true, true, true, true]
  end
  
  specify "new with array-like argument should return a new array by calling to_ary on argument" do
    example do
      Array.new([1, 2, 3])
    end.should == [1, 2, 3]
  end
  
  specify "new with size and block should return an array of size elements from the result of passing each index to block" do
    example do
      Array.new(5) { |i| i + 1 }
    end.should == [1, 2, 3, 4, 5]
  end

  specify "new with size and block should have proper side effects from passing each index to block" do
    example do
      a = []
      Array.new(5) { |i| a << i + 1 }
      a
    end.should == [1, 2, 3, 4, 5]
  end
  
  specify ".[] should return a new array populated with the given elements" do
    example do
      Array.[](5, true, nil, 'a', "Ruby")
    end.should == [5, true, nil, "a", "Ruby"]
  end
  
  specify "[] should be a synonym for .[]" do
    example do
      Array[5, true, nil, 'a', "Ruby"]
    end.should == [5, true, nil, "a", "Ruby"]
  end
end

context "Array instance method" do
  
  specify "& should create an array with elements common to both arrays (intersection)" do
    example do
      [ 1, 1, 3, 5 ] & [ 1, 2, 3 ]
    end.should == [1, 3]
  end
  
  specify "& should create an array with no duplicates" do
    example do
      ([ 1, 1, 3, 5 ] & [ 1, 2, 3 ]).uniq!
    end.should == nil
  end
  
  specify "* with a string should be equivalent to self.join(str)" do
    example do
      [ 1, 2, 3 ] * ","
    end.should == "1,2,3"
  end
  
  specify "* with an int should concatenate n copies of the array" do
    example do
      [ 1, 2, 3 ] * 3
    end.should == [1, 2, 3, 1, 2, 3, 1, 2, 3]
  end
  
  specify "+ should concatenate two arrays" do
    example do
      [ 1, 2, 3 ] + [ 4, 5 ]
    end.should == [1, 2, 3, 4, 5]
  end

  specify "- should create an array minus any items from other array" do
    example do
      [ 1, 1, 2, 2, 3, 3, 4, 5 ] - [ 1, 2, 4 ]
    end.should == [3, 3, 5]
  end
  
  specify "<< should push the object onto the end of the array" do
    example do
      [ 1, 2 ] << "c" << "d" << [ 3, 4 ]
    end.should == [1, 2, "c", "d", [3, 4]]
  end
  
  specify "<=> should be 1 if the array is greater than the other array" do
    example do
      [ 1, 2, 3, 4, 5, 6 ] <=> [ 1, 2 ]
    end.should == 1
  end
  
  specify "<=> should be 0 if the arrays are equal" do
    example do
      [] <=> []
    end.should == 0
  end
  
  specify "<=> should be -1 if the array is less than the other array" do
    example do
      [ "a", "a", "c" ] <=> [ "a", "b", "c" ]
    end.should == -1
  end
  
  specify "== should be true if each element is == to the corresponding element in the other array" do
    example do
      [ "a", "c", 7 ] == [ "a", "c", 7 ]
    end.should == true
  end
  
  specify "== should be false if any element is not == to the corresponding element in the other the array" do
    example do
      [ "a", "c" ] == [ "a", "c", 7 ]
    end.should == false
  end
  
  specify "[] should provide the element at the specified index" do
    example do
      [ "a", "b", "c", "d", "e" ][1]
    end.should == "b"
  end
  
  specify "[] should provide the element from the end of the array for a negative index" do
    example do
      [ "a", "b", "c", "d", "e" ][-2]
    end.should == "d"
  end
  
  specify "[] should provide a subarray from start containing length elements" do
    example do
      [ "a", "b", "c", "d", "e" ][2, 3]
    end.should == ["c", "d", "e"]
  end
  
  specify "[] should provide a subarray specified by range" do
    example do
      [ "a", "b", "c", "d", "e" ][1..3]
    end.should == ["b", "c", "d"]
  end
  
  specify "[] should not return nil if any elements in the requested range exist" do
    example do
      [ "a", "b", "c", "d", "e" ][4..7]
    end.should == ["e"]
  end
  
  specify "[] should provide nil for a requested index not in the array" do
    example do
      [ "a", "b", "c", "d", "e" ][5]
    end.should == nil
  end
  
  specify "[] should return nil if no requested index is in the array" do
    example do
      [ "a", "b", "c", "d", "e" ][6..10]
    end.should == nil
  end
  
  specify "[]= should set the value of the element at index" do
    example do
      a = [1, 2, 3, 4]
      a[2] = 5
      a[-1] = 6
      a[5] = 3
      a
    end.should == [1, 2, 5, 6, nil, 3]
  end
  
  specify "[]= should remove the section defined by start, length when set to nil" do
    example do
      a = ['a', 'b', 'c', 'd', 'e']
      a[1, 3] = nil
      a
    end.should == ["a", "e"]
  end
  
  specify "[]= should set the section defined by start, length to other" do
    example do
      a = [1, 2, 3, 4, 5, 6]
      a[0, 1] = 2
      a[3, 2] = ['a', 'b', 'c', 'd']
      a
    end.should == [2, 2, 3, "a", "b", "c", "d", 6]
  end
  
  specify "[]= should remove the section defined by range when set to nil" do
    example do
      a = [1, 2, 3, 4, 5]
      a[0..1] = nil
      a
    end.should == [3, 4, 5]
  end
  
  specify "[]= should set the section defined by range to other" do
    example do
      a = [6, 5, 4, 3, 2, 1]
      a[1...2] = 9
      a[3..6] = [6, 6, 6]
      a
    end.should == [6, 9, 4, 6, 6, 6]
  end
  
  specify "assoc should return the first contained array that the argument as the first element" do
    example do
      s1 = [ "colors", "red", "blue", "green" ] 
      s2 = [ "letters", "a", "b", "c" ] 
      s3 = "foo" 
      a = [ s1, s2, s3 ] 
      [a.assoc("letters"), a.assoc("foo")]
    end.should == [["letters", "a", "b", "c"], nil]
  end
  
  specify "at should return the element at index" do
    example do
      a = [1, 2, 3, 4, 5, 6]
      [a.at(0), a.at(-2), a.at(10)]
    end.should == [1, 5, nil]
  end
  
  specify "clear should remove all elements" do
    example do
      a = [1, 2, 3, 4]
      a.clear
      a
    end.should == []
  end
  
  specify "collect should return a copy of array with each element replaced by the value returned by block" do
    example do
      a = ['a', 'b', 'c', 'd']
      b = a.collect { |i| i + '!' }
      b
    end.should == ["a!", "b!", "c!", "d!"]
  end
  
  specify "collect! should replace each element with the value returned by block" do
    example do
      a = [7, 9, 3, 5]
      a.collect! { |i| i - 1 }
      a
    end.should == [6, 8, 2, 4]
  end
  
  specify "compact should return a copy of array with all nil elements removed" do
    example do
      a = [1, nil, 2, nil, 4, nil]
      a.compact
    end.should == [1, 2, 4]
  end
  
  specify "compact! should remove all nil elements" do
    example do
      a = ['a', nil, 'b', nil, nil, 'c']
      a.compact!
    end.should == ["a", "b", "c"]
  end
  
  specify "compact! should return nil if there are no nil elements to remove" do
    example do
      [1, 2, 3].compact!
    end.should == nil
  end
  
  specify "concat should append the elements in the other array" do
    example do
      [1, 2, 3].concat([9, 10, 11])
    end.should == [1, 2, 3, 9, 10, 11]
  end
  
  specify "delete should remove elements equal to object" do
    example do
      a = [1, 2, 3, 3, 4, 3, 5]
      a.delete(3)
      a
    end.should == [1, 2, 4, 5]
  end
  
  specify "delete should return nil if no elements match object" do
    example do
      [1, 2, 4, 5].delete(3)
    end.should == nil
  end
  
  specify "delete_at should remove the element at the specified index" do
    example do
      a = [1, 2, 3, 4]
      a.delete_at(2)
      a
    end.should == [1, 2, 4]
  end

  specify "delete_at should return the removed element at the specified index" do
    example do
      a = [1, 2, 3, 4]
      a.delete_at(2)
    end.should == 3
  end
  
  specify "delete_at should return nil if the index is out of range" do
    example do
      a = [1, 2]
      a.delete_at(3)
    end.should == nil
  end
  
  specify "delete_if should remove each element for which block returns true" do
    example do
      a = [ "a", "b", "c" ] 
      a.delete_if { |x| x >= "b" }
      a
    end.should == ["a"]
  end
  
  specify "each should yield each element to the block" do
    example do
      a = []
      [ 1,2,3 ].each { |item| a << item }
      a
    end.should == [1, 2, 3]
  end
  
  specify "each_index should pass the index of each element to the block" do
    example do
      a = []
      ['a', 'b', 'c', 'd'].each_index { |i| a << i }
      a
    end.should == [0, 1, 2, 3]
  end
  
  specify "empty? should return true if the array has no elements" do
    example do
      [[].empty?, [1].empty?, [1, 2].empty?]
    end.should == [true, false, false]
  end
  
  specify "eql? should return true if other is the same array" do
    example do
      a = []
      b = []
      [a.eql?(a), a.eql?(b)]
    end.should == [true, true]
  end
  
  specify "eql? should return true if other has the same length and elements" do
    example do
      a = [1, 2, 3, 4]
      b = [1, 2, 3, 4]
      c = [1, 2]
      d = ['a', 'b', 'c', 'd']
      [a.eql?(b), a.eql?(c), a.eql?(d)]
    end.should == [true, false, false]
  end

  specify "fetch should return the element at index" do
    example do
      [[1, 2, 3].fetch(1), [1, 2, 3, 4].fetch(-1)]
    end.should == [2, 4]
  end
  
  specify "fetch should raise if there is no element at index" do
    example do
      begin
        [1, 2, 3].fetch(3) 
      rescue IndexError
        a = "index 3 out of array"
      end
      a
    end.should == "index 3 out of array"
  end
  
  specify "fetch with default should return default if there is no element at index" do
    example do
      [1, 2, 3].fetch(5, nil)
    end.should == nil
  end

  specify "fetch with block should return the value of block if there is no element at index" do
    example do
      [1, 2, 3].fetch(9) { |i| i * i }
    end.should == 81
  end
  
  specify "fill should replace all elements in the array with object" do
    example do
      ['a', 'b', 'c', 'duh'].fill(8)
    end.should == [8, 8, 8, 8]
  end
  
  specify "fill with start, length should replace length elements beginning with start with object" do
    example do
      [1, 2, 3, 4, 5].fill('a', 2, 2)
    end.should == [1, 2, "a", "a", 5]
  end
  
  specify "fill with range should replace elements in range with object" do
    example do
      [1, 2, 3, 4, 5, 6].fill(8, 0..3)
    end.should == [8, 8, 8, 8, 5, 6]
  end
  
  specify "fill with block should replace all elements with the value of block" do
    example do
      [nil, nil, nil, nil].fill { |i| i * 2 }
    end.should == [0, 2, 4, 6]
  end
  
  specify "fill with start, length and block should replace length elements beginning with start with the value of block" do
    example do
      [true, false, true, false, true, false, true].fill(1, 4) { |i| i + 3 }
    end.should == [true, 4, 5, 6, 7, false, true]
  end
  
  specify "fill with range and block should replace all elements in range with the value of block" do
    example do
      [1, 1, 1, 1, 1, 1].fill(1..6) { |i| i + 1 }
    end.should == [1, 2, 3, 4, 5, 6, 7]
  end
  
  specify "first should return the first element" do
    example do
      [true, false, nil].first
    end.should == true
  end
  
  specify "first should return nil if self is empty" do
    example do
      [].first
    end.should == nil
  end
  
  specify "first with count should return the first count elements" do
    example do
      [true, false, true, nil, false].first(2)
    end.should == [true, false]
  end
  
  specify "flatten should return a one-dimensional flattening recursively" do
    example do
      [[[1, [2, 3]],[2, 3, [4, [4, [5, 5]], [1, 2, 3]]], [4]]].flatten
    end.should == [1, 2, 3, 2, 3, 4, 4, 5, 5, 1, 2, 3, 4]
  end
  
  specify "flatten! should modify array to produce a one-dimensional flattening recursively" do
    example do
      a = [[[1, [2, 3]],[2, 3, [4, [4, [5, 5]], [1, 2, 3]]], [4]]]
      a.flatten!
      a
    end.should == [1, 2, 3, 2, 3, 4, 4, 5, 5, 1, 2, 3, 4]
  end
  
  specify "frozen? should return true if array is frozen" do
    example do
      [1, 3, 2].freeze.frozen?
    end.should == true
  end
  
  specify "frozen? should return true if array is temporarily frozen while being sorted" do
    example do
      a = [1, 2, 3]
      b = []
      a.sort! { |x,y| b << a.frozen?; x <=> y }
      b
    end.should == [true, true]
  end
  
  specify "hash should be provided" do
    example do
      [].respond_to?(:hash)
    end.should == true
  end
  
  specify "include? should return true if object is present" do
    example do
      [1, 2, "a", "b"].include?("c")
    end.should == false
  end
  
  specify "index should return the index of the first element == to object" do
    example do
      [2, 1, 1, 1, 1].index(1)
    end.should == 1
  end
  
  specify "index should return nil if no element is == to object" do
    example do
      [2, 1, 1, 1, 1].index(3)
    end.should == nil
  end
  
  specify "indexes should be a DEPRECATED synonym for values_at" do
    example do
      [1, 2, 3, 4, 5].indexes(1, 0, 5, -1, -8, 10)
    end.should == [2, 1, nil, 5, nil, nil]
  end

  specify "indexes (DEPRECATED) with ranges should return an array of arrays of elements in the ranges" do
    example do
      [1, 2, 3, 4, 5].indexes(0..2, 1...3, 4..6)
    end.should == [[1, 2, 3], [2, 3], [5]]
  end
  
  specify "indices should be a DEPRECATED synonym for values_at" do
    example do
      [1, 2, 3, 4, 5].indices(1, 0, 5, -1, -8, 10)
    end.should == [2, 1, nil, 5, nil, nil]
  end
  
  specify "indices (DEPRECATED) with ranges should return an array of arrays of elements in the ranges" do
    example do
      [1, 2, 3, 4, 5].indices(0..2, 1...3, 4..6)
    end.should == [[1, 2, 3], [2, 3], [5]]
  end
  
  specify "initialize_copy should be a synonym for replace" do
    example do
      [1, 2, 3, 4, 5].send(:initialize_copy, ['a', 'b', 'c'])
    end.should == ["a", "b", "c"]
  end
  
  specify "insert with non-negative index should insert object before the element at index" do
    example do
      [1, 2, 3].insert(1, 'a')
    end.should == [1, "a", 2, 3]
  end
  
  specify "insert with index -1 should append object to the end" do
    example do
      [1, 3, 3].insert(-1, 2)
    end.should == [1, 3, 3, 2]
  end

  specify "insert with negative index should insert object after the element at index" do
    example do
      [3, 3, 3].insert(-2, -3)
    end.should == [3, 3, -3, 3]
  end
  
  specify "inspect should create a printable representation" do
    example do
      [1, 2, 3].inspect
    end.should == "[1, 2, 3]"
  end
  
  specify "join should return a string formed by concatentating each element separated by separator" do
    example do
      [1, 2, 3, 4].join(' | ')
    end.should == "1 | 2 | 3 | 4"
  end
  
  specify "last should return the last element" do
    example do
      [1, 1, 1, 1, 2].last
    end.should == 2
  end
  
  specify "last should return nil if self is empty" do
    example do
      [].last
    end.should == nil
  end
  
  specify "last with count should return the last count elements" do
    example do
      [1, 2, 3, 4, 5, 9].last(3)
    end.should == [4, 5, 9]
  end
  
  specify "length should return the number of elements" do
    example do
      [1, 2, 3].length
    end.should == 3
  end
  
  specify "map should be a synonym for collect" do
    example do
      a = ['a', 'b', 'c', 'd']
      b = a.collect { |i| i + '!' }
      b
    end.should == ["a!", "b!", "c!", "d!"]
  end
  
  specify "map! should be a synonym for collect!" do
    example do
      a = [7, 9, 3, 5]
      a.collect! { |i| i - 1 }
      a
    end.should == [6, 8, 2, 4]
  end
  
  specify "nitems should return the number of non-nil elements" do
    example do
      [1, nil, 2, 3, nil, nil, 4].nitems
    end.should == 4
  end
  
  specify "pack should return a binary representation of the elements according to template" do
    example do
    end.should == "not implemented"
  end
  
  specify "partition should return two arrays" do
    example do
      [].partition
    end.should == [[], []]
  end
  
  specify "partition should return in the left array values for which the block evaluates to true" do
    example do
      [ 0,1,2,3,4,5 ].partition { |i| i % 2 == 0 }
    end.should == [[0, 2, 4], [1, 3, 5]]
  end
  
  specify "pop should remove and return the last element of the array" do
    example do
      a = ["a", 1, nil, true]
      [a.pop, a.length]
    end.should == [true, 3]
  end
  
  specify "pop should return nil if there are no more elements" do
    example do
      [].pop
    end.should == nil
  end
  
  specify "push should append the arguments to the array" do
    example do
      a = [ "a", "b", "c" ]
      a.push("d", "e", "f")
      a
    end.should == ["a", "b", "c", "d", "e", "f"]
  end
  
  specify "rassoc should return the first contained array whose second element is == object" do
    example do
      [[1, "a"], [2, "b"], [3, "b"], [4, "c"]].rassoc("b")
    end.should == [2, "b"]
  end
  
  specify "reject should return a new array of all elements for which block is false" do
    example do
      [1, 2, 3, 4, 5].reject { |i| i < 3 }
    end.should == [3, 4, 5]
  end
  
  specify "reject! should remove elements for which block is true" do
    example do
      a = [3, 4, 5, 6, 7, 8, 9, 10, 11]
      a.reject! { |i| i % 2 == 0 }
      a
    end.should == [3, 5, 7, 9, 11]
  end
  
  specify "reject! should return nil if no changes are made" do
    example do
      [1, 2, 3].reject! { |i| i < 0 }
    end.should == nil
  end
  
  specify "replace should replace the elements with elements from other array" do
    example do
      [1, 2, 3, 4, 5].replace(['a', 'b', 'c'])
    end.should == ["a", "b", "c"]
  end
  
  specify "reverse should return a new array with the elements in reverse order" do
    example do
      [1, 3, 5, 2].reverse
    end.should == [2, 5, 3, 1]
  end
  
  specify "reverse! should reverse the elements in place" do
    example do
      a = [6, 3, 4, 2, 1]
      a.reverse!
      a
    end.should == [1, 2, 4, 3, 6]
  end
  
  specify "reverse_each should traverse array in reverse order and pass each element to block" do
    example do
      a = []
      [1, 3, 4, 6].reverse_each { |i| a << i }
      a
    end.should == [6, 4, 3, 1]
  end
  
  specify "rindex should return the last index where element is == object" do
    example do
      [1, 1, 3, 2, 1, 3].rindex(3)
    end.should == 5
  end
  
  specify "rindex should return nil if no element is found" do
    example do
      [1, 1, 3, 2, 1, 3].rindex(4)
    end.should == nil
  end
  
  specify "select should return a new array of elements for which block is true" do
    example do
      [1, 3, 4, 5, 6, 9].select { |i| i % ((i + 1) / 2) == 0}
    end.should == [1, 4, 6]
  end
  
  specify "shift should return the first element" do
    example do
      [5, 1, 1, 5, 4].shift
    end.should == 5
  end
  
  specify "shift should remove the first element" do
    example do
      a = [5, 1, 1, 5, 4]
      a.shift
      a
    end.should == [1, 1, 5, 4]
  end
  
  specify "shift should return nil when the array is empty" do
    example do
      [].shift
    end.should == nil
  end
  
  specify "size should be a synonym for length" do
    example do
      [1, 2, 3].length
    end.should == 3
  end
  
  specify "slice should be a synonym for []" do
    example do
    end.should == "not implemented"
  end
  
  specify "slice! with index should remove and return the element at index" do
    example do
      a = [1, 2, 3, 4]
      b = a.slice!(2)
      [a, b]
    end.should == [[1, 2, 4], 3]
  end
  
  specify "slice! with start, length should remove and return length elements beginning at start" do
    example do
      a = [0, 1, 1, 2, 3, 5, 8]
      b = a.slice!(2, 3)
      [a, b]
    end.should == [[0, 1, 5, 8], [1, 2, 3]]
  end

  specify "slice! with range should remove and return elements in range" do
    example do
      a = [1, 1, 2, 2, 3, 3, 4, 5]
      b = a.slice!(1...4)
      [a, b]
    end.should == [[1, 3, 3, 4, 5], [1, 2, 2]]
  end
  
  specify "sort should return a new array from sorting elements using <=>" do
    example do
      [1, 1, 5, -5, 2, -10, 14, 6].sort
    end.should == [-10, -5, 1, 1, 2, 5, 6, 14]
  end
  
  specify "sort with block should return a new array from sorting elements using block value" do
    example do
      a = [1, 2, 3, 4, 5]
      [a.sort { |x,y| y <=> (x - 1) }, a]
    end.should == [[5, 4, 3, 2, 1], [1, 2, 3, 4, 5]]
  end
  
  specify "sort! should sort array in place using <=>" do
    example do
      a = [1, 9, 7, 11, -1, -4]
      a.sort!
      a
    end.should == [-4, -1, 1, 7, 9, 11]
  end
  
  specify "sort! should sort array in place using block value" do
    example do
      a = [3, 4, 5, 9, 10]
      a.sort! { |x, y| y <=> x }
      a
    end.should == [10, 9, 5, 4, 3]
  end
  
  specify "to_a should return the array if it is an instance of Array" do
    example do
      [1, 2, 3].to_a
    end.should == [1, 2, 3]
  end
  
  specify "to_a called on a subclass of Array should return an instance of Array" do
    example do
      class Ary < Array; end

      Ary.new.to_a.class
    end.should == Array
  end
  
  specify "to_ary should return self" do
    example do
      a = [1, 2, 3]
      a.equal?(a.to_ary)
    end.should == true
  end
  
  specify "to_s should return the result of calling join on self" do
    example do
      [1, 2, 3, 4].to_s
    end.should == "1234"
  end
  
  specify "transpose assumes an array of arrays and should return the result of transposing rows and columns" do
    example do
      [[1, 'a'], [2, 'b'], [3, 'c']].transpose
    end.should == [[1, 2, 3], ["a", "b", "c"]]
  end
  
  specify "uniq should return an array with no duplicates" do
    example do
      [ "a", "a", "b", "b", "c" ].uniq
    end.should == ["a", "b", "c"]
  end
  
  specify "uniq! should return the same array" do
    example do
      a = [ "a", "a", "b", "b", "c" ]
      a.equal?(a.uniq!)
    end.should == true
  end
  
  specify "uniq! should return the array with no duplicates" do
    example do
      [ "a", "a", "b", "b", "c" ].uniq!
    end.should == ["a", "b", "c"]
  end
  
  specify "uniq! should return nil if no changes are made to the array" do
    example do
      [ "a", "b", "c" ].uniq!
    end.should == nil
  end
  
  specify "unshift should prepend object" do
    example do
      [1, 2, 3].unshift("a")
    end.should == ["a", 1, 2, 3]
  end
  
  specify "values_at with indexes should return an array of elements at the indexes" do
    example do
      [1, 2, 3, 4, 5].values_at(1, 0, 5, -1, -8, 10)
    end.should == [2, 1, nil, 5, nil, nil]
  end
  
  specify "values_at with ranges should return an array of elements in the ranges" do
    example do
      [1, 2, 3, 4, 5].values_at(0..2, 1...3, 4..6)
    end.should == [1, 2, 3, 2, 3, 5, nil]
  end
  
  specify "zip should return an array of arrays containing cooresponding elements of each array" do
    example do
      [1, 2, 3, 4, 5].zip(["a", "b", "c", "d"])
    end.should == [[1, "a"], [2, "b"], [3, "c"], [4, "d"], [5, nil]]
  end
  
  specify "| should return an array of elements common to both arrays (union)" do
    example do
      [1, 2, 3] | [1, 1, 2, 3, 4, 4, 5]
    end.should == [1, 2, 3, 4, 5]
  end
  
  specify "| should return an array with no duplicates" do
    example do
      a = [1, 2, 3] | [1, 1, 2, 3, 4, 4, 5]
      a.uniq!
    end.should == nil
  end
end

# regression tests
context "Array Bugs" do
  # per irc conversation between Defiler, evan, mae(listening)
  specify "[1,2,3][2..-1] should return [3]" do
    example do
      [1,2,3][2..-1]
    end.should == [3]
  end
end

