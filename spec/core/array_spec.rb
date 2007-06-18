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

class MyArray < Array; end
class ToAryArray < Array
  def to_ary() ["to_ary", "was", "called!"] end
end

context "Array class method" do  
  specify "new without arguments should return a new array" do
    a = Array.new
    a.class.should == Array
    b = MyArray.new
    b.class.should == MyArray
  end
  
  specify "new with size should return a new array of size with nil elements" do
    Array.new(5).should == [nil, nil, nil, nil, nil]
    a = MyArray.new(5)
    a.class.should == MyArray
    a.inspect.should == [nil, nil, nil, nil, nil].inspect
  end

  specify "new should call to_int on size" do
    obj = Object.new
    def obj.to_int() 3 end
    Array.new(obj).should == [nil, nil, nil]
  end
  
  specify "new with size and default object should return a new array of size objects" do
    Array.new(4, true).should == [true, true, true, true]

    # Shouldn't copy object
    str = "x"
    ary = Array.new(4, str)
    str << "y"
    ary.should == [str, str, str, str]

    a = MyArray.new(4, true)
    a.class.should == MyArray
    a.inspect.should == [true, true, true, true].inspect
  end
  
  specify "new with array-like argument should return a new array by calling to_ary on argument" do
    obj = Object.new
    def obj.to_ary() [:foo] end
    Array.new(obj).should == [:foo]
    
    a = MyArray.new(obj)
    a.class.should == MyArray
    a.inspect.should == [:foo].inspect
  end
  
  specify "new with array-like argument shouldn't call to_ary on array subclasses" do
    Array.new(ToAryArray[5, 6, 7]).should == [5, 6, 7]
  end
  
  specify "new should call to_ary before to_int" do
    obj = Object.new
    def obj.to_ary() [1, 2, 3] end
    def obj.to_int() 3 end

    Array.new(obj).should == [1, 2, 3]
  end
  
  specify "new with size and block should return an array of size elements from the result of passing each index to block" do
    Array.new(5) { |i| i + 1 }.should == [1, 2, 3, 4, 5]
    
    a = MyArray.new(5) { |i| i + 1 }
    a.class.should == MyArray
    a.inspect.should == [1, 2, 3, 4, 5].inspect
  end
  
  specify ".[] should return a new array populated with the given elements" do
    Array.[](5, true, nil, 'a', "Ruby").should == [5, true, nil, "a", "Ruby"]

    a = MyArray.[](5, true, nil, 'a', "Ruby")
    a.class.should == MyArray
    a.inspect.should == [5, true, nil, "a", "Ruby"].inspect
  end

  specify "[] should be a synonym for .[]" do
    Array[5, true, nil, 'a', "Ruby"].should == [5, true, nil, "a", "Ruby"]

    a = MyArray[5, true, nil, 'a', "Ruby"]
    a.class.should == MyArray
    a.inspect.should == [5, true, nil, "a", "Ruby"].inspect
  end
end

context "Array instance method" do
  specify "& should create an array with elements common to both arrays (intersection)" do
    ([] & []).should == []
    ([1, 2] & []).should == []
    ([] & [1, 2]).should == []
    ([ 1, 1, 3, 5 ] & [ 1, 2, 3 ]).should == [1, 3]
  end
  
  specify "& should create an array with no duplicates" do
    ([] | []).should == []
    ([1, 2] | []).should == [1, 2]
    ([] | [1, 2]).should == [1, 2]
    ([ 1, 1, 3, 5 ] & [ 1, 2, 3 ]).uniq!.should == nil
  end
  
  specify "& does not modify the original Array" do
    a = [1, 1, 3, 5]
    a & [1, 2, 3]
    a.should == [1, 1, 3, 5]
  end

  specify "& should call to_ary on its argument" do
    obj = Object.new
    def obj.to_ary() [1, 2, 3] end
    ([1, 2] & obj).should == ([1, 2] & obj.to_ary)
  end

  # MRI doesn't actually call eql?() however. So you can't reimplement it.
  specify "& should act as if using eql?" do
    ([5.0, 4.0] & [5, 4]).should == []
    str = "x"
    ([str] & [str.dup]).should == [str]
  end
  
  specify "& with array subclasses shouldn't return subclass instance" do
    (MyArray[1, 2, 3] & []).class.should == Array
    (MyArray[1, 2, 3] & MyArray[1, 2, 3]).class.should == Array
    ([] & MyArray[1, 2, 3]).class.should == Array
  end

  specify "& shouldn't call to_ary on array subclasses" do
    ([5, 6] & ToAryArray[1, 2, 5, 6]).should == [5, 6]
  end
  
  specify "| should return an array of elements that appear in either array (union) without duplicates" do
    ([1, 2, 3] | [1, 2, 3, 4, 5]).should == [1, 2, 3, 4, 5]
  end

  specify "| should call to_ary on its argument" do
    obj = Object.new
    def obj.to_ary() [1, 2, 3] end
    ([0] | obj).should == ([0] | obj.to_ary)
  end

  # MRI doesn't actually call eql?() however. So you can't reimplement it.
  specify "| should act as if using eql?" do
    ([5.0, 4.0] | [5, 4]).should == [5.0, 4.0, 5, 4]
    str = "x"
    ([str] | [str.dup]).should == [str]
  end
  
  specify "| with array subclasses shouldn't return subclass instance" do
    (MyArray[1, 2, 3] | []).class.should == Array
    (MyArray[1, 2, 3] | MyArray[1, 2, 3]).class.should == Array
    ([] | MyArray[1, 2, 3]).class.should == Array
  end
  
  specify "| shouldn't call to_ary on array subclasses" do
    ([1, 2] | ToAryArray[5, 6]).should == [1, 2, 5, 6]
  end
  
  specify "* with a string should be equivalent to self.join(str)" do
    ([ 1, 2, 3 ] * ",").should == [1, 2, 3].join(",")
  end

  specify "* with a string should handle recursive arrays just like join" do
    x = []
    x << x
    (x * ":").should == x.join(":")

    x = []
    y = []
    y << 9 << x << 8 << y << 7
    x << 1 << x << 2 << y << 3
    (x * ":").should == x.join(":")
  end

  specify "* should call to_str on its argument just like join" do
    obj = Object.new
    def obj.to_str() "x" end
    ([1, 2, 3] * obj).should == [1, 2, 3].join(obj)
  end
  
  specify "* with an int should concatenate n copies of the array" do
    ([ 1, 2, 3 ] * 0).should == []
    ([ 1, 2, 3 ] * 1).should == [1, 2, 3]
    ([ 1, 2, 3 ] * 3).should == [1, 2, 3, 1, 2, 3, 1, 2, 3]
    ([] * 10).should == []
  end
  
  specify "* with a negative int should raise an ArgumentError" do
    should_raise(ArgumentError) { [ 1, 2, 3 ] * -1 }
    should_raise(ArgumentError) { [] * -1 }
  end
  
  specify "* should call to_int on its argument" do
    obj = Object.new
    def obj.to_int() 2 end

    ([1, 2, 3] * obj).should == [1, 2, 3] * obj.to_int
  end

  specify "* should call to_str on its argument before to_int" do
    obj = Object.new
    def obj.to_int() 2 end
    def obj.to_str() "x" end
    ([1, 2, 3] * obj).should == [1, 2, 3] * obj.to_str
  end

  specify "* on array subclass should return subclass instance" do
    (MyArray[1, 2, 3] * 0).class.should == MyArray
    (MyArray[1, 2, 3] * 1).class.should == MyArray
    (MyArray[1, 2, 3] * 2).class.should == MyArray
  end

  specify "* raises if the argument can neither be converted to a string nor an integer" do
    should_raise(TypeError) { [1, 2] * Object.new }
  end
  
  specify "+ should concatenate two arrays" do
    ([ 1, 2, 3 ] + [ 3, 4, 5 ]).should == [1, 2, 3, 3, 4, 5]
    ([ 1, 2, 3 ] + []).should == [1, 2, 3]
    ([] + [ 1, 2, 3 ]).should == [1, 2, 3]
    ([] + []).should == []
  end

  specify "+ should call to_ary on its argument" do
    obj = Object.new
    def obj.to_ary() ["x", "y"] end
    ([1, 2, 3] + obj).should == [1, 2, 3] + obj.to_ary
  end

  specify "+ with array subclasses shouldn't return subclass instance" do
    (MyArray[1, 2, 3] + []).class.should == Array
    (MyArray[1, 2, 3] + MyArray[]).class.should == Array
    ([1, 2, 3] + MyArray[]).class.should == Array
  end

  specify "+ shouldn't call to_ary on array subclasses" do
    ([5, 6] + ToAryArray[1, 2]).should == [5, 6, 1, 2]
  end

  specify "- should create an array minus any items from other array" do
    ([] - [ 1, 2, 4 ]).should == []
    ([1, 2, 4] - []).should == [1, 2, 4]
    ([ 1, 2, 3, 4, 5 ] - [ 1, 2, 4 ]).should == [3, 5]
  end

  specify "- removes multiple items on the lhs equal to one on the rhs" do
    ([1, 1, 2, 2, 3, 3, 4, 5] - [1, 2, 4]).should == [3, 3, 5]
  end

  specify "- should call to_ary on its argument" do
    obj = Object.new
    def obj.to_ary() [2, 3, 3, 4] end
    ([1, 1, 2, 2, 3, 4] - obj).should == [1, 1]
  end

  specify "- with array subclasses shouldn't return subclass instance" do
    (MyArray[1, 2, 3] - []).class.should == Array
    (MyArray[1, 2, 3] - MyArray[]).class.should == Array
    ([1, 2, 3] - MyArray[]).class.should == Array
  end

  specify "- shouldn't call to_ary on array subclasses" do
    ([5, 6, 7] - ToAryArray[7]).should == [5, 6]
  end
  
  specify "<< should push the object onto the end of the array" do
    ([ 1, 2 ] << "c" << "d" << [ 3, 4 ]).should == [1, 2, "c", "d", [3, 4]]
  end
  
  specify "<=> should call <=> left to right and return first non-0 result" do
    [-1, +1, nil, "foobar"].each do |result|
      lhs = Array.new(3) { Object.new }
      rhs = Array.new(3) { Object.new }
    
      lhs[0].should_receive(:<=>, :with => [rhs[0]], :returning => 0)
      lhs[1].should_receive(:<=>, :with => [rhs[1]], :returning => result)
      lhs[2].should_not_receive(:<=>)

      (lhs <=> rhs).should == result
    end
  end
  
  specify "<=> should be 0 if the arrays are equal" do
    ([] <=> []).should == 0
    ([1, 2, 3, 4, 5, 6] <=> [1, 2, 3, 4, 5.0, 6.0]).should == 0
  end
  
  specify "<=> should be -1 if the array is shorter than the other array" do
    ([] <=> [1]).should == -1
    ([1, 1] <=> [1, 1, 1]).should == -1
  end

  specify "<=> should be +1 if the array is longer than the other array" do
    ([1] <=> []).should == +1
    ([1, 1, 1] <=> [1, 1]).should == +1
  end

  specify "<=> should call to_ary on its argument" do
    obj = Object.new
    def obj.to_ary() [1, 2, 3] end
    ([4, 5] <=> obj).should == ([4, 5] <=> obj.to_ary)
  end

  specify "<=> shouldn't call to_ary on array subclasses" do
    ([5, 6, 7] <=> ToAryArray[5, 6, 7]).should == 0
  end
  
  specify "== should be true if each element is == to the corresponding element in the other array" do
    [].should == []
    ["a", "c", 7].should == ["a", "c", 7]

    obj = Object.new
    def obj.==(other) true end
    [obj].should == [5]
  end
  
  specify "== should be false if any element is not == to the corresponding element in the other the array" do
    ([ "a", "c" ] == [ "a", "c", 7 ]).should == false
  end
  
  specify "== should be instantly false when sizes of the arrays differ" do
    obj = Object.new
    obj.should_not_receive(:==)
    
    [].should_not == [obj]
    [obj].should_not == []
  end

  # Broken in MRI as well. See MRI bug #11585:
  # http://rubyforge.org/tracker/index.php?func=detail&aid=11585&group_id=426&atid=1698
  specify "== should call to_ary on its argument" do
    obj = Object.new
    obj.should_receive(:to_ary, :returning => [1, 2, 3])
    
    [1, 2, 3].should == obj
  end
  
  specify "== shouldn't call to_ary on array subclasses" do
    ([5, 6, 7] == ToAryArray[5, 6, 7]).should == true
  end

  specify "== should ignore array class differences" do
    MyArray[1, 2, 3].should == [1, 2, 3]
    MyArray[1, 2, 3].should == MyArray[1, 2, 3]
    [1, 2, 3].should == MyArray[1, 2, 3]
  end

  specify "assoc should return the first array whose 1st item is == obj or nil" do
    s1 = ["colors", "red", "blue", "green"] 
    s2 = [:letters, "a", "b", "c"]
    s3 = [4]
    s4 = [nil, nil]
    a = [s1, s2, s3, s4]
    a.assoc(s1.first).should == s1
    a.assoc(s2.first).should == s2
    a.assoc(s3.first).should == s3
    a.assoc(s4.first).should == s4
    a.assoc(4).should == [4]
    a.assoc("key not in array").should == nil
  end

  specify "assoc should call == on argument" do
    key = Object.new
    items = Array.new(3) { [Object.new, "foo"] }
    items[0][0].should_receive(:==, :with => [key], :returning => false)
    items[1][0].should_receive(:==, :with => [key], :returning => true)
    items[2][0].should_not_receive(:==, :with => [key])
    items.assoc(key).should == items[1]
  end
  
  specify "assoc should ignore any non-Array elements" do
    [1, 2, 3].assoc(2).should == nil
    s1 = [4]
    s2 = [5, 4, 3]
    a = ["foo", [], s1, s2, nil, []] 
    a.assoc(s1.first).should == s1
    a.assoc(s2.first).should == s2
  end

  specify "at should return the element at index" do
    a = [1, 2, 3, 4, 5, 6]
    a.at(0).should  == 1
    a.at(-2).should == 5
    a.at(10).should == nil
  end

  specify "at should call to_int on its argument" do
    a = ["a", "b", "c"]
    a.at(0.5).should == "a"
    
    obj = Object.new
    obj.should_receive(:to_int, :returning => 2)
    a.at(obj).should == "c"
  end
  
  specify "class should return Array" do
    [].class.should == Array
  end
  
  specify "clear should remove all elements" do
    a = [1, 2, 3, 4]
    a.clear.equal?(a).should == true
    a.should == []
  end
  
  specify "collect should return a copy of array with each element replaced by the value returned by block" do
    a = ['a', 'b', 'c', 'd']
    b = a.collect { |i| i + '!' }
    b.should == ["a!", "b!", "c!", "d!"]
  end
  
  specify "collect on array subclasses shouldn't return subclass instance" do
    MyArray[1, 2, 3].collect { |x| x + 1 }.class.should == Array
  end
  
  specify "collect! should replace each element with the value returned by block" do
    a = [7, 9, 3, 5]
    a.collect! { |i| i - 1 }.equal?(a).should == true
    a.should == [6, 8, 2, 4]
  end
  
  specify "compact should return a copy of array with all nil elements removed" do
    a = [1, nil, 2, nil, 4, nil]
    a.compact.should == [1, 2, 4]
  end

  specify "compact on array subclasses should return subclass instance" do
    MyArray[1, 2, 3, nil].compact.class.should == MyArray
  end
  
  specify "compact! should remove all nil elements" do
    a = ['a', nil, 'b', nil, nil, false, 'c', nil]
    a.compact!.equal?(a).should == true
    a.should == ["a", "b", false, "c"]
  end
  
  specify "compact! should return nil if there are no nil elements to remove" do
    [1, 2, false, 3].compact!.should == nil
  end
  
  specify "concat should append the elements in the other array" do
    ary = [1, 2, 3]
    ary.concat([9, 10, 11]).equal?(ary).should == true
    ary.should == [1, 2, 3, 9, 10, 11]
    ary.concat([])
    ary.should == [1, 2, 3, 9, 10, 11]
  end
  
  specify "concat shouldn't loop endlessly when argument is self" do
    ary = ["x", "y"]
    ary.concat(ary).should == ["x", "y", "x", "y"]
  end  

  specify "concat should call to_ary on its argument" do
    obj = Object.new
    def obj.to_ary() ["x", "y"] end
    [4, 5, 6].concat(obj).should == [4, 5, 6, "x", "y"]
  end
  
  specify "concat shouldn't call to_ary on array subclasses" do
    [].concat(ToAryArray[5, 6, 7]).should == [5, 6, 7]
  end
  
  specify "delete removes elements that are #== to object" do
    x = Object.new
    def x.==(other) 3 == other end

    a = [1, 2, 3, x, 4, 3, 5, x]
    a.delete Object.new
    a.should == [1, 2, 3, x, 4, 3, 5, x]

    a.delete 3
    a.should == [1, 2, 4, 5]
  end

  specify "delete should return object or nil if no elements match object" do
    [1, 2, 4, 5].delete(1).should == 1
    [1, 2, 4, 5].delete(3).should == nil
  end

  specify 'delete may be given a block that is executed if no element matches object' do
    [].delete('a') {:not_found}.should == :not_found
  end
  
  specify "delete_at should remove the element at the specified index" do
    a = [1, 2, 3, 4]
    a.delete_at(2)
    a.should == [1, 2, 4]
    a.delete_at(-1)
    a.should == [1, 2]
  end

  specify "delete_at should return the removed element at the specified index" do
    a = [1, 2, 3, 4]
    a.delete_at(2).should == 3
    a.delete_at(-1).should == 4
  end
  
  specify "delete_at should return nil if the index is out of range" do
    a = [1, 2]
    a.delete_at(3).should == nil
  end

  specify "delete_at should call to_int on its argument" do
    obj = Object.new
    def obj.to_int() -1 end
    [1, 2].delete_at(obj).should == 2
  end
  
  specify "delete_at may take negative indices" do
    a = [1, 2]
    a.delete_at(-2).should == 1
  end
  
  specify "delete_if should remove each element for which block returns true" do
    a = [ "a", "b", "c" ] 
    a.delete_if { |x| x >= "b" }.equal?(a).should == true
    a.should == ["a"]
  end
  
  specify "each should yield each element to the block" do
    a = []
    x = [1, 2, 3]
    x.each { |item| a << item }.equal?(x).should == true
    a.should == [1, 2, 3]
  end
  
  specify "each_index should pass the index of each element to the block" do
    a = []
    x = ['a', 'b', 'c', 'd']
    x.each_index { |i| a << i }.equal?(x).should == true
    a.should == [0, 1, 2, 3]
  end
  
  specify "empty? should return true if the array has no elements" do
    [].empty?.should == true
    [1].empty?.should == false
    [1, 2].empty?.should == false
  end
  
  specify "eql? should return true if other is the same array" do
    a, b = [1], [2]

    a.eql?(b).should == false
    a.eql?(a).should == true
  end
  
  specify "eql? should return true if other has the same length and elements" do
    a = [1, 2, 3, 4]
    b = [1, 2, 3, 4]
    c = [1, 2]
    d = ['a', 'b', 'c', 'd']

    a.eql?(b).should == true
    a.eql?(c).should == false
    a.eql?(d).should == false
    [].eql?([]).should == true
  end

  specify "eql? should ignore array class differences" do
    MyArray[1, 2, 3].eql?([1, 2, 3]).should == true
    MyArray[1, 2, 3].eql?(MyArray[1, 2, 3]).should == true
    [1, 2, 3].eql?(MyArray[1, 2, 3]).should == true
  end

  specify "fetch should return the element at index" do
    [1, 2, 3].fetch(1).should == 2
    [nil].fetch(0).should == nil
  end

  specify "fetch counts negative indices backwards from end" do
    [1, 2, 3, 4].fetch(-1).should == 4
  end
  
  specify "fetch should raise if there is no element at index" do
    should_raise(IndexError) { [1, 2, 3].fetch(3) }
    should_raise(IndexError) { [1, 2, 3].fetch(-4) }
    should_raise(IndexError) { [].fetch(0) }
  end
  
  specify "fetch with default should return default if there is no element at index" do
    [1, 2, 3].fetch(5, :not_found).should == :not_found
    [1, 2, 3].fetch(5, nil).should == nil
    [1, 2, 3].fetch(-4, :not_found).should == :not_found
    [nil].fetch(0, :not_found).should == nil
  end

  specify "fetch with block should return the value of block if there is no element at index" do
    [1, 2, 3].fetch(9) { |i| i * i }.should == 81
  end

  specify "fetch's default block takes precedence over its default argument" do
    [1, 2, 3].fetch(9, :foo) { |i| i * i }.should == 81
  end

  specify "fetch should call to_int on its argument" do
    x = Object.new
    def x.to_int() 0 end
    [1, 2, 3].fetch(x).should == 1
  end
  
  specify "fill should replace all elements in the array with object" do
    ary = ['a', 'b', 'c', 'duh']
    ary.fill(8).should == [8, 8, 8, 8]

    str = "x"
    ary.fill(str).should == [str, str, str, str]
    str << "y"
    ary.should == [str, str, str, str]
  end
  
  specify "fill with start, length should replace length elements beginning with start with object" do
    [1, 2, 3, 4, 5].fill('a', 2, 2).should == [1, 2, "a", "a", 5]
    [1, 2, 3, 4, 5].fill('a', 2, 5).should == [1, 2, "a", "a", "a", "a", "a"]
    [1, 2, 3, 4, 5].fill('a', 2, -2).should == [1, 2, 3, 4, 5]
    [1, 2, 3, 4, 5].fill('a', 2, 0).should == [1, 2, 3, 4, 5]

    [1, 2, 3, 4, 5].fill('a', -2, 2).should == [1, 2, 3, "a", "a"]    
    [1, 2, 3, 4, 5].fill('a', -2, 4).should == [1, 2, 3, "a", "a", "a", "a"]    
    [1, 2, 3, 4, 5].fill('a', -2, -2).should == [1, 2, 3, 4, 5]
    [1, 2, 3, 4, 5].fill('a', -2, 0).should == [1, 2, 3, 4, 5]
  end
  
  specify "fill should call to_int on start and length" do
    x = Object.new
    def x.to_int() 2 end
    
    [1, 2, 3, 4, 5].fill('a', x, x).should == [1, 2, "a", "a", 5]
  end

  specify "fill with negative index past the beginning of Array starts at 0" do
    [1, 2, 3, 4, 5].fill('a', -25, 3).should == ['a', 'a', 'a', 4, 5]
    [1, 2, 3, 4, 5].fill('a', -10, 10).should == %w|a a a a a a a a a a|
  end
  
  specify "fill with an index and a count of < 1 doesn't change the Array" do
    [1, 2, 3].fill('a', 1, -3).should == [1, 2, 3]
    [1, 2, 3].fill('a', 1, 0).should == [1, 2, 3]
  end

  specify "fill with range should replace elements in range with object" do
    [1, 2, 3, 4, 5, 6].fill(8, 0..3).should == [8, 8, 8, 8, 5, 6]
    [1, 2, 3, 4, 5, 6].fill(8, 0...3).should == [8, 8, 8, 4, 5, 6]
    [1, 2, 3, 4, 5, 6].fill('x', 4..6).should == [1, 2, 3, 4, 'x', 'x', 'x']
    [1, 2, 3, 4, 5, 6].fill('x', 4...6).should == [1, 2, 3, 4, 'x', 'x']
    [1, 2, 3, 4, 5, 6].fill('x', -2..-1).should == [1, 2, 3, 4, 'x', 'x']
    [1, 2, 3, 4, 5, 6].fill('x', -2...-1).should == [1, 2, 3, 4, 'x', 6]
    [1, 2, 3, 4, 5, 6].fill('x', -2...-2).should == [1, 2, 3, 4, 5, 6]
    [1, 2, 3, 4, 5, 6].fill('x', -2..-2).should == [1, 2, 3, 4, 'x', 6]
    [1, 2, 3, 4, 5, 6].fill('x', -2..0).should == [1, 2, 3, 4, 5, 6]
    [1, 2, 3, 4, 5, 6].fill('x', 0...0).should == [1, 2, 3, 4, 5, 6]
    [1, 2, 3, 4, 5, 6].fill('x', 1..1).should == [1, 'x', 3, 4, 5, 6]
  end
  
  specify "fill with block should replace all elements with the value of block (index given to block)" do
    [nil, nil, nil, nil].fill { |i| i * 2 }.should == [0, 2, 4, 6]
  end
  
  specify "fill with start, length and block should replace length elements beginning with start with the value of block" do
    [true, false, true, false, true, false, true].fill(1, 4) { |i| i + 3 }.should == [true, 4, 5, 6, 7, false, true]
  end
  
  specify "fill with range and block should replace all elements in range with the value of block" do
    [1, 1, 1, 1, 1, 1].fill(1..6) { |i| i + 1 }.should == [1, 2, 3, 4, 5, 6, 7]
  end

  specify "fill increases the Array size when necessary" do
    a = [1, 2, 3]
    a.size.should == 3
    a.fill 'a', 0, 10
    a.size.should == 10 
  end

  specify "fill raises ArgumentError if the wrong number of arguments is given" do
    should_raise(ArgumentError) { [].fill('a', 1, 2, true) }
    should_raise(ArgumentError) { [].fill('a', 1, true) {|i|} }
  end

  specify "fill raises TypeError if the index is not numeric" do
    should_raise(TypeError) { [].fill 'a', true }
  end

  specify "fill with range and length argument raises TypeError" do
    should_raise(TypeError) { [].fill('x', 0 .. 2, 5) }
  end

  specify "fill with nil length acts as if length argument isn't supplied" do
    a = [1, 2, 3]
    a.fill('x', 1, nil)
    a.should == [1, 'x', 'x']
  end
  
  specify "first should return the first element" do
    %w{a b c}.first.should == 'a'
    [nil].first.should == nil
  end
  
  specify "first should return nil if self is empty" do
    [].first.should == nil
  end
  
  specify "first with count should return the first count elements" do
    [true, false, true, nil, false].first(2).should == [true, false]
  end
  
  specify "first with count == 0 should return an empty array" do
    [1, 2, 3, 4, 5].first(0).should == []
  end
  
  specify "first with count == 1 should return an array containing the first element" do
    [1, 2, 3, 4, 5].first(1).should == [1]
  end
  
  specify "first should raise ArgumentError when count is negative" do
    should_raise(ArgumentError) { [1, 2].first(-1) }
  end
  
  specify "first should return the entire array when count > length" do
    [1, 2, 3, 4, 5, 9].first(10).should == [1, 2, 3, 4, 5, 9]
  end

  specify "first should call to_int on count" do
    obj = Object.new
    def obj.to_int() 2 end
    [1, 2, 3, 4, 5].first(obj).should == [1, 2]
  end
  
  specify "first with count on array subclasses shouldn't return subclass instance" do
    MyArray[1, 2, 3].first(0).class.should == Array
    MyArray[1, 2, 3].first(1).class.should == Array
    MyArray[1, 2, 3].first(2).class.should == Array
  end
  
  # FIX: as of r1357, #flatten[!] causes Rubinius to allocate memory without bound
  except :rbx do
    specify "flatten should return a one-dimensional flattening recursively" do
      [[[1, [2, 3]],[2, 3, [4, [4, [5, 5]], [1, 2, 3]]], [4]], []].flatten.should == [1, 2, 3, 2, 3, 4, 4, 5, 5, 1, 2, 3, 4]
    end

    specify "flatten shouldn't call flatten on elements" do
      obj = Object.new
      def obj.flatten() [1, 2] end
      [obj, obj].flatten.should == [obj, obj]
  
      obj = [5, 4]
      def obj.flatten() [1, 2] end
      [obj, obj].flatten.should == [5, 4, 5, 4]
    end
  
    specify "flatten should complain about recursive arrays" do
      x = []
      x << x
      should_raise(ArgumentError) { x.flatten }
    
      x = []
      y = []
      x << y
      y << x
      should_raise(ArgumentError) { x.flatten }
    end
  
    specify "flatten on array subclasses should return subclass instance" do
      MyArray[].flatten.class.should == MyArray
      MyArray[1, 2, 3].flatten.class.should == MyArray
      MyArray[1, [2], 3].flatten.class.should == MyArray
      [MyArray[1, 2, 3]].flatten.class.should == Array
    end
  
    specify "flatten! should modify array to produce a one-dimensional flattening recursively" do
      a = [[[1, [2, 3]],[2, 3, [4, [4, [5, 5]], [1, 2, 3]]], [4]], []]
      a.flatten!.equal?(a).should == true
      a.should == [1, 2, 3, 2, 3, 4, 4, 5, 5, 1, 2, 3, 4]
    end
  
    specify "flatten! should return nil if no modifications took place" do
      a = [1, 2, 3]
      a.flatten!.should == nil
    end
  
    specify "flatten! should complain about recursive arrays" do
      x = []
      x << x
      should_raise(ArgumentError) { x.flatten! }
    
      x = []
      y = []
      x << y
      y << x
      should_raise(ArgumentError) { x.flatten! }
    end
  end
  
  specify "frozen? should return true if array is frozen" do
    a = [1, 2, 3]
    a.frozen?.should == false
    a.freeze
    a.frozen?.should == true
  end

  specify "frozen? should return true if array is temporarily frozen while being sort!ed" do
    a = [1, 2, 3]
    a.sort! { |x,y| a.frozen?.should == true; x <=> y }
  end

  specify "frozen? should return false for arrays being sorted (they aren't temporarily frozen)" do
    a = [1, 2, 3]
    a.sort { |x,y| a.frozen?.should == false; x <=> y }
  end
  
  specify "hash returns the same fixnum for arrays with the same content" do
    [].respond_to?(:hash).should == true
    
    [[], [1, 2, 3]].each do |ary|
      ary.hash.should == ary.dup.hash
      ary.hash.class.should == Fixnum
    end
  end
  
  specify "hash calls to_int on result of calling hash on each element" do
    ary = Array.new(5) do
      # Can't use should_receive here because it calls hash()
      obj = Object.new
      def obj.hash()
        def self.to_int() freeze; 0 end
        return self
      end
      obj
    end
    
    ary.hash
    ary.each { |obj| obj.frozen?.should == true }
  end
  
  specify "hash should ignore array class differences" do
    MyArray[].hash.should == [].hash
    MyArray[1, 2].hash.should == [1, 2].hash
  end

  specify "hash provides the same hash code for any two Arrays with the same content" do
    a = [1, 2, 3, 4]
    a.fill 'a', 0..3
    b = %w|a a a a|
    a.hash.should == b.hash
  end
  
  specify "hash returning the same value implies that the arrays are eql?()" do
    a = [1, 2, 3, 4]
    a.fill 'a', 0..3
    b = %w|a a a a|
    a.hash.should == b.hash
    a.eql?(b).should == true
  end

  specify "include? should return true if object is present, false otherwise" do
    [1, 2, "a", "b"].include?("c").should == false
    [1, 2, "a", "b"].include?("a").should == true
  end

  specify "include? calls == on elements from left to right until success" do
    key = "x"
    ary = Array.new(3) { Object.new }
    ary[0].should_receive(:==, :with => [key], :returning => false)
    ary[1].should_receive(:==, :with => [key], :returning => true)
    ary[2].should_not_receive(:==)
    
    ary.include?(key).should == true
  end
  
  specify "index returns the index of the first element == to object" do
    x = Object.new
    def x.==(obj) 3 == obj; end

    [2, x, 3, 1, 3, 1].index(3).should == 1
  end

  specify "index returns 0 if first element == to object" do
    [2, 1, 3, 2, 5].index(2).should == 0
  end

  specify "index returns size-1 if only last element == to object" do
    [2, 1, 3, 1, 5].index(5).should == 4
  end

  specify "index returns nil if no element == to object" do
    [2, 1, 1, 1, 1].index(3).should == nil
  end
  
  # If we test in terms of something then TEST IN TERMS OF IT :) --rue
  specify "indexes and #indices with integer indices are DEPRECATED synonyms for values_at" do
    array = [1, 2, 3, 4, 5]

    x = Object.new
    def x.to_int() 4 end
      
    params = [1, 0, 5, -1, -8, 10, x]
    array.indexes(*params).should == array.values_at(*params)
    array.indices(*params).should == array.values_at(*params)
  end

  specify 'indexes and indices can be given ranges which are returned as nested arrays (DEPRECATED)' do
    warn " (#indexes is deprecated but #values_at does not use the same interface!)"

    array = [1, 2, 3, 4, 5]

    params = [0..2, 1...3, 4..6]
    array.indexes(*params).should == [[1, 2, 3], [2, 3], [5]]
    array.indices(*params).should == [[1, 2, 3], [2, 3], [5]]
  end
  
  specify "initialize_copy should be a synonym for replace" do
    init_ary = [1, 2, 3, 4, 5]
    repl_ary = [1, 2, 3, 4, 5]
    arg = %w(a b c)
    
    init_ary.send(:initialize_copy, arg).should == repl_ary.replace(arg)
    init_ary.should == repl_ary
  end
  
  specify "insert with non-negative index should insert objects before the element at index" do
    ary = []
    ary.insert(0, 3).equal?(ary).should == true
    ary.should == [3]

    ary.insert(0, 1, 2).equal?(ary).should == true
    ary.should == [1, 2, 3]
    ary.insert(0)
    ary.should == [1, 2, 3]
    
    # Let's just assume insert() always modifies the array from now on.
    ary.insert(1, 'a').should == [1, 'a', 2, 3]
    ary.insert(0, 'b').should == ['b', 1, 'a', 2, 3]
    ary.insert(5, 'c').should == ['b', 1, 'a', 2, 3, 'c']
    ary.insert(7, 'd').should == ['b', 1, 'a', 2, 3, 'c', nil, 'd']
    ary.insert(10, 5, 4).should == ['b', 1, 'a', 2, 3, 'c', nil, 'd', nil, nil, 5, 4]
  end

  specify "insert with index -1 should append objects to the end" do
    [1, 3, 3].insert(-1, 2, 'x', 0.5).should == [1, 3, 3, 2, 'x', 0.5]
  end

  specify "insert with negative index should insert objects after the element at index" do
    ary = []
    ary.insert(-1, 3).should == [3]
    ary.insert(-2, 2).should == [2, 3]
    ary.insert(-3, 1).should == [1, 2, 3]
    ary.insert(-2, -3).should == [1, 2, -3, 3]
    ary.insert(-1, []).should == [1, 2, -3, 3, []]
    ary.insert(-2, 'x', 'y').should == [1, 2, -3, 3, 'x', 'y', []]
    ary = [1, 2, 3]
  end

  specify "insert pads with nils if the index to be inserted to is past the end" do
    [].insert(5, 5).should == [nil, nil, nil, nil, nil, 5]
  end

  specify "insert with a negative index allows insertion before the first element" do
    [1, 2, 3].insert(-4, -3).should == [-3, 1, 2, 3]
  end  
  
  specify "insert with negative index beyond array should raise" do
    should_raise(IndexError) { [].insert(-2, 1) }
    should_raise(IndexError) { [1].insert(-3, 2) }
  end

  specify "insert without objects should do nothing" do
    [].insert(0).should == []
    [].insert(-1).should == []
    [].insert(10).should == []
    [].insert(-2).should == []
  end

  specify "insert should call to_int on position argument" do
    obj = Object.new
    def obj.to_int() 2 end
    [].insert(obj, 'x').should == [nil, nil, 'x']
  end
  
  # FIX: compatibility? --rue
  specify "inspect should return a string equivalent to evaluated source code representation" do
    [1, 2, 3].inspect.should == '[1, 2, 3]'
    [1, 2, 3 + 4].inspect.should == '[1, 2, 7]'
    [1, ['a', nil, [], 5.0], [[]], -4].inspect.should == '[1, ["a", nil, [], 5.0], [[]], -4]'
  end

  specify "inspect should call inspect on its arguments" do
    items = Array.new(3) do |i|
      obj = Object.new
      obj.should_receive(:inspect, :returning => "items[#{i}]")
      obj
    end
    
    items.inspect.should == '[items[0], items[1], items[2]]'
  end
  
  # FIX: as of r1357 this causes a VM SIGBUS
  # specify "inspect should handle recursive arrays" do
  #   x = [1, 2]
  #   x << x << 4
  #   x.inspect.should == '[1, 2, [...], 4]'
  # 
  #   x = [1, 2]
  #   y = [3, 4]
  #   x << y
  #   y << x
  #   x.inspect.should == '[1, 2, [3, 4, [...]]]'
  #   y.inspect.should == '[3, 4, [1, 2, [...]]]'
  # end
  
  specify "join should return a string formed by concatenating each element.to_s separated by separator without trailing separator" do
    obj = Object.new
    def obj.to_s() 'foo' end

    [1, 2, 3, 4, obj].join(' | ').should == '1 | 2 | 3 | 4 | foo'
  end

  specify "join's separator defaults to $, (which defaults to empty)" do
    [1, 2, 3].join.should == '123'
    old, $, = $,, '-'
    [1, 2, 3].join.should == '1-2-3'
    $, = old
  end
  
  specify "join should call to_str on its separator argument" do
    obj = Object.new
    def obj.to_str() '::' end    
    [1, 2, 3, 4].join(obj).should == '1::2::3::4'
  end

  specify "join should handle recursive arrays" do
    x = []
    x << x
    x.join(":").should == '[...]'
    
    x = []
    y = []
    y << 9 << x << 8 << y << 7
    x << 1 << x << 2 << y << 3
    # representations when recursing from x
    # these are here to make it easier to understand what is happening
    y_rec = '9:[...]:8:9:[...]:8:[...]:7:7'
    x_rec = '1:[...]:2:' + y_rec + ':3'
    x.join(":").should == '1:' + x_rec + ':2:' + y_rec + ':3'
  end
  
  specify "last returns the last element" do
    [1, 1, 1, 1, 2].last.should == 2
  end
  
  specify "last returns nil if self is empty" do
    [].last.should == nil
  end
  
  specify "last returns the last count elements" do
    [1, 2, 3, 4, 5, 9].last(3).should == [4, 5, 9]
  end
  
  specify "last returns an empty array when count == 0" do
    [1, 2, 3, 4, 5].last(0).should == []
  end
  
  specify "last raises ArgumentError when count is negative" do
    should_raise(ArgumentError) { [1, 2].last(-1) }
  end
  
  specify "last returns the entire array when count > length" do
    [1, 2, 3, 4, 5, 9].last(10).should == [1, 2, 3, 4, 5, 9]
  end

  specify "last with count on array subclasses shouldn't return subclass instance" do
    MyArray[1, 2, 3].last(0).class.should == Array
    MyArray[1, 2, 3].last(1).class.should == Array
    MyArray[1, 2, 3].last(2).class.should == Array
  end
  
  specify "length should return the number of elements" do
    [].length.should == 0
    [1, 2, 3].length.should == 3
  end
  
  specify "map should be a synonym for collect" do
    a = ['a', 'b', 'c', 'd']
    a.map { |i| i + '!'}.should == a.collect { |i| i + '!' }
  end

  specify "map on array subclasses shouldn't return subclass instance" do
    MyArray[1, 2, 3].map { |x| x }.class.should == Array
  end
  
  specify "map! should be a synonym for collect!" do
    a, b = [7, 9, 3, 5], [7, 9, 3, 5]
    a.map! { |i| i - 1 }.should == b.collect! { |i| i - 1 }
    a.should == b
  end
  
  specify "nitems should return the number of non-nil elements" do
    [nil].nitems.should == 0
    [].nitems.should == 0    
    [1, 2, 3, nil].nitems.should == 3
    [1, 2, 3].nitems.should == 3
    [1, nil, 2, 3, nil, nil, 4].nitems.should == 4
    [1, nil, 2, false, 3, nil, nil, 4].nitems.should == 5
  end
  
  specify "partition should return two arrays" do
    [].partition {}.should == [[], []]
  end
  
  specify "partition should return in the left array values for which the block evaluates to true" do
    ary = [0, 1, 2, 3, 4, 5]

    ary.partition { |i| true }.should == [ary, []]
    ary.partition { |i| 5 }.should == [ary, []]
    ary.partition { |i| false }.should == [[], ary]
    ary.partition { |i| nil }.should == [[], ary]
    ary.partition { |i| i % 2 == 0 }.should == [[0, 2, 4], [1, 3, 5]]
    ary.partition { |i| i / 3 == 0 }.should == [[0, 1, 2], [3, 4, 5]]
  end
  
  specify "partition on array subclasses shouldn't return subclass instances" do
    result = MyArray[1, 2, 3].partition { |x| x % 2 == 0 }
    result.class.should == Array
    result[0].class.should == Array
    result[1].class.should == Array
  end
  
  specify "pop should remove and return the last element of the array" do
    a = ["a", 1, nil, true]
    
    a.pop.should == true
    a.should == ["a", 1, nil]

    a.pop.should == nil
    a.should == ["a", 1]

    a.pop.should == 1
    a.should == ["a"]

    a.pop.should == "a"
    a.should == []
  end
  
  specify "pop should return nil if there are no more elements" do
    [].pop.should == nil
  end
  
  specify "push should append the arguments to the array" do
    a = [ "a", "b", "c" ]
    a.push("d", "e", "f").equal?(a).should == true
    a.push().should == ["a", "b", "c", "d", "e", "f"]
    a.push(5)
    a.should == ["a", "b", "c", "d", "e", "f", 5]
  end
  
  specify "rassoc should return the first contained array whose second element is == object" do
    ary = [[1, "a", 0.5], [2, "b"], [3, "b"], [4, "c"], [], [5], [6, "d"]]
    ary.rassoc("a").should == [1, "a", 0.5]
    ary.rassoc("b").should == [2, "b"]
    ary.rassoc("d").should == [6, "d"]
    ary.rassoc("z").should == nil
  end
  
  specify "rassoc should call == on argument" do
    key = Object.new
    items = Array.new(3) { ["foo", Object.new, "bar"] }
    items[0][1].should_receive(:==, :with => [key], :returning => false)
    items[1][1].should_receive(:==, :with => [key], :returning => true)
    items[2][1].should_not_receive(:==, :with => [key])
    items.rassoc(key).should == items[1]
  end
  
  specify "reject should return a new array without elements for which block is true" do
    ary = [1, 2, 3, 4, 5]
    ary.reject { true }.should == []
    ary.reject { false }.should == ary
    ary.reject { nil }.should == ary
    ary.reject { 5 }.should == []
    ary.reject { |i| i < 3 }.should == [3, 4, 5]
    ary.reject { |i| i % 2 == 0 }.should == [1, 3, 5]
  end
  
  # Returns MyArray on MRI 1.8 which is inconsistent with select.
  # It has been changed on 1.9 however.
  specify "reject on array subclasses shouldn't return subclass instance" do
    MyArray[1, 2, 3].reject { |x| x % 2 == 0 }.class.should == Array
  end
  
  specify "reject! should remove elements for which block is true" do
    a = [3, 4, 5, 6, 7, 8, 9, 10, 11]
    a.reject! { |i| i % 2 == 0 }.equal?(a).should == true
    a.should == [3, 5, 7, 9, 11]
    a.reject! { |i| i > 8 }
    a.should == [3, 5, 7]
    a.reject! { |i| i < 4 }
    a.should == [5, 7]
    a.reject! { |i| i == 5 }
    a.should == [7]
    a.reject! { true }
    a.should == []
    a.reject! { true }
    a.should == []
  end
  
  specify "reject! should return nil if no changes are made" do
    a = [1, 2, 3]
    
    a.reject! { |i| i < 0 }.should == nil
    
    a.reject! { true }
    a.reject! { true }.should == nil
  end
  
  specify "replace should replace the elements with elements from other array" do
    a = [1, 2, 3, 4, 5]
    b = ['a', 'b', 'c']
    a.replace(b).equal?(a).should == true
    a.should == b
    a.equal?(b).should == false

    a.replace([4] * 10)
    a.should == [4] * 10
    
    a.replace([])
    a.should == []
  end
  
  specify "replace should call to_ary on its argument" do
    obj = Object.new
    def obj.to_ary() [1, 2, 3] end
      
    ary = []
    ary.replace(obj)
    ary.should == [1, 2, 3]
  end
  
  specify "replace shouldn't call to_ary on array subclasses" do
    ary = []
    ary.replace(ToAryArray[5, 6, 7])
    ary.should == [5, 6, 7]
  end
  
  specify "reverse should return a new array with the elements in reverse order" do
    [].reverse.should == []
    [1, 3, 5, 2].reverse.should == [2, 5, 3, 1]
  end

  specify "reverse on array subclasses should return subclass instance" do
    MyArray[1, 2, 3].reverse.class.should == MyArray
  end  
  
  specify "reverse! should reverse the elements in place" do
    a = [6, 3, 4, 2, 1]
    a.reverse!.equal?(a).should == true
    a.should == [1, 2, 4, 3, 6]
    [].reverse!.should == []
  end
  
  specify "reverse_each should traverse array in reverse order and pass each element to block" do
    a = []
    [1, 3, 4, 6].reverse_each { |i| a << i }
    a.should == [6, 4, 3, 1]
  end
  
  specify "rindex returns the first index backwards from the end where element == to object" do
    key = 3    
    ary = Array.new(3) { Object.new }
    ary[2].should_receive(:==, :with => [key], :returning => false)
    ary[1].should_receive(:==, :with => [key], :returning => true)
    ary[0].should_not_receive(:==)

    ary.rindex(key).should == 1
  end

  specify "rindex returns size-1 if last element == to object" do
    [2, 1, 3, 2, 5].rindex(5).should == 4
  end

  specify "rindex returns 0 if only first element == to object" do
    [2, 1, 3, 1, 5].rindex(2).should == 0
  end

  specify "rindex returns nil if no element == to object" do
    [1, 1, 3, 2, 1, 3].rindex(4).should == nil
  end
  
  specify "select should return a new array of elements for which block is true" do
    [1, 3, 4, 5, 6, 9].select { |i| i % ((i + 1) / 2) == 0}.should == [1, 4, 6]
  end

  specify "select on array subclasses shouldn't return subclass instance" do
    MyArray[1, 2, 3].select { true }.class.should == Array
  end
  
  specify "shift should remove and return the first element" do
    a = [5, 1, 1, 5, 4]
    a.shift.should == 5
    a.should == [1, 1, 5, 4]
    a.shift.should == 1
    a.should == [1, 5, 4]
    a.shift.should == 1
    a.should == [5, 4]
    a.shift.should == 5
    a.should == [4]
    a.shift.should == 4
    a.should == []
  end
  
  specify "shift should return nil when the array is empty" do
    [].shift.should == nil
  end
  
  specify "size should be a synonym for length" do
    [1, 2, 3].size.should == [1, 2, 3].length
  end
  
  specify "slice! with index should remove and return the element at index" do
    a = [1, 2, 3, 4]
    a.slice!(10).should == nil
    a.should == [1, 2, 3, 4]
    a.slice!(-10).should == nil
    a.should == [1, 2, 3, 4]
    a.slice!(2).should == 3
    a.should == [1, 2, 4]
    a.slice!(-1).should == 4
    a.should == [1, 2]
    a.slice!(1).should == 2
    a.should == [1]
    a.slice!(-1).should == 1
    a.should == []
    a.slice!(-1).should == nil
    a.should == []
    a.slice!(0).should == nil
    a.should == []
  end
  
  specify "slice! with start, length should remove and return length elements beginning at start" do
    a = [1, 2, 3, 4, 5, 6]
    a.slice!(2, 3).should == [3, 4, 5]
    a.should == [1, 2, 6]
    a.slice!(1, 1).should == [2]
    a.should == [1, 6]
    a.slice!(1, 0).should == []
    a.should == [1, 6]
    a.slice!(2, 0).should == []
    a.should == [1, 6]
    a.slice!(0, 4).should == [1, 6]
    a.should == []
    a.slice!(0, 4).should == []
    a.should == []
  end

  specify "slice! should call to_int on start and length arguments" do
    obj = Object.new
    def obj.to_int() 2 end
      
    a = [1, 2, 3, 4, 5]
    a.slice!(obj).should == 3
    a.should == [1, 2, 4, 5]
    a.slice!(obj, obj).should == [4, 5]
    a.should == [1, 2]
    a.slice!(0, obj).should == [1, 2]
    a.should == []
  end

  specify "slice! with range should remove and return elements in range" do
    a = [1, 2, 3, 4, 5, 6, 7, 8]
    a.slice!(1..4).should == [2, 3, 4, 5]
    a.should == [1, 6, 7, 8]
    a.slice!(1...3).should == [6, 7]
    a.should == [1, 8]
    a.slice!(-1..-1).should == [8]
    a.should == [1]
    a.slice!(0...0).should == []
    a.should == [1]
    a.slice!(0..0).should == [1]
    a.should == []
  end
  
  specify "slice! with range should call to_int on range arguments" do
    from = Object.new
    to = Object.new
    
    # So we can construct a range out of them...
    def from.<=>(o) 0 end
    def to.<=>(o) 0 end

    def from.to_int() 1 end
    def to.to_int() -2 end
      
    a = [1, 2, 3, 4, 5]
      
    a.slice!(from .. to).should == [2, 3, 4]
    a.should == [1, 5]

    a.slice!(1..0).should == []
    a.should == [1, 5]
  
    should_raise(TypeError) { a.slice!("a" .. "b") }
    should_raise(TypeError) { a.slice!(from .. "b") }
  end
  
  # TODO: MRI behaves inconsistently here. I'm trying to find out what it should
  # do at ruby-core right now. -- flgr
  # See http://groups.google.com/group/ruby-core-google/t/af70e3d0e9b82f39
  specify "slice! with indices outside of array should (not?) expand array" do
    # This is the way MRI behaves -- subject to change
    a = [1, 2]
    a.slice!(4).should == nil
    a.should == [1, 2]
    a.slice!(4, 0).should == nil
    a.should == [1, 2, nil, nil]
    a.slice!(6, 1).should == nil
    a.should == [1, 2, nil, nil, nil, nil]
    a.slice!(8...8).should == nil
    a.should == [1, 2, nil, nil, nil, nil, nil, nil]
    a.slice!(10..10).should == nil
    a.should == [1, 2, nil, nil, nil, nil, nil, nil, nil, nil]
  end
  
  class D 
    def <=>(obj) 
      return 4 <=> obj unless obj.class == D
      0
    end
  end

  specify "sort should return a new array from sorting elements using <=> on the pivot" do
    d = D.new

    [1, 1, 5, -5, 2, -10, 14, 6].sort.should == [-10, -5, 1, 1, 2, 5, 6, 14]
    [d, 1].sort.should == [1, d]
  end

  specify "sort raises an ArgumentError if the comparison cannot be completed" do
    d = D.new

    # Fails essentially because of d.<=>(e) whereas d.<=>(1) would work
    should_raise(ArgumentError) { [1, d].sort.should == [1, d] }
  end
  
  specify "sort may take a block which is used to determine the order of objects a and b described as -1, 0 or +1" do
    a = [5, 1, 4, 3, 2]
    a.sort.should == [1, 2, 3, 4, 5]
    a.sort {|x, y| y <=> x}.should == [5, 4, 3, 2, 1]
  end
  
  specify "sort on array subclasses should return subclass instance" do
    ary = MyArray[1, 2, 3]
    ary.sort.class.should == MyArray
  end
  
  specify "sort! should sort array in place using <=>" do
    a = [1, 9, 7, 11, -1, -4]
    a.sort!
    a.should == [-4, -1, 1, 7, 9, 11]
  end
  
  specify "sort! should sort array in place using block value" do
    a = [1, 3, 2, 5, 4]
    a.sort! { |x, y| y <=> x }
    a.should == [5, 4, 3, 2, 1]
  end
  
  specify "to_a returns self" do
    a = [1, 2, 3]
    a.to_a.should == [1, 2, 3]
    a.equal?(a.to_a).should == true 
  end
  
  specify "to_a on array subclasses shouldn't return subclass instance" do
    e = MyArray.new
    e << 1
    e.to_a.class.should == Array
    e.to_a.should == [1]
  end
  
  specify "to_ary returns self" do
    a = [1, 2, 3]
    a.equal?(a.to_ary).should == true
    a = MyArray[1, 2, 3]
    a.equal?(a.to_ary).should == true
  end
  
  specify "to_s is equivalent to #joining without a separator string" do
    a = [1, 2, 3, 4]
    a.to_s.should == a.join
    $, = '-'
    a.to_s.should == a.join
    $, = ''
  end

  specify "transpose assumes an array of arrays and should return the result of transposing rows and columns" do
    [[1, 'a'], [2, 'b'], [3, 'c']].transpose.should == [[1, 2, 3], ["a", "b", "c"]]
  end

  specify "transpose raises if the elements of the array are not Arrays or respond to to_ary" do
    g = Object.new
    def g.to_a() [1, 2] end
    h = Object.new
    def h.to_ary() [1, 2] end
  end
  
  specify "slice! with index should remove and return the element at index" do
    a = [1, 2, 3, 4]
    a.slice!(10).should == nil
    a.should == [1, 2, 3, 4]
    a.slice!(-10).should == nil
    a.should == [1, 2, 3, 4]
    a.slice!(2).should == 3
    a.should == [1, 2, 4]
    a.slice!(-1).should == 4
    a.should == [1, 2]
    a.slice!(1).should == 2
    a.should == [1]
    a.slice!(-1).should == 1
    a.should == []
    a.slice!(-1).should == nil
    a.should == []
    a.slice!(0).should == nil
    a.should == []
  end
  
  specify "slice! with start, length should remove and return length elements beginning at start" do
    a = [1, 2, 3, 4, 5, 6]
    a.slice!(2, 3).should == [3, 4, 5]
    a.should == [1, 2, 6]
    a.slice!(1, 1).should == [2]
    a.should == [1, 6]
    a.slice!(1, 0).should == []
    a.should == [1, 6]
    a.slice!(2, 0).should == []
    a.should == [1, 6]
    a.slice!(0, 4).should == [1, 6]
    a.should == []
    a.slice!(0, 4).should == []
    a.should == []
  end

  specify "slice! should call to_int on start and length arguments" do
    obj = Object.new
    def obj.to_int() 2 end
      
    a = [1, 2, 3, 4, 5]
    a.slice!(obj).should == 3
    a.should == [1, 2, 4, 5]
    a.slice!(obj, obj).should == [4, 5]
    a.should == [1, 2]
    a.slice!(0, obj).should == [1, 2]
    a.should == []
  end

  specify "slice! with range should remove and return elements in range" do
    a = [1, 2, 3, 4, 5, 6, 7, 8]
    a.slice!(1..4).should == [2, 3, 4, 5]
    a.should == [1, 6, 7, 8]
    a.slice!(1...3).should == [6, 7]
    a.should == [1, 8]
    a.slice!(-1..-1).should == [8]
    a.should == [1]
    a.slice!(0...0).should == []
    a.should == [1]
    a.slice!(0..0).should == [1]
    a.should == []
  end
  
  specify "slice! with range should call to_int on range arguments" do
    from = Object.new
    to = Object.new
    
    # So we can construct a range out of them...
    def from.<=>(o) 0 end
    def to.<=>(o) 0 end

    def from.to_int() 1 end
    def to.to_int() -2 end
      
    a = [1, 2, 3, 4, 5]
      
    a.slice!(from .. to).should == [2, 3, 4]
    a.should == [1, 5]

    a.slice!(1..0).should == []
    a.should == [1, 5]
  
    should_raise(TypeError) { a.slice!("a" .. "b") }
    should_raise(TypeError) { a.slice!(from .. "b") }
  end
  
  # TODO: MRI behaves inconsistently here. I'm trying to find out what it should
  # do at ruby-core right now. -- flgr
  # See http://groups.google.com/group/ruby-core-google/t/af70e3d0e9b82f39
  specify "slice! with indices outside of array should (not?) expand array" do
    # This is the way MRI behaves -- subject to change
    a = [1, 2]
    a.slice!(4).should == nil
    a.should == [1, 2]
    a.slice!(4, 0).should == nil
    a.should == [1, 2, nil, nil]
    a.slice!(6, 1).should == nil
    a.should == [1, 2, nil, nil, nil, nil]
    a.slice!(8...8).should == nil
    a.should == [1, 2, nil, nil, nil, nil, nil, nil]
    a.slice!(10..10).should == nil
    a.should == [1, 2, nil, nil, nil, nil, nil, nil, nil, nil]
  end
  
  class D 
    def <=>(obj) 
      return 4 <=> obj unless obj.class == D
      0
    end
  end

  specify "sort should return a new array from sorting elements using <=> on the pivot" do
    d = D.new

    [1, 1, 5, -5, 2, -10, 14, 6].sort.should == [-10, -5, 1, 1, 2, 5, 6, 14]
    [d, 1].sort.should == [1, d]
  end

  specify "sort raises an ArgumentError if the comparison cannot be completed" do
    d = D.new

    # Fails essentially because of d.<=>(e) whereas d.<=>(1) would work
    should_raise(ArgumentError) { [1, d].sort.should == [1, d] }
  end
  
  specify "sort may take a block which is used to determine the order of objects a and b described as -1, 0 or +1" do
    a = [5, 1, 4, 3, 2]
    a.sort.should == [1, 2, 3, 4, 5]
    a.sort {|x, y| y <=> x}.should == [5, 4, 3, 2, 1]
  end
  
  specify "sort on array subclasses should return subclass instance" do
    ary = MyArray[1, 2, 3]
    ary.sort.class.should == MyArray
  end
  
  specify "sort! should sort array in place using <=>" do
    a = [1, 9, 7, 11, -1, -4]
    a.sort!
    a.should == [-4, -1, 1, 7, 9, 11]
  end
  
  specify "sort! should sort array in place using block value" do
    a = [1, 3, 2, 5, 4]
    a.sort! { |x, y| y <=> x }
    a.should == [5, 4, 3, 2, 1]
  end
  
  specify "to_a returns self" do
    a = [1, 2, 3]
    a.to_a.should == [1, 2, 3]
    a.equal?(a.to_a).should == true 
  end
  
  specify "to_a on array subclasses shouldn't return subclass instance" do
    e = MyArray.new
    e << 1
    e.to_a.class.should == Array
    e.to_a.should == [1]
  end
  
  specify "to_ary returns self" do
    a = [1, 2, 3]
    a.equal?(a.to_ary).should == true
    a = MyArray[1, 2, 3]
    a.equal?(a.to_ary).should == true
  end
  
  specify "to_s is equivalent to #joining without a separator string" do
    a = [1, 2, 3, 4]
    a.to_s.should == a.join
    $, = '-'
    a.to_s.should == a.join
    $, = ''

    x = []
    x << x
    x.to_s.should == x.join

    x = []
    y = []
    y << 9 << x << 8 << y << 7
    x << 1 << x << 2 << y << 3
    x.to_s.should == x.join    
  end

  specify "transpose assumes an array of arrays and should return the result of transposing rows and columns" do
    [[1, 'a'], [2, 'b'], [3, 'c']].transpose.should == [[1, 2, 3], ["a", "b", "c"]]
    [[1, 2, 3], ["a", "b", "c"]].transpose.should == [[1, 'a'], [2, 'b'], [3, 'c']]
    [].transpose.should == []
    [[]].transpose.should == []
    [[], []].transpose.should == []
    [[0]].transpose.should == [[0]]
    [[0], [1]].transpose.should == [[0, 1]]
  end

  specify "transpose raises if the items aren't arrays and don't respond to to_ary" do
    g = Object.new
    def g.to_a() [1, 2] end
    h = Object.new
    def h.to_ary() [1, 2] end

    should_raise(TypeError) { [g, [:a, :b]].transpose } 
    [h, [:a, :b]].transpose.should == [[1, :a], [2, :b]]
  end
  
  specify "transpose shouldn't call to_ary on array subclass elements" do
    ary = [ToAryArray[1, 2], ToAryArray[4, 6]]
    ary.transpose.should == [[1, 4], [2, 6]]
  end

  specify "transpose should raise if the arrays are not of the same length" do
    should_raise(IndexError) { [[1, 2], [:a]].transpose }
  end
  
  specify "transpose on array subclasses shouldn't return subclass instance" do
    result = MyArray[MyArray[1, 2, 3], MyArray[4, 5, 6]].transpose
    result.class.should == Array
    result[0].class.should == Array
    result[1].class.should == Array
  end
  
  specify "uniq should return an array with no duplicates" do
    ["a", "a", "b", "b", "c"].uniq.should == ["a", "b", "c"]
  end

  # But MRI doesn't actually call eql?()
  specify "uniq compares elements with eql? semantics for numbers" do
    [1.0, 1].uniq.should == [1.0, 1]
  end
  
  specify "uniq on array subclasses should return subclass instance" do
    MyArray[1, 2, 3].uniq.class.should == MyArray
  end
  
  specify "uniq! modifies the array in place" do
    a = [ "a", "a", "b", "b", "c" ]
    a.uniq!
    a.should == ["a", "b", "c"]
  end
  
  specify "uniq! should return self" do
    a = [ "a", "a", "b", "b", "c" ]
    a.equal?(a.uniq!).should == true
  end
  
  specify "uniq! should return nil if no changes are made to the array" do
    [ "a", "b", "c" ].uniq!.should == nil
  end
  
  specify "unshift should prepend object to the original array" do
    a = [1, 2, 3]
    a.unshift("a").equal?(a).should == true
    a.should == ['a', 1, 2, 3]
    a.unshift().equal?(a).should == true
    a.should == ['a', 1, 2, 3]
    a.unshift(5, 4, 3)
    a.should == [5, 4, 3, 'a', 1, 2, 3]
  end
  
  specify "values_at with indices should return an array of elements at the indexes" do
    [1, 2, 3, 4, 5].values_at().should == []
    [1, 2, 3, 4, 5].values_at(1, 0, 5, -1, -8, 10).should == [2, 1, nil, 5, nil, nil]
  end

  specify "values_at should call to_int on its indices" do
    obj = Object.new
    def obj.to_int() 1 end
    [1, 2].values_at(obj, obj, obj).should == [2, 2, 2]
  end
  
  specify "values_at with ranges should return an array of elements in the ranges" do
    [1, 2, 3, 4, 5].values_at(0..2, 1...3, 4..6).should == [1, 2, 3, 2, 3, 5, nil]
    [1, 2, 3, 4, 5].values_at(6..4).should == []
  end

  specify "values_at with ranges should call to_int on arguments of ranges" do
    from = Object.new
    to = Object.new

    # So we can construct a range out of them...
    def from.<=>(o) 0 end
    def to.<=>(o) 0 end

    def from.to_int() 1 end
    def to.to_int() -2 end
      
    ary = [1, 2, 3, 4, 5]
    ary.values_at(from .. to, from ... to, to .. from).should == [2, 3, 4, 2, 3]
  end
  
  specify "values_at on array subclasses shouldn't return subclass instance" do
    MyArray[1, 2, 3].values_at(0, 1..2, 1).class.should == Array
  end
  
  specify "zip should return an array of arrays containing corresponding elements of each array" do
    [1, 2, 3, 4].zip(["a", "b", "c", "d", "e"]).should ==
      [[1, "a"], [2, "b"], [3, "c"], [4, "d"]]
  end
  
  specify 'zip fills in missing values with nil' do
    [1, 2, 3, 4, 5].zip(["a", "b", "c", "d"]).should ==
      [[1, "a"], [2, "b"], [3, "c"], [4, "d"], [5, nil]]
  end
  
  # MRI 1.8.4 uses to_ary, but it's been fixed in 1.9, perhaps also in 1.8.5
  specify "zip should call to_a on its arguments" do
    [1, 2, 3].zip("f" .. "z", 1 .. 9).should ==
      [[1, "f", 1], [2, "g", 2], [3, "h", 3]]
  end  

  specify "zip should call block if supplied" do
    values = []
    [1, 2, 3, 4].zip(["a", "b", "c", "d", "e"]) { |value|
      values << value
    }.should == nil
    
    values.should == [[1, "a"], [2, "b"], [3, "c"], [4, "d"]]
  end
  
  specify "zip on array subclasses shouldn't return subclass instance" do
    MyArray[1, 2, 3].zip(["a", "b"]).class.should == Array
  end
end

describe 'Array access using #[] and #slice' do
  # These two must be synonymous
  %w|[] slice|.each do |cmd|

    specify "provide the element at the specified index with [x] (##{cmd})" do
      [ "a", "b", "c", "d", "e" ].send(cmd, 1).should == "b"

      a = [1, 2, 3, 4]
      
      a.send(cmd, 0).should == 1
      a.send(cmd, 1).should == 2
      a.send(cmd, 2).should == 3
      a.send(cmd, 3).should == 4
      a.send(cmd, 4).should == nil
      a.send(cmd, 10).should == nil

      a.should == [1, 2, 3, 4]
    end
    
    specify "count backwards for negative indices for [x] (##{cmd})" do
      [ "a", "b", "c", "d", "e" ].send(cmd, -2).should == "d"

      a = [1, 2, 3, 4]

      a.send(cmd, -1).should == 4
      a.send(cmd, -2).should == 3
      a.send(cmd, -3).should == 2
      a.send(cmd, -4).should == 1
      a.send(cmd, -5).should == nil
      a.send(cmd, -10).should == nil

      a.should == [1, 2, 3, 4]
    end
    
    specify "[index, length] returns subarray with index from start containing length elements (##{cmd})" do
      [ "a", "b", "c", "d", "e" ].send(cmd, 2, 3).should == ["c", "d", "e"]

      a = [1, 2, 3, 4]

      a.send(cmd, 0, 0).should == []
      a.send(cmd, 0, 1).should == [1]
      a.send(cmd, 0, 2).should == [1, 2]
      a.send(cmd, 0, 4).should == [1, 2, 3, 4]
      a.send(cmd, 0, 6).should == [1, 2, 3, 4]
      a.send(cmd, 0, -1).should == nil
      a.send(cmd, 0, -2).should == nil
      a.send(cmd, 0, -4).should == nil

      a.send(cmd, 2, 0).should == []
      a.send(cmd, 2, 1).should == [3]
      a.send(cmd, 2, 2).should == [3, 4]
      a.send(cmd, 2, 4).should == [3, 4]
      a.send(cmd, 2, -1).should == nil

      a.send(cmd, 4, 0).should == []
      a.send(cmd, 4, 2).should == []
      a.send(cmd, 4, -1).should == nil

      a.send(cmd, 5, 0).should == nil
      a.send(cmd, 5, 2).should == nil
      a.send(cmd, 5, -1).should == nil

      a.send(cmd, 6, 0).should == nil
      a.send(cmd, 6, 2).should == nil
      a.send(cmd, 6, -1).should == nil

      a.should == [1, 2, 3, 4]
    end
    
    specify "[index, -length] returns subarray with index from end of length elements (##{cmd})" do
      [ "a", "b", "c", "d", "e" ].send(cmd, -2, 2).should == ["d", "e"]
      
      a = [1, 2, 3, 4]

      a.send(cmd, -1, 0).should == []
      a.send(cmd, -1, 1).should == [4]
      a.send(cmd, -1, 2).should == [4]
      a.send(cmd, -1, -1).should == nil

      a.send(cmd, -2, 0).should == []
      a.send(cmd, -2, 1).should == [3]
      a.send(cmd, -2, 2).should == [3, 4]
      a.send(cmd, -2, 4).should == [3, 4]
      a.send(cmd, -2, -1).should == nil

      a.send(cmd, -4, 0).should == []
      a.send(cmd, -4, 1).should == [1]
      a.send(cmd, -4, 2).should == [1, 2]
      a.send(cmd, -4, 4).should == [1, 2, 3, 4]
      a.send(cmd, -4, 6).should == [1, 2, 3, 4]
      a.send(cmd, -4, -1).should == nil

      a.send(cmd, -5, 0).should == nil
      a.send(cmd, -5, 1).should == nil
      a.send(cmd, -5, 10).should == nil
      a.send(cmd, -5, -1).should == nil

      a.should == [1, 2, 3, 4]
    end
    
    specify "[0, length] should provide a subarray from start containing length elements (##{cmd})" do
      [ "a", "b", "c", "d", "e" ].send(cmd, 0, 3).should == ["a", "b", "c"]
    end

    specify "[index, length] should call to_int on index and length arguments (##{cmd})" do
      obj = Object.new
      def obj.to_int() 2 end
        
      a = [1, 2, 3, 4]
      a.send(cmd, obj).should == 3
      a.send(cmd, obj, 1).should == [3]
      a.send(cmd, obj, obj).should == [3, 4]
      a.send(cmd, 0, obj).should == [1, 2]
    end
    
    specify "[m..n] should provide a subarray specified by range (##{cmd})" do
      [ "a", "b", "c", "d", "e" ].send(cmd, 1..3).should == ["b", "c", "d"]
      [ "a", "b", "c", "d", "e" ].send(cmd, 4..-1).should == ['e']
      [ "a", "b", "c", "d", "e" ].send(cmd, 3..3).should == ['d']
      [ "a", "b", "c", "d", "e" ].send(cmd, 3..-2).should == ['d']
      ['a'].send(cmd, 0..-1).should == ['a']

      a = [1, 2, 3, 4]
      
      a.send(cmd, 0..-10).should == []
      a.send(cmd, 0..0).should == [1]
      a.send(cmd, 0..1).should == [1, 2]
      a.send(cmd, 0..2).should == [1, 2, 3]
      a.send(cmd, 0..3).should == [1, 2, 3, 4]
      a.send(cmd, 0..4).should == [1, 2, 3, 4]
      a.send(cmd, 0..10).should == [1, 2, 3, 4]
      
      a.send(cmd, 2..-10).should == []
      a.send(cmd, 2..0).should == []
      a.send(cmd, 2..2).should == [3]
      a.send(cmd, 2..3).should == [3, 4]
      a.send(cmd, 2..4).should == [3, 4]

      a.send(cmd, 3..0).should == []
      a.send(cmd, 3..3).should == [4]
      a.send(cmd, 3..4).should == [4]
      
      a.send(cmd, 4..0).should == []
      a.send(cmd, 4..4).should == []
      a.send(cmd, 4..5).should == []

      a.send(cmd, 5..0).should == nil
      a.send(cmd, 5..5).should == nil
      a.send(cmd, 5..6).should == nil

      a.should == [1, 2, 3, 4]
    end
    
    specify "[m...n] should provide a subarray specified by range sans last as per normal Ranges (##{cmd})" do
      [ "a", "b", "c", "d", "e" ].send(cmd, 1...3).should == ["b", "c"]

      a = [1, 2, 3, 4]
      
      a.send(cmd, 0...-10).should == []
      a.send(cmd, 0...0).should == []
      a.send(cmd, 0...1).should == [1]
      a.send(cmd, 0...2).should == [1, 2]
      a.send(cmd, 0...3).should == [1, 2, 3]
      a.send(cmd, 0...4).should == [1, 2, 3, 4]
      a.send(cmd, 0...10).should == [1, 2, 3, 4]
      
      a.send(cmd, 2...-10).should == []
      a.send(cmd, 2...0).should == []
      a.send(cmd, 2...2).should == []
      a.send(cmd, 2...3).should == [3]
      a.send(cmd, 2...4).should == [3, 4]
      
      a.send(cmd, 3...0).should == []
      a.send(cmd, 3...3).should == []
      a.send(cmd, 3...4).should == [4]
      
      a.send(cmd, 4...0).should == []
      a.send(cmd, 4...4).should == []
      a.send(cmd, 4...5).should == []
      
      a.send(cmd, 5...0).should == nil
      a.send(cmd, 5...5).should == nil
      a.send(cmd, 5...6).should == nil

      a.should == [1, 2, 3, 4]
    end
    
    specify "[m..n] should return existing requested items if range start is in the array but range end is not (##{cmd})" do
      [ "a", "b", "c", "d", "e" ].send(cmd, 4..7).should == ["e"]
    end
    
    specify "[m..n] and [m...n] work with a negative m and both signs for n" do 
      a = [1, 2, 3, 4]

      a.send(cmd, -1..-1).should == [4]
      a.send(cmd, -1...-1).should == []
      a.send(cmd, -1..3).should == [4]
      a.send(cmd, -1...3).should == []
      a.send(cmd, -1..4).should == [4]
      a.send(cmd, -1...4).should == [4]
      a.send(cmd, -1..10).should == [4]
      a.send(cmd, -1...10).should == [4]
      a.send(cmd, -1..0).should == []
      a.send(cmd, -1..-4).should == []
      a.send(cmd, -1...-4).should == []
      a.send(cmd, -1..-6).should == []
      a.send(cmd, -1...-6).should == []

      a.send(cmd, -2..-2).should == [3]
      a.send(cmd, -2...-2).should == []
      a.send(cmd, -2..-1).should == [3, 4]
      a.send(cmd, -2...-1).should == [3]
      a.send(cmd, -2..10).should == [3, 4]
      a.send(cmd, -2...10).should == [3, 4]

      a.send(cmd, -4..-4).should == [1]
      a.send(cmd, -4..-2).should == [1, 2, 3]
      a.send(cmd, -4...-2).should == [1, 2]
      a.send(cmd, -4..-1).should == [1, 2, 3, 4]
      a.send(cmd, -4...-1).should == [1, 2, 3]
      a.send(cmd, -4..3).should == [1, 2, 3, 4]
      a.send(cmd, -4...3).should == [1, 2, 3]
      a.send(cmd, -4..4).should == [1, 2, 3, 4]
      a.send(cmd, -4...4).should == [1, 2, 3, 4]
      a.send(cmd, -4...4).should == [1, 2, 3, 4]
      a.send(cmd, -4..0).should == [1]
      a.send(cmd, -4...0).should == []
      a.send(cmd, -4..1).should == [1, 2]
      a.send(cmd, -4...1).should == [1]

      a.send(cmd, -5..-5).should == nil
      a.send(cmd, -5...-5).should == nil
      a.send(cmd, -5..-4).should == nil
      a.send(cmd, -5..-1).should == nil
      a.send(cmd, -5..10).should == nil
      
      a.should == [1, 2, 3, 4]
    end
  
    specify "[m..n] and [m...n] with range should call to_int on range arguments (##{cmd})" do
      from = Object.new
      to = Object.new

      # So we can construct a range out of them...
      def from.<=>(o) 0 end
      def to.<=>(o) 0 end

      def from.to_int() 1 end
      def to.to_int() -2 end
        
      a = [1, 2, 3, 4]
        
      a.send(cmd, from..to).should == [2, 3]
      a.send(cmd, from...to).should == [2]
      a.send(cmd, 1..0).should == []
      a.send(cmd, 1...0).should == []
      
      should_raise(TypeError) { a.slice("a" .. "b") }
      should_raise(TypeError) { a.slice("a" ... "b") }
      should_raise(TypeError) { a.slice(from .. "b") }
      should_raise(TypeError) { a.slice(from ... "b") }
    end

    specify "[index] returns nil for a requested index not in the array (##{cmd})" do
      [ "a", "b", "c", "d", "e" ].send(cmd, 5).should == nil
    end
    
    specify "[index, length] returns [] if the index is valid but count is zero (##{cmd})" do
      [ "a", "b", "c", "d", "e" ].send(cmd, 0, 0).should == []
      [ "a", "b", "c", "d", "e" ].send(cmd, 2, 0).should == []
    end

    specify "[index, length] returns nil if count is zero but index is invalid (##{cmd})" do
      [ "a", "b", "c", "d", "e" ].send(cmd, 100, 0).should == nil
      [ "a", "b", "c", "d", "e" ].send(cmd, -50, 0).should == nil
    end

    # This is by design. It is in the official documentation.
    specify "[x, y] returns [] if index == array.size (##{cmd})" do
      %w|a b c d e|.send(cmd, 5, 2).should == []
    end

    specify "[x, y] returns nil if index > array.size (##{cmd})" do
      %w|a b c d e|.send(cmd, 6, 2).should == nil
    end

    specify "[x, y] returns nil if count is negative (##{cmd})" do
      %w|a b c d e|.send(cmd, 3, -1).should == nil
      %w|a b c d e|.send(cmd, 2, -2).should == nil
      %w|a b c d e|.send(cmd, 1, -100).should == nil
    end
    
    specify "[x..y] returns nil if no requested index is in the array (##{cmd})" do
      [ "a", "b", "c", "d", "e" ].send(cmd, 6..10).should == nil
    end
    
    specify "[m..n] returns nil if range start is not in the array (##{cmd})" do
      [ "a", "b", "c", "d", "e" ].send(cmd, -10..2).should == nil
      [ "a", "b", "c", "d", "e" ].send(cmd, 10..12).should == nil
    end
    
    specify "[m...n] should return an empty array when m == n (##{cmd})" do
      [1, 2, 3, 4, 5].send(cmd, 1...1).should == []
    end
    
    specify "[0...0] should return an empty array (##{cmd})" do
      [1, 2, 3, 4, 5].send(cmd, 0...0).should == []
    end
    
    specify "[m..n] should provide a subarray where m, n negatives and m < n (##{cmd})" do
      [ "a", "b", "c", "d", "e" ].send(cmd, -3..-2).should == ["c", "d"]
    end
    
    specify "[0..0] should return an array containing the first element (##{cmd})" do
      [1, 2, 3, 4, 5].send(cmd, 0..0).should == [1]
    end
    
    specify "[0..-1] should return the entire array (##{cmd})" do
      [1, 2, 3, 4, 5].send(cmd, 0..-1).should == [1, 2, 3, 4, 5]
    end
    
    specify "[0...-1] should return all but the last element (##{cmd})" do
      [1, 2, 3, 4, 5].send(cmd, 0...-1).should == [1, 2, 3, 4]
    end

    specify "returns [3] for [2..-1] out of [1, 2, 3]  (##{cmd}) <Specifies bug found by brixen, Defiler, mae>" do
      [1,2,3].send(cmd, 2..-1).should == [3]
    end
    
    specify "[m..n] should return an empty array when m > n and m, n are positive (##{cmd})" do
      [1, 2, 3, 4, 5].send(cmd, 3..2).should == []
    end
    
    specify "[m..n] should return an empty array when m > n and m, n are negative (##{cmd})" do
      [1, 2, 3, 4, 5].send(cmd, -2..-3).should == []
    end
  
    specify "slice with indices outside of array should not expand array" do
      a = [1, 2]
      a.send(cmd, 4).should == nil
      a.should == [1, 2]
      a.send(cmd, 4, 0).should == nil
      a.should == [1, 2]
      a.send(cmd, 6, 1).should == nil
      a.should == [1, 2]
      a.send(cmd, 8...8).should == nil
      a.should == [1, 2]
      a.send(cmd, 10..10).should == nil
      a.should == [1, 2]
    end
    
    specify "slice on array subclasses should return subclass instance" do
      ary = MyArray[1, 2, 3]
      ary.send(cmd, 0, 0).class.should == MyArray
      ary.send(cmd, 0, 2).class.should == MyArray
      ary.send(cmd, 0..10).class.should == MyArray
    end
  end                         # [] slice .each
end

describe 'Array splicing using #[]=' do
  specify "[]= should set the value of the element at index" do
    a = [1, 2, 3, 4]
    a[2] = 5
    a[-1] = 6
    a[5] = 3
    a.should == [1, 2, 5, 6, nil, 3]

    a = []
    a[4] = "e"
    a.should == [nil, nil, nil, nil, "e"]
    a[3] = "d"
    a.should == [nil, nil, nil, "d", "e"]
    a[0] = "a"
    a.should == ["a", nil, nil, "d", "e"]
    a[-3] = "C"
    a.should == ["a", nil, "C", "d", "e"]
    a[-1] = "E"
    a.should == ["a", nil, "C", "d", "E"]
    a[-5] = "A"
    a.should == ["A", nil, "C", "d", "E"]
    a[5] = "f"
    a.should == ["A", nil, "C", "d", "E", "f"]
    a[1] = []
    a.should == ["A", [], "C", "d", "E", "f"]
    a[-1] = nil
    a.should == ["A", [], "C", "d", "E", nil]
  end
  
  specify "[]= should remove the section defined by start, length when set to nil" do
    a = ['a', 'b', 'c', 'd', 'e']
    a[1, 3] = nil
    a.should == ["a", "e"]
  end
  
  specify "[]= should set the section defined by start, length to other" do
    a = [1, 2, 3, 4, 5, 6]
    a[0, 1] = 2
    a[3, 2] = ['a', 'b', 'c', 'd']
    a.should == [2, 2, 3, "a", "b", "c", "d", 6]
  end
  
  specify "[]= should remove the section defined by range when set to nil" do
    a = [1, 2, 3, 4, 5]
    a[0..1] = nil
    a.should == [3, 4, 5]
  end
  
  specify "[]= should set the section defined by range to other" do
    a = [6, 5, 4, 3, 2, 1]
    a[1...2] = 9
    a[3..6] = [6, 6, 6]
    a.should == [6, 9, 4, 6, 6, 6]
  end

  specify "[0]= returns value assigned" do
    a = [1, 2, 3, 4, 5]
    (a[0] = 6).should == 6
  end
  
  specify "[idx]= returns value assigned if idx is inside array" do
    a = [1, 2, 3, 4, 5]
    (a[3] = 6).should == 6
  end
  
  specify "[idx]= returns value assigned if idx is right beyond right array boundary" do
    a = [1, 2, 3, 4, 5]
    (a[5] = 6).should == 6
  end
  
  specify "[idx]= returns value assigned if idx far beyond right array boundary" do
    a = [1, 2, 3, 4, 5]
    (a[10] = 6).should == 6
  end

  specify "[idx, cnt]= returns non-array value if non-array value assigned" do
    a = [1, 2, 3, 4, 5]
    (a[2, 3] = 10).should == 10
  end

  specify "[idx, cnt]= returns array if array assigned" do
    a = [1, 2, 3, 4, 5]
    (a[2, 3] = [4, 5]).should == [4, 5]
  end

  specify "[m..n]= returns non-array value if non-array value assigned" do
    a = [1, 2, 3, 4, 5]
    (a[2..4] = 10).should == 10
  end
  
  specify "[m..n]= returns array if array assigned" do
    a = [1, 2, 3, 4, 5]
    (a[2..4] = [7, 8]).should == [7, 8]
  end
  
  specify "[idx]= sets the value of the element at index" do
      a = [1, 2, 3, 4]
      a[2] = 5
      a[-1] = 6
      a[5] = 3
      a.should == [1, 2, 5, 6, nil, 3]
    end

  specify "[idx]= with idx right beyond array boundary should set the value of the element" do
    a = [1, 2, 3, 4]
    a[4] = 8
    a.should == [1, 2, 3, 4, 8]
  end
    
  specify "[idx, cnt]= removes the section defined by start, length when set to nil" do
      a = ['a', 'b', 'c', 'd', 'e']
      a[1, 3] = nil
      a.should == ["a", "e"]
    end
    
  specify "[idx, cnt]= removes the section when set to nil if negative index within bounds and cnt > 0" do
    a = ['a', 'b', 'c', 'd', 'e']
    a[-3, 2] = nil
    a.should == ["a", "b", "e"]
  end
  
  specify "[idx, cnt]= replaces the section defined by start, length to other" do
      a = [1, 2, 3, 4, 5, 6]
      a[0, 1] = 2
      a[3, 2] = ['a', 'b', 'c', 'd']
      a.should == [2, 2, 3, "a", "b", "c", "d", 6]
    end

  specify "[idx, cnt]= replaces the section to other if idx < 0 and cnt > 0" do
    a = [1, 2, 3, 4, 5, 6]
    a[-3, 2] = ["x", "y", "z"]
    a.should == [1, 2, 3, "x", "y", "z", 6]
  end

  specify "[idx, cnt]= replaces the section to other even if cnt spanning beyond the array boundary" do
    a = [1, 2, 3, 4, 5]
    a[-1, 3] = [7, 8]
    a.should == [1, 2, 3, 4, 7, 8]
  end

  specify '[idx, cnt]= pads the Array with nils if the span is past the end' do
    a = [1, 2, 3, 4, 5]
    a[10, 1] = [1]
    a.should == [1, 2, 3, 4, 5, nil, nil, nil, nil, nil, 1]

    b = [1, 2, 3, 4, 5]
    b[10, 0] = [1]
    a.should == [1, 2, 3, 4, 5, nil, nil, nil, nil, nil, 1]
  end

  specify "[idx, 0]= inserts other section in place defined by idx" do
    a = [1, 2, 3, 4, 5]
    a[3, 0] = [7, 8]
    a.should == [1, 2, 3, 7, 8, 4, 5]
  end
  
  specify "[idx, count]= with start and negative length should raise" do
    a = [1, 2, 3, 4]
    should_raise(IndexError) { a[-2, -1] = "" }
    should_raise(IndexError) { a[0, -1] = "" }
    should_raise(IndexError) { a[2, -1] = "" }
    should_raise(IndexError) { a[4, -1] = "" }
    should_raise(IndexError) { a[10, -1] = "" }
    should_raise(IndexError) { [1, 2, 3, 4,  5][2, -1] = [7, 8] }
  end

  specify "[idx, count]= with start, length should set elements" do
    a = [];   a[0, 0] = nil;            a.should == []
    a = [];   a[2, 0] = nil;            a.should == [nil, nil]
    a = [];   a[0, 2] = nil;            a.should == []
    a = [];   a[2, 2] = nil;            a.should == [nil, nil]

    a = [];   a[0, 0] = [];             a.should == []
    a = [];   a[2, 0] = [];             a.should == [nil, nil]
    a = [];   a[0, 2] = [];             a.should == []
    a = [];   a[2, 2] = [];             a.should == [nil, nil]

    a = [];   a[0, 0] = ["a"];          a.should == ["a"]
    a = [];   a[2, 0] = ["a"];          a.should == [nil, nil, "a"]
    a = [];   a[0, 2] = ["a"];          a.should == ["a"]
    a = [];   a[2, 2] = ["a"];          a.should == [nil, nil, "a"]
    
    a = [];   a[0, 0] = ["a","b"];      a.should == ["a", "b"]
    a = [];   a[2, 0] = ["a","b"];      a.should == [nil, nil, "a", "b"]
    a = [];   a[0, 2] = ["a","b"];      a.should == ["a", "b"]
    a = [];   a[2, 2] = ["a","b"];      a.should == [nil, nil, "a", "b"]

    a = [];   a[0, 0] = ["a","b","c"];  a.should == ["a", "b", "c"]
    a = [];   a[2, 0] = ["a","b","c"];  a.should == [nil, nil, "a", "b", "c"]
    a = [];   a[0, 2] = ["a","b","c"];  a.should == ["a", "b", "c"]
    a = [];   a[2, 2] = ["a","b","c"];  a.should == [nil, nil, "a", "b", "c"]
    
    a = [1, 2, 3, 4]
    a[0, 0] = [];         a.should == [1, 2, 3, 4]
    a[1, 0] = [];         a.should == [1, 2, 3, 4]
    a[-1,0] = [];         a.should == [1, 2, 3, 4]

    a = [1, 2, 3, 4]
    a[0, 0] = [8, 9, 9];  a.should == [8, 9, 9, 1, 2, 3, 4]
    a = [1, 2, 3, 4]
    a[1, 0] = [8, 9, 9];  a.should == [1, 8, 9, 9, 2, 3, 4]
    a = [1, 2, 3, 4]
    a[-1,0] = [8, 9, 9];  a.should == [1, 2, 3, 8, 9, 9, 4]
    a = [1, 2, 3, 4]
    a[4, 0] = [8, 9, 9];  a.should == [1, 2, 3, 4, 8, 9, 9]

    a = [1, 2, 3, 4]
    a[0, 1] = [9];        a.should == [9, 2, 3, 4]
    a[1, 1] = [8];        a.should == [9, 8, 3, 4]
    a[-1,1] = [7];        a.should == [9, 8, 3, 7]
    a[4, 1] = [9];        a.should == [9, 8, 3, 7, 9]

    a = [1, 2, 3, 4]
    a[0, 1] = [8, 9];     a.should == [8, 9, 2, 3, 4]
    a = [1, 2, 3, 4]
    a[1, 1] = [8, 9];     a.should == [1, 8, 9, 3, 4]
    a = [1, 2, 3, 4]
    a[-1,1] = [8, 9];     a.should == [1, 2, 3, 8, 9]
    a = [1, 2, 3, 4]
    a[4, 1] = [8, 9];     a.should == [1, 2, 3, 4, 8, 9]
    
    a = [1, 2, 3, 4]
    a[0, 2] = [8, 9];     a.should == [8, 9, 3, 4]
    a = [1, 2, 3, 4]
    a[1, 2] = [8, 9];     a.should == [1, 8, 9, 4]
    a = [1, 2, 3, 4]
    a[-2,2] = [8, 9];     a.should == [1, 2, 8, 9]
    a = [1, 2, 3, 4]
    a[-1,2] = [8, 9];     a.should == [1, 2, 3, 8, 9]
    a = [1, 2, 3, 4]
    a[4, 2] = [8, 9];     a.should == [1, 2, 3, 4, 8, 9]

    a = [1, 2, 3, 4]
    a[0, 2] = [7, 8, 9];  a.should == [7, 8, 9, 3, 4]
    a = [1, 2, 3, 4]
    a[1, 2] = [7, 8, 9];  a.should == [1, 7, 8, 9, 4]
    a = [1, 2, 3, 4]
    a[-2,2] = [7, 8, 9];  a.should == [1, 2, 7, 8, 9]
    a = [1, 2, 3, 4]
    a[-1,2] = [7, 8, 9];  a.should == [1, 2, 3, 7, 8, 9]
    a = [1, 2, 3, 4]
    a[4, 2] = [7, 8, 9];  a.should == [1, 2, 3, 4, 7, 8, 9]
    
    a = [1, 2, 3, 4]
    a[0, 2] = [1, 1.25, 1.5, 1.75, 2]
    a.should == [1, 1.25, 1.5, 1.75, 2, 3, 4]
    a[1, 1] = a[3, 1] = []
    a.should == [1, 1.5, 2, 3, 4]
    a[0, 2] = [1]
    a.should == [1, 2, 3, 4]
    a[5, 0] = [4, 3, 2, 1]
    a.should == [1, 2, 3, 4, nil, 4, 3, 2, 1]
    a[-2, 5] = nil
    a.should == [1, 2, 3, 4, nil, 4, 3]
    a[-2, 5] = []
    a.should == [1, 2, 3, 4, nil]
    a[0, 2] = nil
    a.should == [3, 4, nil]
    a[0, 100] = [1, 2, 3]
    a.should == [1, 2, 3]
    a[0, 2] *= 2
    a.should == [1, 2, 1, 2, 3]
    a[0, 2] |= [2, 3, 4]
    a.should == [1, 2, 3, 4, 1, 2, 3]
    a[2, 0] += [3, 2, 2]
    a.should == [1, 2, 3, 2, 2, 3, 4, 1, 2, 3]
    a[0, 4] -= [2, 3]
    a.should == [1, 2, 3, 4, 1, 2, 3]
    a[0, 6] &= [4]
    a.should == [4, 3]
  end
  
  specify "[]= should call to_int on its start and length arguments" do
    obj = Object.new
    def obj.to_int() 2 end
      
    a = [1, 2, 3, 4]
    a[obj, 0] = [9]
    a.should == [1, 2, 9, 3, 4]
    a[obj, obj] = []
    a.should == [1, 2, 4]
    a[obj] = -1
    a.should == [1, 2, -1]
  end

  specify "[m..n]= removes the section defined by range when set to nil" do
      a = [1, 2, 3, 4, 5]
      a[0..1] = nil
      a.should == [3, 4, 5]
    end

  specify "[m..n]= removes the section when set to nil if m and n < 0" do
    a = [1, 2, 3, 4, 5]
    a[-3..-2] = nil
    a.should == [1, 2, 5]
  end
    
  specify "[m..n]= replaces the section defined by range" do
      a = [6, 5, 4, 3, 2, 1]
      a[1...2] = 9
      a[3..6] = [6, 6, 6]
      a.should == [6, 9, 4, 6, 6, 6]
    end

  specify "[m..n]= replaces the section if m and n < 0" do
    a = [1, 2, 3, 4, 5]
    a[-3..-2] = [7, 8, 9]
    a.should == [1, 2, 7, 8, 9, 5]
  end

  specify "[m..n]= replaces the section if m < 0 and n > 0" do
    a = [1, 2, 3, 4, 5]
    a[-4..3] = [8]
    a.should == [1, 8, 5]
  end

  specify "[m..n]= inserts the other section at m if m > n" do
    a = [1, 2, 3, 4, 5]
    a[3..1] = [8]
    a.should == [1, 2, 3, 8, 4, 5]
  end
  
  specify "[]= with range argument should set elements" do
    ary = [1, 2, 3]
    rhs = [nil, [], ["x"], ["x", "y"]]
    (0 .. ary.size + 2).each do |a|
      (a .. ary.size + 3).each do |b|
        rhs.each do |c|
          ary1 = ary.dup
          ary1[a .. b] = c
          ary2 = ary.dup
          ary2[a, 1 + b-a] = c
          ary1.should == ary2
          
          ary1 = ary.dup
          ary1[a ... b] = c
          ary2 = ary.dup
          ary2[a, b-a] = c
          ary1.should == ary2
        end
      end
    end

    # Now we only have to test cases where the start, length interface would
    # have raise an exception because of negative size
    ary[1...1] = [5]
    ary.should == [1, 5, 2, 3]
    ary[1..0] = [4, 3]
    ary.should == [1, 4, 3, 5, 2, 3]
    ary[-1..0] = nil
    ary.should == [1, 4, 3, 5, 2, 3]
    ary[-3..2] = []
    ary.should == [1, 4, 3, 5, 2, 3]
    ary[4..2] = []
    ary.should == [1, 4, 3, 5, 2, 3]
  end

  specify "[]= with range should call to_int on range arguments" do
    from = Object.new
    to = Object.new

    # So we can construct a range out of them...
    def from.<=>(o) 0 end
    def to.<=>(o) 0 end

    def from.to_int() 1 end
    def to.to_int() -2 end
      
    a = [1, 2, 3, 4]
      
    a[from .. to] = ["a", "b", "c"]
    a.should == [1, "a", "b", "c", 4]

    a[1..0] = ["x"]
    a.should == [1, "x", "a", "b", "c", 4]
    should_raise(TypeError) { a["a" .. "b"] = [] }
    should_raise(TypeError) { a[from .. "b"] = [] }
  end
  
  specify "[]= with negative index beyond array should raise" do
    a = [1, 2, 3, 4]
    should_raise(IndexError) { a[-5] = "" }
    should_raise(IndexError) { a[-5, -1] = "" }
    should_raise(IndexError) { a[-5, 0] = "" }
    should_raise(IndexError) { a[-5, 1] = "" }
    should_raise(IndexError) { a[-5, 2] = "" }
    should_raise(IndexError) { a[-5, 10] = "" }
    
    should_raise(RangeError) { a[-5..-5] = "" }
    should_raise(RangeError) { a[-5...-5] = "" }
    should_raise(RangeError) { a[-5..-4] = "" }
    should_raise(RangeError) { a[-5...-4] = "" }
    should_raise(RangeError) { a[-5..10] = "" }
    should_raise(RangeError) { a[-5...10] = "" }
    
    # ok
    a[0..-9] = [1]
    a.should == [1, 1, 2, 3, 4]
  end
  
  specify "[]= should call to_ary on its rhs argument for multi-element sets" do
    obj = Object.new
    def obj.to_ary() [1, 2, 3] end
    ary = [1, 2]
    ary[0, 0] = obj
    ary.should == [1, 2, 3, 1, 2]
    ary[1, 10] = obj
    ary.should == [1, 1, 2, 3]
  end
  
  specify "[]= shouldn't call to_ary on rhs array subclasses for multi-element sets" do
    ary = []
    ary[0, 0] = ToAryArray[5, 6, 7]
    ary.should == [5, 6, 7]
  end
end

context "On a frozen array" do
  ary = [1, 2, 3]
  ary.freeze
  
  specify "<< should raise" do
    should_raise(TypeError) { ary << 5 }
  end

  specify "clear should raise" do
    should_raise(TypeError) { ary.clear }
  end

  specify "collect! should raise" do
    should_raise(TypeError) { ary.collect! {} }
  end

  specify "compact! should raise" do
    should_raise(TypeError) { ary.compact! }
  end

  specify "concat should raise" do
    ary.concat([]) # ok
    should_raise(TypeError) { ary.concat([1]) }
  end

  specify "delete should raise" do
    ary.delete(0) # ok, not in array
    should_raise(TypeError) { ary.delete(1) }
  end

  specify "delete_at should raise" do
    should_raise(TypeError) { ary.delete_at(0) }
  end

  specify "delete_if should raise" do
    should_raise(TypeError) { ary.delete_if {} }
  end

  specify "fill should raise" do
    should_raise(TypeError) { ary.fill('x') }
  end

  specify "flatten! should raise" do
    ary.flatten! # ok, already flat
    nested_ary = [1, 2, []]
    nested_ary.freeze
    should_raise(TypeError) { nested_ary.flatten! }
  end

  specify "insert should raise" do
    ary.insert(0) # ok
    should_raise(TypeError) { ary.insert(0, 'x') }
  end

  specify "map! should raise" do
    should_raise(TypeError) { ary.map! {} }
  end

  specify "pop should raise" do
    should_raise(TypeError) { ary.pop }
  end

  specify "push should raise" do
    ary.push() # ok
    should_raise(TypeError) { ary.push(1) }
  end

  specify "reject! should raise" do
    should_raise(TypeError) { ary.reject! {} }
  end

  specify "replace should raise" do
    should_raise(TypeError) { ary.replace(ary) }
  end

  specify "reverse! should raise" do
    should_raise(TypeError) { ary.reverse! }
  end

  specify "shift should raise" do
    should_raise(TypeError) { ary.shift }
  end

  specify "slice! should raise" do
    should_raise(TypeError) { ary.slice!(0, 0) }
  end

  specify "sort! should raise" do
    should_raise(TypeError) { ary.sort! }
  end

  specify "uniq! should raise" do
    ary.uniq! # ok, already uniq
    dup_ary = [1, 1, 2]
    dup_ary.freeze
    should_raise(TypeError) { dup_ary.uniq! }
  end

  specify "unshift should raise" do
    ary.unshift() # ok
    should_raise(TypeError) { ary.unshift(1) }
  end
end


# Redundant, should be in Object --rue
context "Array inherited instance method" do
 specify "instance_variable_get should return the value of the instance variable" do
    a = []
    a.instance_variable_set(:@c, 1)
    a.instance_variable_get(:@c).should == 1
 end
 
 specify "instance_variable_get should return nil if the instance variable does not exist" do
    [].instance_variable_get(:@c).should == nil
 end
 
 specify "instance_variable_get should raise NameError if the argument is not of form '@x'" do
    should_raise(NameError) { [].instance_variable_get(:c) }
 end
end

describe 'Array packing' do
  #      Directive    Meaning
  #      ---------------------------------------------------------------
  #          @     |  Moves to absolute position
  #          B     |  Bit string (descending bit order)
  #          b     |  Bit string (ascending bit order)
  #          D, d  |  Double-precision float, native format
  #          E     |  Double-precision float, little-endian byte order
  #          e     |  Single-precision float, little-endian byte order
  #          F, f  |  Single-precision float, native format
  #          G     |  Double-precision float, network (big-endian) byte order
  #          g     |  Single-precision float, network (big-endian) byte order
  #          H     |  Hex string (high nibble first)
  #          h     |  Hex string (low nibble first)
  #          I     |  Unsigned integer
  #          i     |  Integer
  #          L     |  Unsigned long
  #          l     |  Long
  #          N     |  Long, network (big-endian) byte order
  #          n     |  Short, network (big-endian) byte-order
  #          P     |  Pointer to a structure (fixed-length string)
  #          p     |  Pointer to a null-terminated string
  #          Q, q  |  64-bit number
  #          S     |  Unsigned short
  #          s     |  Short
  #          U     |  UTF-8
  #          V     |  Long, little-endian byte order
  #          v     |  Short, little-endian byte order
  #          w     |  BER-compressed integer\fnm

  specify "pack('%') raises ArgumentError" do
    should_raise(ArgumentError) { [].pack("%") }
  end

  specify "pack('#') skips everything till the end of schema string" do
    ["abc", "def"].pack("A*#A10%").should == "abc"
  end

  specify "pack('#') skips everything till the end of schema line" do
    ["abc", "def"].pack("A*#A10%\nA*").should == "abcdef"
  end

  specify "pack('A') returns space padded string" do
    ['abcde'].pack('A7').should == 'abcde  '
  end

  specify "pack('A') cuts string if its size greater than directive count" do
    ['abcde'].pack('A3').should == 'abc'
  end

  specify "pack('A') consider count = 1 if count omited" do
    ['abcde'].pack('A').should == 'a'
  end

  specify "pack('A') returns empty string if count = 0" do
    ['abcde'].pack('A0').should == ''
  end

  specify "pack('A') returns the whole argument string with star parameter" do
    ['abcdef'].pack('A*').should == 'abcdef'
  end

  specify "pack('A') raises TypeError if array item is not String" do
    should_raise(TypeError) { [123].pack('A5') }
    should_raise(TypeError) { [:hello].pack('A5') }
    should_raise(TypeError) { [Object.new].pack('A5') }
  end

  specify "pack('A') should work with multi-digit padding sizes" do
    ['abcdef'].pack('A10').should == "abcdef    "
  end

  specify "pack('a') returns null padded string" do
    ['abcdef'].pack('a7').should == "abcdef\x0"
  end

  specify "pack('a') cuts string if its size greater than directive count" do
    ['abcde'].pack('a3').should == 'abc'
  end

  specify "pack('a') consider count = 1 if count omited" do
    ['abcde'].pack('a').should == 'a'
  end

  specify "pack('a') returns empty string if count = 0" do
    ['abcde'].pack('a0').should == ''
  end

  specify "pack('a') returns the whole argument string with star parameter" do
    ['abcdef'].pack('a*').should == 'abcdef'
  end

  specify "pack('a') raises TypeError if array item is not String" do
    should_raise(TypeError) { [123].pack('a5') }
    should_raise(TypeError) { [:hello].pack('a5') }
    should_raise(TypeError) { [Object.new].pack('a5') }
  end

  specify "pack('B') returns packed bit-string descending order" do
    ["011000010110001001100011"].pack('B24').should == 'abc'
  end

  specify "pack('B') uses char codes to determine if bit is set or not" do
    ["bccddddefgghhhijjkklllmm"].pack('B24').should == ["011000010110001001100011"].pack('B24')
  end

  specify "pack('B') conversion edge case: all zeros" do
    ["00000000"].pack('B8').should == "\000"
  end

  specify "pack('B') conversion edge case: all ones" do
    ["11111111"].pack('B8').should == "\377"
  end

  specify "pack('B') conversion edge case: left one" do
    ["10000000"].pack('B8').should == "\200"
  end

  specify "pack('B') conversion edge case: left one" do
    ["00000001"].pack('B8').should == "\001"
  end

  specify "pack('B') conversion edge case: edge sequences not in first char" do
    ["0000000010000000000000011111111100000000"].pack('B40').should == "\000\200\001\377\000"
  end

  specify "pack('B') uses zeros if count is not multiple of 8" do
    ["00111111"].pack('B4').should == ["00110000"].pack('B8')
  end

  specify "pack('B') returns zero-char for each 2 of count that greater than string length" do
    [""].pack('B6').should == "\000\000\000"
  end

  specify "pack('B') returns extra zero char if count is odd and greater than string length" do
    [""].pack('B7').should == "\000\000\000\000"
  end

  specify "pack('B') starts new char if string is ended before char's 8 bits" do
    ["0011"].pack('B8').should == "0\000\000"
  end

  specify "pack('B') considers count = 1 if no explicit count it given" do
    ["10000000"].pack('B').should == ["10000000"].pack('B1')
    ["01000000"].pack('B').should == ["01000000"].pack('B1')
  end

  specify "pack('B') returns empty string if count = 0" do
    ["10101010"].pack('B0').should == ""
  end

  specify "pack('B') uses argument string length as count if count = *" do
    ["00111111010"].pack('B*').should == ["00111111010"].pack('B11')
  end

  specify "pack('B') consumes only one array item" do
    ["0011", "1111"].pack('B*').should == ["0011"].pack('B4')
    ["0011", "1011"].pack('B*B*').should == ["0011"].pack('B4') + ["1011"].pack('B4')
  end

  specify "pack('B') raises TypeError if corresponding array item is not String" do
    should_raise(TypeError) { [123].pack('B8') }
    should_raise(TypeError) { [:data].pack('B8') }
    should_raise(TypeError) { [Object.new].pack('B8') }
  end

  specify "pack('b') returns packed bit-string descending order" do
    ["100001100100011011000110"].pack('b24').should == 'abc'
  end

  specify "pack('b') conversion edge case: all zeros" do
    ["00000000"].pack('b8').should == "\000"
  end

  specify "pack('b') conversion edge case: all ones" do
    ["11111111"].pack('b8').should == "\377"
  end

  specify "pack('b') conversion edge case: left one" do
    ["10000000"].pack('b8').should == "\001"
  end

  specify "pack('b') conversion edge case: left one" do
    ["00000001"].pack('b8').should == "\200"
  end

  specify "pack('b') conversion edge case: edge sequences not in first char" do
    ["0000000010000000000000011111111100000000"].pack('b40').should == "\000\001\200\377\000"
  end

  specify "pack('b') uses char codes to determine if bit is set or not" do
    ["abbbbccddefffgghiijjjkkl"].pack('b24').should == ["100001100100011011000110"].pack('b24')
  end

  specify "pack('b') uses zeros if count is not multiple of 8" do
    ["00111111"].pack('b4').should == ["00110000"].pack('b8')
  end

  specify "pack('b') returns zero-char for each 2 of count that greater than string length" do
    [""].pack('b6').should == "\000\000\000"
  end

  specify "pack('b') returns extra zero char if count is odd and greater than string length" do
    [""].pack('b7').should == "\000\000\000\000"
  end

  specify "pack('b') starts new char if argument string is ended before char's 8 bits" do
    ["0011"].pack('b8').should == "\f\000\000"
  end

  specify "pack('b') considers count = 1 if no explicit count it given" do
    ["10000000"].pack('b').should == ["10000000"].pack('b1')
    ["01000000"].pack('b').should == ["01000000"].pack('b1')
  end

  specify "pack('b') returns empty string if count = 0" do
    ["10101010"].pack('b0').should == ""
  end

  specify "pack('b') uses argument string length as count if count = *" do
    ["00111111010"].pack('b*').should == ["00111111010"].pack('b11')
  end

  specify "pack('b') consumes only one array item" do
    ["0011", "1111"].pack('b*').should == ["0011"].pack('b4')
    ["0011", "1011"].pack('b*b*').should == ["0011"].pack('b4') + ["1011"].pack('b4')
  end

  specify "pack('b') raises TypeError if corresponding array item is not String" do
    should_raise(TypeError) { [123].pack('b8') }
    should_raise(TypeError) { [:data].pack('b8') }
    should_raise(TypeError) { [Object.new].pack('b8') }
  end

  specify "pack('C') returns string with char of appropriate number" do
    [49].pack('C').should == '1'
  end

  specify "pack('C') reduces value to fit in byte" do
    [257].pack('C').should == "\001"
  end

  specify "pack('C') converts negative values to positive" do
    [-1].pack('C').should == [255].pack('C')
    [-257].pack('C').should == [255].pack('C')
  end

  specify "pack('C') converts float to integer and returns char with that number" do
    [5.0].pack('C').should == [5].pack('C')
  end

  specify "pack('C') calls to_i on symbol and returns char with that number" do
    [:hello].pack('C').should == [:hello.to_i].pack('C')
  end

  specify "pack('C') raises TypeErorr if value is string" do
    should_raise(TypeError) { ["hello"].pack('C') }
  end

  specify "pack('C') processes count number of array elements if count given" do
    [1, 2, 3].pack('C3').should == "\001\002\003"
  end

  specify "pack('C') returns empty string if count = 0" do
    [1, 2, 3].pack('C0').should == ''
  end

  specify "pack('C') with star parameter processes all remaining array items" do
    [1, 2, 3, 4, 5].pack('C*').should == "\001\002\003\004\005"
  end

  specify "pack('C') raises ArgumentError if count is greater than array elements left" do
    should_raise(ArgumentError) { [1, 2].pack('C3') }
  end

  specify "pack('c') returns string with char of appropriate number" do
    [49].pack('c').should == '1'
  end

  specify "pack('c') reduces value to fit in byte" do
    [257].pack('c').should == "\001"
  end

  specify "pack('c') converts negative values to positive" do
    [-1].pack('c').should == [255].pack('C')
    [-257].pack('c').should == [255].pack('C')
  end

  specify "pack('c') converts float to integer and returns char with that number" do
    [5.0].pack('c').should == [5].pack('c')
  end

  specify "pack('c') calls to_i on symbol and returns char with that number" do
    [:hello].pack('c').should == [:hello.to_i].pack('c')
  end

  specify "pack('c') raises TypeErorr if value is string" do
    should_raise(TypeError) { ["hello"].pack('c') }
  end

  specify "pack('c') processes count number of array elements if count given" do
    [1, 2, 3].pack('c3').should == "\001\002\003"
  end

  specify "pack('c') returns empty string if count = 0" do
    [1, 2, 3].pack('c0').should == ''
  end

  specify "pack('c') with star parameter processes all remaining array items" do
    [1, 2, 3, 4, 5].pack('c*').should == "\001\002\003\004\005"
  end

  specify "pack('c') raises ArgumentError if count is greater than array elements left" do
    should_raise(ArgumentError) { [1, 2].pack('c3') }
  end

  specify "pack('M') enocdes string with Qouted Printable encoding" do
    ["ABCDEF"].pack('M').should == "ABCDEF=\n"
  end

  specify "pack('M') doesn't encode new line chars" do
    ["\nA"].pack('M').should == "\nA=\n"
  end

  specify "pack('M') always appends soft line break at the end of encoded string" do
    ["ABC"].pack('M')[-2, 2].should == "=\n"
  end

  specify "pack('M') appends soft line break after each 72 chars + 1 encoded char in encoded string" do
    s = ["A"*150].pack('M')
    s[73, 2].should == "=\n"
    s[148, 2].should == "=\n"

    s = ["A"*72+"\001"].pack('M')
    s[75, 2].should == "=\n"
  end

  specify "pack('M') doesn't quote chars 32..60 and 62..126)" do
    32.upto(60) do |i|
     [i.chr].pack('M').should == i.chr+"=\n"
    end

    62.upto(126) do |i|
     [i.chr].pack('M').should == i.chr+"=\n"
    end
  end

  specify "pack('M') quotes chars by adding equal sign and char's hex value" do
    ["\001"].pack('M').should == "=01=\n"
  end

  specify "pack('M') quotes equal sign" do
    ["="].pack('M').should == "=3D=\n"
  end

  specify "pack('M') doesn't quote \\t char" do
    ["\t"].pack('M').should == "\t=\n"
  end

  specify "pack('M') returns empty string if source string is empty" do
    [""].pack('M').should == ""
  end

  specify "pack('M') calls to_s on object to convert to string" do
    class X; def to_s; "unnamed object"; end; end

    [123].pack('M').should == "123=\n"
    [:hello].pack('M').should == "hello=\n"
    [X.new].pack('M').should == "unnamed object=\n"
  end

  specify "pack('M') ignores count parameter" do
    ["ABC", "DEF", "GHI"].pack('M0').should == ["ABC"].pack('M')
    ["ABC", "DEF", "GHI"].pack('M3').should == ["ABC"].pack('M')
  end

  specify "pack('M') ignores star parameter" do
    ["ABC", "DEF", "GHI"].pack('M*').should == ["ABC"].pack('M')
  end

  specify "pack('m') encodes string with Base64 encoding" do
    ["ABCDEF"].pack('m').should == "QUJDREVG\n"
  end

  specify "pack('m') converts series of 3-char sequences into four 4-char sequences" do
    ["ABCDEFGHI"].pack('m').size.should == 4+4+4+1
  end

  specify "pack('m') fills chars with non-significant bits with '=' sign" do
    ["A"].pack('m').should == "QQ==\n"
  end

  specify "pack('m') appends newline at the end of result string" do
    ["A"].pack('m')[-1, 1].should == "\n"
  end

  specify "pack('m') appends newline after each 60 chars in result string" do
    s = ["ABC"*31].pack('m')
    s[60, 1].should == "\n"
    s[121, 1].should == "\n"
  end

  specify "pack('m') encodes 6-bit char less than 26 with capital letters" do
    [( 0*4).chr].pack('m').should == "AA==\n"
    [( 1*4).chr].pack('m').should == "BA==\n"

    [(25*4).chr].pack('m').should == "ZA==\n"
  end

  specify "pack('m') encodes 6-bit char from 26 to 51 with lowercase letters" do
    [(26*4).chr].pack('m').should == "aA==\n"
    [(27*4).chr].pack('m').should == "bA==\n"

    [(51*4).chr].pack('m').should == "zA==\n"
  end

  specify "pack('m') encodes 6-bit char 62 with '+'" do
    [(62*4).chr].pack('m').should == "+A==\n"
  end

  specify "pack('m') encodes 6-bit char 63 with '/'" do
    [(63*4).chr].pack('m').should == "/A==\n"
  end

  specify "pack('m') returns empty string if source string is empty" do
    [""].pack('m').should == ""
  end

  specify "pack('m') raises TypeError if corresponding array item is not string" do
    should_raise(TypeError) { [123].pack('m') }
    should_raise(TypeError) { [:hello].pack('m') }
    should_raise(TypeError) { [Object.new].pack('m') }
  end

  specify "pack('m') ignores count parameter" do
    ["ABC", "DEF", "GHI"].pack('m0').should == ["ABC"].pack('m')
    ["ABC", "DEF", "GHI"].pack('m3').should == ["ABC"].pack('m')
  end

  specify "pack('m') ignores star parameter" do
    ["ABC", "DEF", "GHI"].pack('m*').should == ["ABC"].pack('m')
  end

  specify "pack('u') encodes string with UU-encoding" do
    ["ABCDEF"].pack('u').should == "&04)#1$5&\n"
  end

  specify "pack('u') converts series of 3-char sequences into four 4-char sequences" do
    ["ABCDEFGHI"].pack('u').size.should == 4+4+4+1+1
  end

  specify "pack('u') appends zero-chars to source string if string length is not multiple of 3" do
    ["A"].pack('u').should == "!00``\n"
  end

  specify "pack('u') appends newline at the end of result string" do
    ["A"].pack('u')[-1, 1].should == "\n"
  end

  specify "pack('u') splits source string into lines with no more than 45 chars" do
    s = ["A"*91].pack('u')
    s[61, 1].should == "\n"
    s[123, 1].should == "\n"
  end

  specify "pack('u') prepends encoded line length to each line" do
    s = ["A"*50].pack('u')
    s[ 0].should == 45+32
    s[62].should ==  5+32
  end

  specify "pack('u') encodes 6-bit char with another char starting from char 32" do
    [( 1*4).chr].pack('u').should == "!!```\n"
    [(16*4).chr].pack('u').should == "!0```\n"
    [(25*4).chr].pack('u').should == "!9```\n"
    [(63*4).chr].pack('u').should == "!_```\n"
  end

  specify "pack('u') replaces spaces in encoded string with grave accent (`) char" do
    [( 0*4).chr].pack('u').should == "!````\n"
  end

  specify "pack('u') returns empty string if source string is empty" do
    [""].pack('u').should == ""
  end

  specify "pack('u') raises TypeError if corresponding array item is not string" do
    should_raise(TypeError) { [123].pack('u') }
    should_raise(TypeError) { [:hello].pack('u') }
    should_raise(TypeError) { [Object.new].pack('u') }
  end

  specify "pack('u') ignores count parameter" do
    ["ABC", "DEF", "GHI"].pack('u0').should == ["ABC"].pack('u')
    ["ABC", "DEF", "GHI"].pack('u3').should == ["ABC"].pack('u')
  end

  specify "pack('u') ignores star parameter" do
    ["ABC", "DEF", "GHI"].pack('u*').should == ["ABC"].pack('u')
  end

  specify "pack('X') decreases result string by one char" do
    ['abcdef'].pack('A4X').should == 'abc'
  end

  specify "pack('X') with count decreases result string by count chars" do
    ['abcdef'].pack('A6X4').should == 'ab'
  end

  specify "pack('X') with zero count doesnt change result string" do
    ['abcdef'].pack('A6X0').should == 'abcdef'
  end

  specify "pack('X') treats start parameter as zero count" do
    ['abcdef'].pack('A6X*').should == 'abcdef'
  end

  specify "pack('X') raises ArgumentError if count greater than already generated string length" do
    should_raise(ArgumentError) { ['abcdef'].pack('A6X7') }
  end

  specify "pack('X') raises ArgumentError if it is first directive" do
    should_raise(ArgumentError) { [].pack('X') }
  end

  specify "pack('x') returns zero-char string" do
    [].pack('x').should == "\000"
  end

  specify "pack('x') with count returns string of count zero chars" do
    [].pack('x5').should == "\000\000\000\000\000"
  end

  specify "pack('x') with count = 0 returns empty string" do
    [].pack('x0').should == ""
  end

  specify "pack('x') with star parameter behaves like with count = 0" do
    [].pack('x*').should == ""
  end

  specify "pack('Z') returns null padded string" do
    ['abcdef'].pack('Z7').should == "abcdef\000"
  end

  specify "pack('Z') cuts string if its size greater than directive count" do
    ['abcde'].pack('Z3').should == 'abc'
  end

  specify "pack('Z') consider count = 1 if count omited" do
    ['abcde'].pack('Z').should == 'a'
  end

  specify "pack('Z') returns empty string if count = 0" do
    ['abcde'].pack('Z0').should == ''
  end

  specify "pack('Z') returns the whole argument string plus null char with star parameter" do
    ['abcdef'].pack('Z*').should == "abcdef\000"
  end

  specify "pack('Z') raises TypeError if array item is not String" do
    should_raise(TypeError) { [123].pack('Z5') }
    should_raise(TypeError) { [:hello].pack('Z5') }
    should_raise(TypeError) { [Object.new].pack('Z5') }
  end
end

# Redundant, should be in Object? --rue
context "Array inherited instance method" do
  specify "instance_variable_get should return the value of the instance variable" do
    a = []
    a.instance_variable_set(:@c, 1)
    a.instance_variable_get(:@c).should == 1
  end

  specify "instance_variable_get should return nil if the instance variable does not exist" do
    [].instance_variable_get(:@c).should == nil
  end

  specify "instance_variable_get should raise NameError if the argument is not of form '@x'" do
    should_raise(NameError) { [].instance_variable_get(:c) }
  end
end
