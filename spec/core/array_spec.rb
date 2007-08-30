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

class MyRange < Range; end

frozen_array = [1, 2, 3]
frozen_array.freeze

describe "Array" do
  it "includes Enumerable" do
    Array.ancestors.include?(Enumerable).should == true
  end
end

describe "Array.new" do
  it "returns a new array when not passed arguments" do
    a = Array.new
    a.class.should == Array
    b = MyArray.new
    b.class.should == MyArray
  end
  
  it "raises ArgumentError when passed a negative size" do
    should_raise(ArgumentError) { Array.new(-1) }
  end
  
  it "returns a new array of size with nil elements" do
    Array.new(5).should == [nil, nil, nil, nil, nil]
    a = MyArray.new(5)
    a.class.should == MyArray
    a.inspect.should == [nil, nil, nil, nil, nil].inspect
  end

  it "calls to_int on size" do
    obj = Object.new
    def obj.to_int() 3 end
    Array.new(obj).should == [nil, nil, nil]
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_int], :returning => 3)
    Array.new(obj).should == [nil, nil, nil]
  end
  
  it "returns a new array of size default objects" do
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
  
  it "returns a new array by calling to_ary on an array-like argument" do
    obj = Object.new
    def obj.to_ary() [:foo] end
    Array.new(obj).should == [:foo]
    
    a = MyArray.new(obj)
    a.class.should == MyArray
    a.inspect.should == [:foo].inspect

    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_ary], :returning => true, :count => :any)
    obj.should_receive(:method_missing, :with => [:to_ary], :returning => [:foo], :count => :any)
    Array.new(obj).should == [:foo]
  end
  
  it "does not call to_ary on Array subclasses when passed an array-like argument" do
    Array.new(ToAryArray[5, 6, 7]).should == [5, 6, 7]
  end
  
  it "calls to_ary on an argument before to_int" do
    obj = Object.new
    def obj.to_ary() [1, 2, 3] end
    def obj.to_int() 3 end

    Array.new(obj).should == [1, 2, 3]
  end
    
  it "returns an array of size elements from the result of passing each index to block" do
    Array.new(5) { |i| i + 1 }.should == [1, 2, 3, 4, 5]
    
    a = MyArray.new(5) { |i| i + 1 }
    a.class.should == MyArray
    a.inspect.should == [1, 2, 3, 4, 5].inspect
  end  

  it "will fail if a to_ary is supplied as the first argument and a second argument is given" do
    should_raise(TypeError) { Array.new([1, 2], 1) } 
  end
end

describe "Array.[]" do
  it "returns a new array populated with the given elements" do
    Array.[](5, true, nil, 'a', "Ruby").should == [5, true, nil, "a", "Ruby"]

    a = MyArray.[](5, true, nil, 'a', "Ruby")
    a.class.should == MyArray
    a.inspect.should == [5, true, nil, "a", "Ruby"].inspect
  end
end

describe "Array[]" do
  it "is a synonym for .[]" do
    Array[5, true, nil, 'a', "Ruby"].should == Array.[](5, true, nil, "a", "Ruby")

    a = MyArray[5, true, nil, 'a', "Ruby"]
    a.class.should == MyArray
    a.inspect.should == [5, true, nil, "a", "Ruby"].inspect
  end
end

describe "Array#<<" do
  it "pushes the object onto the end of the array" do
    ([ 1, 2 ] << "c" << "d" << [ 3, 4 ]).should == [1, 2, "c", "d", [3, 4]]
  end

  it "returns self to allow chaining" do
    a = []
    b = a
    (a << 1).equal?(b).should == true
    (a << 2 << 3).equal?(b).should == true
  end

  it "correctly resizes the Array" do
    a = []
    a.size.should == 0
    a << :foo
    a.size.should == 1
    a << :bar << :baz
    a.size.should == 3
  end  
  
  it "raises TypeError on a frozen array" do
    should_raise(TypeError) { frozen_array << 5 }
  end
end

describe "Array#&" do
  it "creates an array with elements common to both arrays (intersection)" do
    ([] & []).should == []
    ([1, 2] & []).should == []
    ([] & [1, 2]).should == []
    ([ 1, 3, 5 ] & [ 1, 2, 3 ]).should == [1, 3]
  end
  
  it "creates an array with no duplicates" do
    ([ 1, 1, 3, 5 ] & [ 1, 2, 3 ]).uniq!.should == nil
  end
  
  it "creates an array with elements in order they are first encountered" do
    ([ 1, 2, 3, 2, 5 ] & [ 5, 2, 3, 4 ]).should == [2, 3, 5]
  end

  it "does not modify the original Array" do
    a = [1, 1, 3, 5]
    a & [1, 2, 3]
    a.should == [1, 1, 3, 5]
  end

  it "calls to_ary on its argument" do
    obj = Object.new
    def obj.to_ary() [1, 2, 3] end
    ([1, 2] & obj).should == ([1, 2])
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_ary], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_ary], :returning => [1, 2, 3])
    ([1, 2] & obj).should == [1, 2]
  end

  # MRI doesn't actually call eql?() however. So you can't reimplement it.
  it "acts as if using eql?" do
    ([5.0, 4.0] & [5, 4]).should == []
    str = "x"
    ([str] & [str.dup]).should == [str]
  end
  
  it "does return subclass instances for Array subclasses" do
    (MyArray[1, 2, 3] & []).class.should == Array
    (MyArray[1, 2, 3] & MyArray[1, 2, 3]).class.should == Array
    ([] & MyArray[1, 2, 3]).class.should == Array
  end

  it "does not call to_ary on array subclasses" do
    ([5, 6] & ToAryArray[1, 2, 5, 6]).should == [5, 6]
  end  
end

describe "Array#|" do
  it "returns an array of elements that appear in either array (union)" do
    ([] | []).should == []
    ([1, 2] | []).should == [1, 2]
    ([] | [1, 2]).should == [1, 2]
    ([ 1, 2, 3, 4 ] | [ 3, 4, 5 ]).should == [1, 2, 3, 4, 5]
  end
  
  it "creates an array with no duplicates" do
    ([ 1, 2, 3, 1, 4, 5 ] | [ 1, 3, 4, 5, 3, 6 ]).should == [1, 2, 3, 4, 5, 6]
  end
  
  it "creates an array with elements in order they are first encountered" do
    ([ 1, 2, 3, 1 ] | [ 1, 3, 4, 5 ]).should == [1, 2, 3, 4, 5]
  end

  it "calls to_ary on its argument" do
    obj = Object.new
    def obj.to_ary() [1, 2, 3] end
    ([0] | obj).should == ([0] | obj.to_ary)
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_ary], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_ary], :returning => [1, 2, 3])
    ([0] | obj).should == [0, 1, 2, 3]
  end

  # MRI doesn't actually call eql?() however. So you can't reimplement it.
  it "acts as if using eql?" do
    ([5.0, 4.0] | [5, 4]).should == [5.0, 4.0, 5, 4]
    str = "x"
    ([str] | [str.dup]).should == [str]
  end
  
  it "does not return subclass instances for Array subclasses" do
    (MyArray[1, 2, 3] | []).class.should == Array
    (MyArray[1, 2, 3] | MyArray[1, 2, 3]).class.should == Array
    ([] | MyArray[1, 2, 3]).class.should == Array
  end
  
  it "does not call to_ary on array subclasses" do
    ([1, 2] | ToAryArray[5, 6]).should == [1, 2, 5, 6]
  end
end

describe "Array#*" do
  it "is equivalent to self.join(str) when passed a string" do
    ([ 1, 2, 3 ] * ",").should == [1, 2, 3].join(",")
  end

# Rubinius cannot create recursive Arrays
failure :rubinius do
  it "handles recursive arrays like #join" do
    x = []
    x << x
    (x * ":").should == x.join(":")

    x = []
    y = []
    y << 9 << x << 8 << y << 7
    x << 1 << x << 2 << y << 3
    (x * ":").should == x.join(":")
  end
end  

  it "calls to_str on its argument" do
    obj = Object.new
    def obj.to_str() "x" end
    ([1, 2, 3] * obj).should == [1, 2, 3].join(obj)
    
# There is something weird with these
#    obj = Object.new
#    obj.should_receive(:respond_to?, :count => 2, :with => [:to_str], :returning => true, :count => :any)
#    obj.should_receive(:method_missing, :count => 2, :with => [:to_str], :returning => "x")
#    ([1, 2, 3] * obj).should == [1, 2, 3].join(obj)
  end
  
  it "concatenates n copies of the array when passed an integer" do
    ([ 1, 2, 3 ] * 0).should == []
    ([ 1, 2, 3 ] * 1).should == [1, 2, 3]
    ([ 1, 2, 3 ] * 3).should == [1, 2, 3, 1, 2, 3, 1, 2, 3]
    ([] * 10).should == []
  end
  
  it "raises ArgumentError when passed a negative integer" do
    should_raise(ArgumentError) { [ 1, 2, 3 ] * -1 }
    should_raise(ArgumentError) { [] * -1 }
  end
  
  it "calls to_int on its argument" do
    obj = Object.new
    def obj.to_int() 2 end

    ([1, 2, 3] * obj).should == [1, 2, 3] * obj.to_int
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_int], :returning => 2)
    ([1, 2, 3] * obj).should == [1, 2, 3] * 2
  end

  it "calls to_str on its argument before to_int" do
    obj = Object.new
    def obj.to_int() 2 end
    def obj.to_str() "x" end
    ([1, 2, 3] * obj).should == [1, 2, 3] * obj.to_str
  end

  it "returns subclass instance with Array subclasses" do
    (MyArray[1, 2, 3] * 0).class.should == MyArray
    (MyArray[1, 2, 3] * 1).class.should == MyArray
    (MyArray[1, 2, 3] * 2).class.should == MyArray
  end

  it "raises TypeError if the argument can neither be converted to a string nor an integer" do
    should_raise(TypeError) { [1, 2] * Object.new }
  end  
end

describe "Array#+" do
  it "concatenates two arrays" do
    ([ 1, 2, 3 ] + [ 3, 4, 5 ]).should == [1, 2, 3, 3, 4, 5]
    ([ 1, 2, 3 ] + []).should == [1, 2, 3]
    ([] + [ 1, 2, 3 ]).should == [1, 2, 3]
    ([] + []).should == []
  end

  it "calls to_ary on its argument" do
    obj = Object.new
    def obj.to_ary() ["x", "y"] end
    ([1, 2, 3] + obj).should == [1, 2, 3] + obj.to_ary
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_ary], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_ary], :returning => [:x])
    ([1, 2, 3] + obj).should == [1, 2, 3, :x]
  end

  it "does return subclass instances with Array subclasses" do
    (MyArray[1, 2, 3] + []).class.should == Array
    (MyArray[1, 2, 3] + MyArray[]).class.should == Array
    ([1, 2, 3] + MyArray[]).class.should == Array
  end

  it "does not call to_ary on array subclasses" do
    ([5, 6] + ToAryArray[1, 2]).should == [5, 6, 1, 2]
  end
end

describe "Array#-" do
  it "creates an array minus any items from other array" do
    ([] - [ 1, 2, 4 ]).should == []
    ([1, 2, 4] - []).should == [1, 2, 4]
    ([ 1, 2, 3, 4, 5 ] - [ 1, 2, 4 ]).should == [3, 5]
  end

  it "removes multiple items on the lhs equal to one on the rhs" do
    ([1, 1, 2, 2, 3, 3, 4, 5] - [1, 2, 4]).should == [3, 3, 5]
  end

  it "calls to_ary on its argument" do
    obj = Object.new
    def obj.to_ary() [2, 3, 3, 4] end
    ([1, 1, 2, 2, 3, 4] - obj).should == [1, 1]
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_ary], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_ary], :returning => [2, 3, 4])
    ([1, 1, 2, 2, 3, 4] - obj).should == [1, 1]
  end

  it "does not return subclass instance for Array subclasses" do
    (MyArray[1, 2, 3] - []).class.should == Array
    (MyArray[1, 2, 3] - MyArray[]).class.should == Array
    ([1, 2, 3] - MyArray[]).class.should == Array
  end

  it "does not call to_ary on array subclasses" do
    ([5, 6, 7] - ToAryArray[7]).should == [5, 6]
  end
end

describe "Array#<=>" do
  it "calls <=> left to right and return first non-0 result" do
    [-1, +1, nil, "foobar"].each do |result|
      lhs = Array.new(3) { Object.new }
      rhs = Array.new(3) { Object.new }
    
      lhs[0].should_receive(:<=>, :with => [rhs[0]], :returning => 0)
      lhs[1].should_receive(:<=>, :with => [rhs[1]], :returning => result)
      lhs[2].should_not_receive(:<=>)

      (lhs <=> rhs).should == result
    end
  end
  
  it "returns 0 if the arrays are equal" do
    ([] <=> []).should == 0
    ([1, 2, 3, 4, 5, 6] <=> [1, 2, 3, 4, 5.0, 6.0]).should == 0
  end
  
  it "returns -1 if the array is shorter than the other array" do
    ([] <=> [1]).should == -1
    ([1, 1] <=> [1, 1, 1]).should == -1
  end

  it "returns +1 if the array is longer than the other array" do
    ([1] <=> []).should == +1
    ([1, 1, 1] <=> [1, 1]).should == +1
  end

  it "calls to_ary on its argument" do
    obj = Object.new
    def obj.to_ary() [1, 2, 3] end
    ([4, 5] <=> obj).should == ([4, 5] <=> obj.to_ary)
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_ary], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_ary], :returning => [4, 5])
    ([4, 5] <=> obj).should == 0
  end

  it "does not call to_ary on array subclasses" do
    ([5, 6, 7] <=> ToAryArray[5, 6, 7]).should == 0
  end
end

describe "Array#==" do
  it "returns true if each element is == to the corresponding element in the other array" do
    [].should == []
    ["a", "c", 7].should == ["a", "c", 7]

    obj = Object.new
    def obj.==(other) true end
    [obj].should == [5]
  end
  
  it "returns false if any element is not == to the corresponding element in the other the array" do
    ([ "a", "c" ] == [ "a", "c", 7 ]).should == false
  end
  
  it "returns false immediately when sizes of the arrays differ" do
    obj = Object.new
    obj.should_not_receive(:==)
    
    [].should_not == [obj]
    [obj].should_not == []
  end

  # Broken in MRI as well. See MRI bug #11585:
  # http://rubyforge.org/tracker/index.php?func=detail&aid=11585&group_id=426&atid=1698
  compliant(:r19) do
    it "calls to_ary on its argument" do
      obj = Object.new
      obj.should_receive(:to_ary, :returning => [1, 2, 3])
    
      [1, 2, 3].should == obj
    end
  end
  
  it "does not call to_ary on array subclasses" do
    [5, 6, 7].should == ToAryArray[5, 6, 7]
  end

  it "ignores array class differences" do
    MyArray[1, 2, 3].should == [1, 2, 3]
    MyArray[1, 2, 3].should == MyArray[1, 2, 3]
    [1, 2, 3].should == MyArray[1, 2, 3]
  end
end

describe "Array#assoc" do
  it "returns the first array whose 1st item is == obj or nil" do
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

  specify "calls == on argument" do
    key = Object.new
    items = [['not it', 1], ['it', 2], ['na', 3]]

    items.assoc(key).should == nil

    def key.==(other); other == 'it' ; end

    items.assoc(key).should == items[1]
  end

#  it "calls == on argument" do
#    key = Object.new
#    items = Array.new(3) { [Object.new, "foo"] }
#    items[0][0].should_receive(:==, :with => [key], :returning => false)
#    items[1][0].should_receive(:==, :with => [key], :returning => true)
#    items[2][0].should_not_receive(:==, :with => [key])
#    items.assoc(key).should == items[1]
#  end
  
  it "ignores any non-Array elements" do
    [1, 2, 3].assoc(2).should == nil
    s1 = [4]
    s2 = [5, 4, 3]
    a = ["foo", [], s1, s2, nil, []] 
    a.assoc(s1.first).should == s1
    a.assoc(s2.first).should == s2
  end
end

describe "Array#at" do
  it "returns the element at index" do
    a = [1, 2, 3, 4, 5, 6]
    a.at(0).should  == 1
    a.at(-2).should == 5
    a.at(10).should == nil
  end

  it "calls to_int on its argument" do
    a = ["a", "b", "c"]
    a.at(0.5).should == "a"
    
    obj = Object.new
    obj.should_receive(:to_int, :returning => 2)
    a.at(obj).should == "c"
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_int], :returning => 2)
    a.at(obj).should == "c"
  end
end

# Why is this here? -rue
#describe "Array#class" do
#  it "returns Array" do
#    [].class.should == Array
#  end
#end

describe "Array#clear" do
  it "removes all elements" do
    a = [1, 2, 3, 4]
    a.clear.equal?(a).should == true
    a.should == []
  end  

  it "returns self" do
    a = [1]
    oid = a.object_id
    a.clear.object_id.should == oid
  end

  it "leaves the Array empty" do
    a = [1]
    a.clear
    a.empty?.should == true
    a.size.should == 0
  end

  it "raises TypeError on a frozen array" do
    a = [1]
    a.freeze
    should_raise(TypeError) { a.clear }
  end
end

array_collect = shared "Array#collect" do |cmd|
  describe "Array##{cmd}" do
    it "returns a copy of array with each element replaced by the value returned by block" do
      a = ['a', 'b', 'c', 'd']
      b = a.send(cmd) { |i| i + '!' }
      b.should == ["a!", "b!", "c!", "d!"]
    end

    it "does not return subclass instance" do
      MyArray[1, 2, 3].send(cmd) { |x| x + 1 }.class.should == Array
    end
  end
end

describe "Array#collect" do
  it_behaves_like(array_collect, :collect)
end

describe "Array#map" do
  it_behaves_like(array_collect, :map)
end

array_collect_b = shared "Array#collect!" do |cmd|
  describe "Array##{cmd}" do
    it "replaces each element with the value returned by block" do
      a = [7, 9, 3, 5]
      a.send(cmd) { |i| i - 1 }.equal?(a).should == true
      a.should == [6, 8, 2, 4]
    end

    it "raises TypeError on a frozen array" do
      should_raise(TypeError) { frozen_array.send(cmd) {} }
    end
  end
end

describe "Array#collect!" do
  it_behaves_like(array_collect_b, :collect!)
end

describe "Array#map!" do
  it_behaves_like(array_collect_b, :map!)
end

describe "Array#compact" do
  it "returns a copy of array with all nil elements removed" do
    a = [1, nil, 2, nil, 4, nil]
    a.compact.should == [1, 2, 4]
  end

  it "returns subclass instance for Array subclasses" do
    MyArray[1, 2, 3, nil].compact.class.should == MyArray
  end
end

describe "Array#compact!" do
  it "removes all nil elements" do
    a = ['a', nil, 'b', nil, nil, false, 'c', nil]
    a.compact!.equal?(a).should == true
    a.should == ["a", "b", false, "c"]
  end
  
  it "returns nil if there are no nil elements to remove" do
    [1, 2, false, 3].compact!.should == nil
  end
  
  it "raises TypeError on a frozen array" do
    should_raise(TypeError) { frozen_array.compact! }
  end
end

describe "Array#concat" do
  it "appends the elements in the other array" do
    ary = [1, 2, 3]
    ary.concat([9, 10, 11]).equal?(ary).should == true
    ary.should == [1, 2, 3, 9, 10, 11]
    ary.concat([])
    ary.should == [1, 2, 3, 9, 10, 11]
  end
  
  it "does not loop endlessly when argument is self" do
    ary = ["x", "y"]
    ary.concat(ary).should == ["x", "y", "x", "y"]
  end  

  it "calls to_ary on its argument" do
    obj = Object.new
    def obj.to_ary() ["x", "y"] end
    [4, 5, 6].concat(obj).should == [4, 5, 6, "x", "y"]
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_ary], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_ary], :returning => [:x])
    [].concat(obj).should == [:x]
  end
  
  it "does not call to_ary on array subclasses" do
    [].concat(ToAryArray[5, 6, 7]).should == [5, 6, 7]
  end
  
compliant :r18 do
  it "raises a TypeError when Array is frozen and modification occurs" do
    should_raise(TypeError) { frozen_array.concat [1] }
  end

  it "does not raise a TypeError when Array is frozen but no modification occurs" do
    should_raise(TypeError) { frozen_array.concat [] }
  end
end

noncompliant :rubinius do
  it "raises TypeError on a frozen array" do
    should_raise(TypeError) { frozen_array.concat [] }
    should_raise(TypeError) { frozen_array.concat([1]) }
  end
end
end

describe "Array#delete" do
  it "removes elements that are #== to object" do
    x = Object.new
    def x.==(other) 3 == other end

    a = [1, 2, 3, x, 4, 3, 5, x]
    a.delete Object.new
    a.should == [1, 2, 3, x, 4, 3, 5, x]

    a.delete 3
    a.should == [1, 2, 4, 5]
  end

  it "returns object or nil if no elements match object" do
    [1, 2, 4, 5].delete(1).should == 1
    [1, 2, 4, 5].delete(3).should == nil
  end

  it "may be given a block that is executed if no element matches object" do
    [].delete('a') {:not_found}.should == :not_found
  end
  
compliant :r18 do
  it "raises TypeError on a frozen array if a modification would take place" do
    should_raise(TypeError) { frozen_array.delete(1) }
  end

  it "does not raise on a frozen array if a modification would not take place" do
    should_raise(TypeError) { frozen_array.delete(0) }
  end
end
  
noncompliant :rubinius do
  it "raises TypeError on a frozen array" do
    should_raise(TypeError) { frozen_array.delete(0) }
    should_raise(TypeError) { frozen_array.delete(1) }
  end
end
end

describe "Array#delete_at" do
  it "removes the element at the specified index" do
    a = [1, 2, 3, 4]
    a.delete_at(2)
    a.should == [1, 2, 4]
    a.delete_at(-1)
    a.should == [1, 2]
  end

  it "returns the removed element at the specified index" do
    a = [1, 2, 3, 4]
    a.delete_at(2).should == 3
    a.delete_at(-1).should == 4
  end
  
  it "returns nil if the index is out of range" do
    a = [1, 2]
    a.delete_at(3).should == nil
  end

  it "calls to_int on its argument" do
    obj = Object.new
    def obj.to_int() -1 end
    [1, 2].delete_at(obj).should == 2
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_int], :returning => -1)
    [1, 2].delete_at(obj).should == 2
  end
  
  it "accepts negative indices" do
    a = [1, 2]
    a.delete_at(-2).should == 1
  end
  
  it "raises TypeError on a frozen array" do
    should_raise(TypeError) { frozen_array.delete_at(0) }
  end
end

describe "Array#delete_if" do
  it "removes each element for which block returns true" do
    a = [ "a", "b", "c" ] 
    a.delete_if { |x| x >= "b" }.equal?(a).should == true
    a.should == ["a"]
  end
  
  it "raises TypeError on a frozen array" do
    should_raise(TypeError) { frozen_array.delete_if {} }
  end
end

describe "Array#dup" do
  it "returns an Array or a subclass instance" do
    [].dup.class.should == Array
    MyArray[1, 2].dup.class.should == MyArray
  end

  it "produces a shallow copy where the references are directly copied" do
    a = [Object.new, Object.new]
    b = a.dup
    b.first.object_id.should == a.first.object_id
    b.last.object_id.should == a.last.object_id
  end
end

describe "Array#each" do
  it "yields each element to the block" do
    a = []
    x = [1, 2, 3]
    x.each { |item| a << item }.equal?(x).should == true
    a.should == [1, 2, 3]
  end
end

describe "Array#each_index" do
  it "it passes the index of each element to the block" do
    a = []
    x = ['a', 'b', 'c', 'd']
    x.each_index { |i| a << i }.equal?(x).should == true
    a.should == [0, 1, 2, 3]
  end
end

describe "Array#empty?" do
  it "returns true if the array has no elements" do
    [].empty?.should == true
    [1].empty?.should == false
    [1, 2].empty?.should == false
  end
end

describe "Array#eql?" do
  it "returns true if other is the same array" do
    a, b = [1], [2]

    a.eql?(b).should == false
    a.eql?(a).should == true
  end
  
  it "returns true if other has the same length and elements" do
    a = [1, 2, 3, 4]
    b = [1, 2, 3, 4]
    c = [1, 2]
    d = ['a', 'b', 'c', 'd']

    a.eql?(b).should == true
    a.eql?(c).should == false
    a.eql?(d).should == false
    [].eql?([]).should == true
  end

  it "ignores array class differences" do
    MyArray[1, 2, 3].eql?([1, 2, 3]).should == true
    MyArray[1, 2, 3].eql?(MyArray[1, 2, 3]).should == true
    [1, 2, 3].eql?(MyArray[1, 2, 3]).should == true
  end

end

describe "Array#fetch" do
  it "returns the element at index" do
    [1, 2, 3].fetch(1).should == 2
    [nil].fetch(0).should == nil
  end

  it "counts negative indices backwards from end" do
    [1, 2, 3, 4].fetch(-1).should == 4
  end
  
  it "raises IndexError if there is no element at index" do
    should_raise(IndexError) { [1, 2, 3].fetch(3) }
    should_raise(IndexError) { [1, 2, 3].fetch(-4) }
    should_raise(IndexError) { [].fetch(0) }
  end
  
  it "returns default if there is no element at index if passed a default value" do
    [1, 2, 3].fetch(5, :not_found).should == :not_found
    [1, 2, 3].fetch(5, nil).should == nil
    [1, 2, 3].fetch(-4, :not_found).should == :not_found
    [nil].fetch(0, :not_found).should == nil
  end

  it "returns the value of block if there is no element at index if passed a block" do
    [1, 2, 3].fetch(9) { |i| i * i }.should == 81
    [1, 2, 3].fetch(-9) { |i| i * i }.should == 81
  end

  it "passes the original index argument object to the block, not the converted Integer" do
    o = Object.new
    def o.to_int(); 5; end

    [1, 2, 3].fetch(o) { |i| i }.equal?(o).should == true
  end

  it "gives precedence to the default block over the default argument" do
    [1, 2, 3].fetch(9, :foo) { |i| i * i }.should == 81
  end

  it "calls to_int on its argument" do
    x = Object.new
    def x.to_int() 0 end
    [1, 2, 3].fetch(x).should == 1
    
    x = Object.new
    x.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    x.should_receive(:method_missing, :with => [:to_int], :returning => 0)
    [1, 2, 3].fetch(x).should == 1
  end
end

describe "Array#fill" do
  it "replaces all elements in the array with object" do
    ary = ['a', 'b', 'c', 'duh']
    ary.fill(8).should == [8, 8, 8, 8]

    str = "x"
    ary.fill(str).should == [str, str, str, str]
    str << "y"
    ary.should == [str, str, str, str]
  end
  
  it "replaces length elements with object beginning with start index" do
    [1, 2, 3, 4, 5].fill('a', 2, 2).should == [1, 2, "a", "a", 5]
    [1, 2, 3, 4, 5].fill('a', 2, 5).should == [1, 2, "a", "a", "a", "a", "a"]
    [1, 2, 3, 4, 5].fill('a', 2, -2).should == [1, 2, 3, 4, 5]
    [1, 2, 3, 4, 5].fill('a', 2, 0).should == [1, 2, 3, 4, 5]

    [1, 2, 3, 4, 5].fill('a', -2, 2).should == [1, 2, 3, "a", "a"]    
    [1, 2, 3, 4, 5].fill('a', -2, 4).should == [1, 2, 3, "a", "a", "a", "a"]    
    [1, 2, 3, 4, 5].fill('a', -2, -2).should == [1, 2, 3, 4, 5]
    [1, 2, 3, 4, 5].fill('a', -2, 0).should == [1, 2, 3, 4, 5]
  end
  
  it "calls to_int on start and length" do
    x = Object.new
    def x.to_int() 2 end
    [1, 2, 3, 4, 5].fill('a', x, x).should == [1, 2, "a", "a", 5]
    
#    x = Object.new
#    x.should_receive(:respond_to?, :count => 2, :with => [:to_int], :returning => true)
#    x.should_receive(:method_missing, :count => 2, :with => [:to_int], :returning => 2)
#    [1, 2, 3, 4, 5].fill('a', x, x).should == [1, 2, "a", "a", 5]
  end

  it "starts at 0 if the negative index is before the start of the array" do
    [1, 2, 3, 4, 5].fill('a', -25, 3).should == ['a', 'a', 'a', 4, 5]
    [1, 2, 3, 4, 5].fill('a', -10, 10).should == %w|a a a a a a a a a a|
  end
  
  it "does not change the Array with an index and a count of < 1" do
    [1, 2, 3].fill('a', 1, -3).should == [1, 2, 3]
    [1, 2, 3].fill('a', 1, 0).should == [1, 2, 3]
  end

  it "replaces elements in range with object" do
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
  
  it "replaces all elements with the value of block (index given to block)" do
    [nil, nil, nil, nil].fill { |i| i * 2 }.should == [0, 2, 4, 6]
  end
  
  it "replaces length elements beginning with start with the value of block" do
    [true, false, true, false, true, false, true].fill(1, 4) { |i| i + 3 }.should == [true, 4, 5, 6, 7, false, true]
  end
  
  it "replaces all elements in range with the value of block" do
    [1, 1, 1, 1, 1, 1].fill(1..6) { |i| i + 1 }.should == [1, 2, 3, 4, 5, 6, 7]
  end

  it "increases the Array size when necessary" do
    a = [1, 2, 3]
    a.size.should == 3
    a.fill 'a', 0, 10
    a.size.should == 10 
  end

  it "raises ArgumentError if the wrong number of arguments is given" do
    should_raise(ArgumentError) { [].fill('a', 1, 2, true) }
    should_raise(ArgumentError) { [].fill('a', 1, true) {|i|} }
  end

  it "raises TypeError if the index is not numeric" do
    should_raise(TypeError) { [].fill 'a', true }
  end

  it "raises TypeError with range and length argument" do
    should_raise(TypeError) { [].fill('x', 0 .. 2, 5) }
  end

  it "ignores length if it is nil" do
    a = [1, 2, 3]
    a.fill('x', 1, nil)
    a.should == [1, 'x', 'x']
  end
  
  it "raises TypeError on a frozen array" do
    should_raise(TypeError) { frozen_array.fill('x') }
  end
end

describe "Array#first" do
  it "returns the first element" do
    %w{a b c}.first.should == 'a'
    [nil].first.should == nil
  end
  
  it "returns nil if self is empty" do
    [].first.should == nil
  end
  
  it "returns the first count elements" do
    [true, false, true, nil, false].first(2).should == [true, false]
  end
  
  it "returns an empty array when passed count on an empty array" do
    [].first(0).should == []
    [].first(1).should == []
  end
  
  it "returns an empty array when passed count == 0" do
    [1, 2, 3, 4, 5].first(0).should == []
  end
  
  it "returns an array containing the first element when passed count == 1" do
    [1, 2, 3, 4, 5].first(1).should == [1]
  end
  
  it "raises ArgumentError when count is negative" do
    should_raise(ArgumentError) { [1, 2].first(-1) }
  end
  
  it "returns the entire array when count > length" do
    [1, 2, 3, 4, 5, 9].first(10).should == [1, 2, 3, 4, 5, 9]
  end

  it "calls to_int on count" do
    obj = Object.new
    def obj.to_int() 2 end
    [1, 2, 3, 4, 5].first(obj).should == [1, 2]
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_int], :returning => 2)
    [1, 2, 3, 4, 5].first(obj).should == [1, 2]    
  end
  
  it "does not return subclass instance when passed count on Array subclasses" do
    MyArray[].first(0).class.should == Array
    MyArray[].first(2).class.should == Array
    MyArray[1, 2, 3].first(0).class.should == Array
    MyArray[1, 2, 3].first(1).class.should == Array
    MyArray[1, 2, 3].first(2).class.should == Array
  end
end

describe "Array#flatten" do
  it "returns a one-dimensional flattening recursively" do
    [[[1, [2, 3]],[2, 3, [4, [4, [5, 5]], [1, 2, 3]]], [4]], []].flatten.should == [1, 2, 3, 2, 3, 4, 4, 5, 5, 1, 2, 3, 4]
  end

  it "does not call flatten on elements" do
    obj = Object.new
    def obj.flatten() [1, 2] end
    [obj, obj].flatten.should == [obj, obj]

    obj = [5, 4]
    def obj.flatten() [1, 2] end
    [obj, obj].flatten.should == [5, 4, 5, 4]
  end
  
  it "raises ArgumentError on recursive arrays" do
    x = []
    x << x
    should_raise(ArgumentError) { x.flatten }
  
    x = []
    y = []
    x << y
    y << x
    should_raise(ArgumentError) { x.flatten }
  end
  
  it "returns subclass instance for Array subclasses" do
    MyArray[].flatten.class.should == MyArray
    MyArray[1, 2, 3].flatten.class.should == MyArray
    MyArray[1, [2], 3].flatten.class.should == MyArray
    [MyArray[1, 2, 3]].flatten.class.should == Array
  end
end  

describe "Array#flatten!" do
  it "modifies array to produce a one-dimensional flattening recursively" do
    a = [[[1, [2, 3]],[2, 3, [4, [4, [5, 5]], [1, 2, 3]]], [4]], []]
    a.flatten!.equal?(a).should == true
    a.should == [1, 2, 3, 2, 3, 4, 4, 5, 5, 1, 2, 3, 4]
  end

  it "returns nil if no modifications took place" do
    a = [1, 2, 3]
    a.flatten!.should == nil
  end

  it "raises ArgumentError on recursive arrays" do
    x = []
    x << x
    should_raise(ArgumentError) { x.flatten! }
  
    x = []
    y = []
    x << y
    y << x
    should_raise(ArgumentError) { x.flatten! }
  end

compliant :r18 do
  it "raises TypeError on frozen arrays when modification would take place" do
    nested_ary = [1, 2, []]
    nested_ary.freeze
    should_raise(TypeError) { nested_ary.flatten! }
  end

  it "does not raise on frozen arrays when no modification would take place" do
    frozen_array.flatten! # ok, already flat
  end
end

noncompliant :rubinius do
  it "always raises TypeError on frozen arrays" do
    should_raise(TypeError) { frozen_array.flatten! }
    nested_ary = [1, 2, []]
    nested_ary.freeze
    should_raise(TypeError) { nested_ary.flatten! }
  end
end
end

describe "Array#frozen?" do
  it "returns true if array is frozen" do
    a = [1, 2, 3]
    a.frozen?.should == false
    a.freeze
    a.frozen?.should == true
  end

  it "returns true if array is temporarily frozen while being sort!ed" do
    a = [1, 2, 3]
    a.sort! { |x,y| a.frozen?.should == true; x <=> y }
  end

  it "returns false for arrays being sorted (they aren't temporarily frozen)" do
    a = [1, 2, 3]
    a.sort { |x,y| a.frozen?.should == false; x <=> y }
  end
end

describe "Array#hash" do
  it "returns the same fixnum for arrays with the same content" do
    [].respond_to?(:hash).should == true
    
    [[], [1, 2, 3]].each do |ary|
      ary.hash.should == ary.dup.hash
      ary.hash.class.should == Fixnum
    end
  end
  
  it "calls to_int on result of calling hash on each element" do
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
    
    hash = Object.new
    hash.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    hash.should_receive(:method_missing, :with => [:to_int], :returning => 1)
    
    obj = Object.new
    obj.instance_variable_set(:@hash, hash)
    def obj.hash() @hash end
      
    [obj].hash == [0].hash
  end
  
  it "ignores array class differences" do
    MyArray[].hash.should == [].hash
    MyArray[1, 2].hash.should == [1, 2].hash
  end

  it "returns same hash code for arrays with the same content" do
    a = [1, 2, 3, 4]
    a.fill 'a', 0..3
    b = %w|a a a a|
    a.hash.should == b.hash
  end
  
  it "returns the same value if arrays are #eql?" do
    a = [1, 2, 3, 4]
    a.fill 'a', 0..3
    b = %w|a a a a|
    a.hash.should == b.hash
    a.eql?(b).should == true
  end

end

describe "Array#include?" do
  it "returns true if object is present, false otherwise" do
    [1, 2, "a", "b"].include?("c").should == false
    [1, 2, "a", "b"].include?("a").should == true
  end

  it "calls == on elements from left to right until success" do
    key = "x"
    ary = Array.new(3) { Object.new }
    ary[0].should_receive(:==, :with => [key], :returning => false)
    ary[1].should_receive(:==, :with => [key], :returning => true)
    ary[2].should_not_receive(:==)
    
    ary.include?(key).should == true
  end
end

describe "Array#index" do
  it "returns the index of the first element == to object" do
    x = Object.new
    def x.==(obj) 3 == obj; end

    [2, x, 3, 1, 3, 1].index(3).should == 1
  end

  it "returns 0 if first element == to object" do
    [2, 1, 3, 2, 5].index(2).should == 0
  end

  it "returns size-1 if only last element == to object" do
    [2, 1, 3, 1, 5].index(5).should == 4
  end

  it "returns nil if no element == to object" do
    [2, 1, 1, 1, 1].index(3).should == nil
  end
end

array_indexes = shared "Array#indexes" do |cmd|
  describe "Array##{cmd}" do
    it "returns elements at integer argument indexes (DEPRECATED)" do
      array = [1, 2, 3, 4, 5]

      x = Object.new
      def x.to_int() 4 end

      params = [1, 0, 5, -1, -8, 10, x]
      array.send(cmd, *params).should == array.values_at(*params)
    end
    
    it "calls to_int on arguments" do
      array = [1, 2, 3, 4, 5]

      x = Object.new
      def x.to_int() 4 end
      array.send(cmd, x).should == [5]

      x = Object.new
      x.should_receive(:respond_to?, :count => 2, :with => [:to_int], :returning => true)
      x.should_receive(:method_missing, :count => 2, :with => [:to_int], :returning => 1)
      array.send(cmd, x).should == array.send(cmd, x)
    end

    it "returns elements in range arguments as nested arrays (DEPRECATED)" do
      array = [1, 2, 3, 4, 5]
      params = [0..2, 1...3, 4..6]
      array.indexes(*params).should == [[1, 2, 3], [2, 3], [5]]
      array.indices(*params).should == [[1, 2, 3], [2, 3], [5]]
    end
  end
end

describe "Array#indexes" do
  it_behaves_like(array_indexes, :indexes)
end

describe "Array#indices" do
  it_behaves_like(array_indexes, :indices)
end

describe "Array#initialize" do
  it "is private" do
    [].private_methods.map { |m| m.to_s }.include?("initialize").should == true
  end

  it "does nothing when passed self" do
    ary = [1, 2, 3]
    ary.instance_eval { initialize(ary) }
    ary.should == [1, 2, 3]
  end
  
  it "sets the array to size objects when passed size, object" do
    [].instance_eval { initialize(2, [3]) }.should == [[3], [3]]
    [].instance_eval { initialize(1) }.should == [nil]
  end
  
  it "raises ArgumentError if size is negative" do
    should_raise(ArgumentError) { [].instance_eval { initialize(-1, :a) } }
    should_raise(ArgumentError) { [1, 2, 3].instance_eval { initialize(-1) } }
  end
  
  it "calls to_int on array size" do
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_int], :returning => 1)
    
    [1, 2].instance_eval { initialize(obj, :a) }
  end
  
  it "does not raise TypeError on a frozen array if it would not change the array" do
    frozen_array.instance_eval { initialize() }.should == frozen_array
  end

  it "raises TypeError on frozen arrays" do
    should_raise(TypeError) { frozen_array.instance_eval { initialize(1) } }
    should_raise(TypeError) { frozen_array.instance_eval { initialize([1, 2, 3]) } }
  end
end

array_replace = shared "Array#replace" do |cmd|
  describe "Array##{cmd}" do
    it "replaces the elements with elements from other array" do
      a = [1, 2, 3, 4, 5]
      b = ['a', 'b', 'c']
      a.send(cmd, b).equal?(a).should == true
      a.should == b
      a.equal?(b).should == false

      a.send(cmd, [4] * 10)
      a.should == [4] * 10

      a.send(cmd, [])
      a.should == []
    end

    it "calls to_ary on its argument" do
      obj = Object.new
      def obj.to_ary() [1, 2, 3] end

      ary = []
      ary.send(cmd, obj)
      ary.should == [1, 2, 3]

      obj = Object.new
      obj.should_receive(:respond_to?, :with => [:to_ary], :returning => true)
      obj.should_receive(:method_missing, :with => [:to_ary], :returning => [])

      ary.send(cmd, obj)
      ary.should == []
    end

    it "does not call to_ary on array subclasses" do
      ary = []
      ary.send(cmd, ToAryArray[5, 6, 7])
      ary.should == [5, 6, 7]
    end

    it "raises TypeError on a frozen array" do
      should_raise(TypeError) { frozen_array.send(cmd, frozen_array) }
    end
  end
end

describe "Array#initialize_copy" do
  it "is private" do
    [].private_methods.map { |m| m.to_s }.include?("initialize_copy").should == true
  end
  
  it_behaves_like(array_replace, :initialize_copy)
end

describe "Array#insert" do
  it "inserts objects before the element at index for non-negative index" do
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

  it "appends objects to the end of the array for index == -1" do
    [1, 3, 3].insert(-1, 2, 'x', 0.5).should == [1, 3, 3, 2, 'x', 0.5]
  end

  it "inserts objects after the element at index with negative index" do
    ary = []
    ary.insert(-1, 3).should == [3]
    ary.insert(-2, 2).should == [2, 3]
    ary.insert(-3, 1).should == [1, 2, 3]
    ary.insert(-2, -3).should == [1, 2, -3, 3]
    ary.insert(-1, []).should == [1, 2, -3, 3, []]
    ary.insert(-2, 'x', 'y').should == [1, 2, -3, 3, 'x', 'y', []]
    ary = [1, 2, 3]
  end

  it "pads with nils if the index to be inserted to is past the end" do
    [].insert(5, 5).should == [nil, nil, nil, nil, nil, 5]
  end

  it "can insert before the first element with a negative index" do
    [1, 2, 3].insert(-4, -3).should == [-3, 1, 2, 3]
  end  
  
  it "raises IndexError if the negative index is out of bounds" do
    should_raise(IndexError) { [].insert(-2, 1) }
    should_raise(IndexError) { [1].insert(-3, 2) }
  end

  it "does nothing of no object is passed" do
    [].insert(0).should == []
    [].insert(-1).should == []
    [].insert(10).should == []
    [].insert(-2).should == []
  end

  it "calls to_int on position argument" do
    obj = Object.new
    def obj.to_int() 2 end
    [].insert(obj, 'x').should == [nil, nil, 'x']
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_int], :returning => 2)
    [].insert(obj, 'x').should == [nil, nil, 'x']
  end
  
  it "raises TypeError on frozen arrays" do
    frozen_array.insert(0) # ok
    should_raise(TypeError) { frozen_array.insert(0, 'x') }
  end
end

describe "Array#inspect" do
  # FIX: compatibility? --rue
  it "returns a string equivalent to evaluated source code representation" do
    [1, 2, 3].inspect.should == '[1, 2, 3]'
    [1, 2, 3 + 4].inspect.should == '[1, 2, 7]'
    [1, ['a', nil, [], 5.0], [[]], -4].inspect.should == '[1, ["a", nil, [], 5.0], [[]], -4]'
  end

  it "calls inspect on its arguments" do
    items = Array.new(3) do |i|
      obj = Object.new
      obj.should_receive(:inspect, :returning => "items[#{i}]")
      obj
    end
    
    items.inspect.should == '[items[0], items[1], items[2]]'
  end
  
  it "handles recursive arrays" do
    x = [1, 2]
    x << x << 4
    x.inspect.should == '[1, 2, [...], 4]'

    x = [1, 2]
    y = [3, 4]
    x << y
    y << x
    x.inspect.should == '[1, 2, [3, 4, [...]]]'
    y.inspect.should == '[3, 4, [1, 2, [...]]]'
  end
end

describe "Array#join" do
  it "returns a string formed by concatenating each element.to_s separated by separator without trailing separator" do
    obj = Object.new
    def obj.to_s() 'foo' end
    [1, 2, 3, 4, obj].join(' | ').should == '1 | 2 | 3 | 4 | foo'

# undef is not implemented -rue
#    obj = Object.new
#    class << obj; undef :to_s; end
#    obj.should_receive(:method_missing, :with => [:to_s], :returning => "o")
#    [1, obj].join(":").should == "1:o"
  end
  
  it "uses the same separator with nested arrays" do
    [1, [2, [3, 4], 5], 6].join(":").should == "1:2:3:4:5:6"
    [1, [2, MyArray[3, 4], 5], 6].join(":").should == "1:2:3:4:5:6"
  end

  it "uses $, as the default separator (which defaults to empty)" do
    [1, 2, 3].join.should == '123'
    old, $, = $,, '-'
    [1, 2, 3].join.should == '1-2-3'
    $, = old
  end
  
  it "calls to_str on its separator argument" do
    obj = Object.new
    def obj.to_str() '::' end    
    [1, 2, 3, 4].join(obj).should == '1::2::3::4'
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_str], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_str], :returning => ".")
    [1, 2].join(obj).should == "1.2"
  end

failure :rubinius do
  it "handles recursive arrays" do
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
end
end

describe "Array#last" do
  it "returns the last element" do
    [1, 1, 1, 1, 2].last.should == 2
  end
  
  it "returns nil if self is empty" do
    [].last.should == nil
  end
  
  it "returns the last count elements" do
    [1, 2, 3, 4, 5, 9].last(3).should == [4, 5, 9]
  end

  it "returns an empty array when passed a count on an empty array" do
    [].last(0).should == []
    [].last(1).should == []
  end
  
  it "returns an empty array when count == 0" do
    [1, 2, 3, 4, 5].last(0).should == []
  end
  
  it "raises ArgumentError when count is negative" do
    should_raise(ArgumentError) { [1, 2].last(-1) }
  end
  
  it "returns the entire array when count > length" do
    [1, 2, 3, 4, 5, 9].last(10).should == [1, 2, 3, 4, 5, 9]
  end

  it "does not return subclass instance on Array subclasses" do
    MyArray[].last(0).class.should == Array
    MyArray[].last(2).class.should == Array
    MyArray[1, 2, 3].last(0).class.should == Array
    MyArray[1, 2, 3].last(1).class.should == Array
    MyArray[1, 2, 3].last(2).class.should == Array
  end
end

array_length = shared "Array#length" do |cmd|
  describe "Array##{cmd}" do
    it "returns the number of elements" do
      [].send(cmd).should == 0
      [1, 2, 3].send(cmd).should == 3
    end
  end
end

describe "Array#length" do
  it_behaves_like(array_length, :length)
end

describe "Array#size" do
  it_behaves_like(array_length, :size)
end

describe "Array#nitems" do
  it "returns the number of non-nil elements" do
    [nil].nitems.should == 0
    [].nitems.should == 0    
    [1, 2, 3, nil].nitems.should == 3
    [1, 2, 3].nitems.should == 3
    [1, nil, 2, 3, nil, nil, 4].nitems.should == 4
    [1, nil, 2, false, 3, nil, nil, 4].nitems.should == 5
  end
end

describe "Array#partition" do
  it "returns two arrays" do
    [].partition {}.should == [[], []]
  end
  
  it "returns in the left array values for which the block evaluates to true" do
    ary = [0, 1, 2, 3, 4, 5]

    ary.partition { |i| true }.should == [ary, []]
    ary.partition { |i| 5 }.should == [ary, []]
    ary.partition { |i| false }.should == [[], ary]
    ary.partition { |i| nil }.should == [[], ary]
    ary.partition { |i| i % 2 == 0 }.should == [[0, 2, 4], [1, 3, 5]]
    ary.partition { |i| i / 3 == 0 }.should == [[0, 1, 2], [3, 4, 5]]
  end
  
  it "does not return subclass instances on Array subclasses" do
    result = MyArray[1, 2, 3].partition { |x| x % 2 == 0 }
    result.class.should == Array
    result[0].class.should == Array
    result[1].class.should == Array
  end
end

describe "Array#pop" do
  it "removes and returns the last element of the array" do
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
  
  it "returns nil if there are no more elements" do
    [].pop.should == nil
  end
  
  it "raises TypeError on a frozen array" do
    should_raise(TypeError) { frozen_array.pop }
  end
end

describe "Array#push" do
  it "appends the arguments to the array" do
    a = [ "a", "b", "c" ]
    a.push("d", "e", "f").equal?(a).should == true
    a.push().should == ["a", "b", "c", "d", "e", "f"]
    a.push(5)
    a.should == ["a", "b", "c", "d", "e", "f", 5]
  end
  
  it "raises TypeError on a frozen array" do
    frozen_array.push() # ok
    should_raise(TypeError) { frozen_array.push(1) }
  end
end

describe "Array#rassoc" do
  it "returns the first contained array whose second element is == object" do
    ary = [[1, "a", 0.5], [2, "b"], [3, "b"], [4, "c"], [], [5], [6, "d"]]
    ary.rassoc("a").should == [1, "a", 0.5]
    ary.rassoc("b").should == [2, "b"]
    ary.rassoc("d").should == [6, "d"]
    ary.rassoc("z").should == nil
  end
  
  it "calls == on argument" do
    key = Object.new
    items = Array.new(3) { ["foo", Object.new, "bar"] }
    items[0][1].should_receive(:==, :with => [key], :returning => false)
    items[1][1].should_receive(:==, :with => [key], :returning => true)
    items[2][1].should_not_receive(:==, :with => [key])
    items.rassoc(key).should == items[1]
  end
end

describe "Array#reject" do
  it "returns a new array without elements for which block is true" do
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
  compliant(:ruby) do
    it "does not return subclass instance on Array subclasses" do
      MyArray[1, 2, 3].reject { |x| x % 2 == 0 }.class.should == MyArray
    end
  end
  
  compliant(:r19) do
    it "does not return subclass instance on Array subclasses" do
      MyArray[1, 2, 3].reject { |x| x % 2 == 0 }.class.should == Array
    end
  end
end

describe "Array#reject!" do
  it "removes elements for which block is true" do
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
  
  it "returns nil if no changes are made" do
    a = [1, 2, 3]
    
    a.reject! { |i| i < 0 }.should == nil
    
    a.reject! { true }
    a.reject! { true }.should == nil
  end
  
  it "raises TypeError on a frozen array" do
    should_raise(TypeError) { frozen_array.reject! {} }
  end
end

describe "Array#replace" do
  it_behaves_like(array_replace, :replace)
end

describe "Array#reverse" do
  it "returns a new array with the elements in reverse order" do
    [].reverse.should == []
    [1, 3, 5, 2].reverse.should == [2, 5, 3, 1]
  end

  it "returns subclass instance on Array subclasses" do
    MyArray[1, 2, 3].reverse.class.should == MyArray
  end
end

describe "Array#reverse!" do
  it "reverses the elements in place" do
    a = [6, 3, 4, 2, 1]
    a.reverse!.equal?(a).should == true
    a.should == [1, 2, 4, 3, 6]
    [].reverse!.should == []
  end
  
  it "raises TypeError on a frozen array" do
    should_raise(TypeError) { frozen_array.reverse! }
  end
end

describe "Array#reverse_each" do
  it "traverses array in reverse order and pass each element to block" do
    a = []
    [1, 3, 4, 6].reverse_each { |i| a << i }
    a.should == [6, 4, 3, 1]
  end

  it "does not fail when removing elements from block" do
    ary = [0, 0, 1, 1, 3, 2, 1, :x]
    
    count = 0
    
    ary.reverse_each do |item|
      count += 1
      
      if item == :x then
        ary.slice!(1..-1)
      end
    end
    
    count.should == 2
  end
end

describe "Array#rindex" do
  it "returns the first index backwards from the end where element == to object" do
    key = 3    
    ary = Array.new(3) { Object.new }
    ary[2].should_receive(:==, :with => [key], :returning => false)
    ary[1].should_receive(:==, :with => [key], :returning => true)
    ary[0].should_not_receive(:==)

    ary.rindex(key).should == 1
  end

  it "returns size-1 if last element == to object" do
    [2, 1, 3, 2, 5].rindex(5).should == 4
  end

  it "returns 0 if only first element == to object" do
    [2, 1, 3, 1, 5].rindex(2).should == 0
  end

  it "returns nil if no element == to object" do
    [1, 1, 3, 2, 1, 3].rindex(4).should == nil
  end

  it "does not fail when removing elements from block" do
    sentinel = Object.new
    ary = [0, 0, 1, 1, 3, 2, 1, sentinel]

    sentinel.instance_variable_set(:@ary, ary)
    def sentinel.==(o) @ary.slice!(1..-1); false; end
    
    ary.rindex(0).should == 0
  end
end

describe "Array#select" do
  it "returns a new array of elements for which block is true" do
    [1, 3, 4, 5, 6, 9].select { |i| i % ((i + 1) / 2) == 0}.should == [1, 4, 6]
  end

  it "does not return subclass instance on Array subclasses" do
    MyArray[1, 2, 3].select { true }.class.should == Array
  end
end

describe "Array#shift" do
  it "removes and returns the first element" do
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
  
  it "returns nil when the array is empty" do
    [].shift.should == nil
  end
  
  it "raises TypeError on a frozen array" do
    should_raise(TypeError) { frozen_array.shift }
  end
end

describe "Array#slice!" do
  it "removes and return the element at index" do
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
  
  it "removes and returns length elements beginning at start" do
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

  it "calls to_int on start and length arguments" do
    obj = Object.new
    def obj.to_int() 2 end
      
    a = [1, 2, 3, 4, 5]
    a.slice!(obj).should == 3
    a.should == [1, 2, 4, 5]
    a.slice!(obj, obj).should == [4, 5]
    a.should == [1, 2]
    a.slice!(0, obj).should == [1, 2]
    a.should == []
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_int], :returning => 2)
    a = [1, 2, 3, 4, 5]
    a.slice!(obj).should == 3    
  end

  it "removes and return elements in range" do
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
  
  it "calls to_int on range arguments" do
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
  
    should_raise(TypeError) { a.slice!("a" .. "b") }
    should_raise(TypeError) { a.slice!(from .. "b") }
    
    from = Object.new
    to = Object.new
    
    def from.<=>(o) 0 end
    def to.<=>(o) 0 end
      
    from.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    from.should_receive(:method_missing, :with => [:to_int], :returning => 1)
    to.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    to.should_receive(:method_missing, :with => [:to_int], :returning => -2)

    a = [1, 2, 3, 4, 5]
    a.slice!(from .. to).should == [2, 3, 4]
  end
  
  # TODO: MRI behaves inconsistently here. I'm trying to find out what it should
  # do at ruby-core right now. -- flgr
  # See http://groups.google.com/group/ruby-core-google/t/af70e3d0e9b82f39
  it "does (not?) expand array with indices out of bounds" do
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

  it "raises TypeError on a frozen array" do
    should_raise(TypeError) { frozen_array.slice!(0, 0) }
  end
end

class D 
  def <=>(obj) 
    return 4 <=> obj unless obj.class == D
    0
  end
end

describe "Array#sort" do
  it "returns a new array from sorting elements using <=> on the pivot" do
    d = D.new

    [1, 1, 5, -5, 2, -10, 14, 6].sort.should == [-10, -5, 1, 1, 2, 5, 6, 14]
    [d, 1].sort.should == [1, d]
  end

  it "raises an ArgumentError if the comparison cannot be completed" do
    d = D.new

    # Fails essentially because of 1.<=>(d) whereas d.<=>(1) would work
    should_raise(ArgumentError) { [1, d].sort.should == [1, d] }
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
  
  it "raises TypeError on a frozen array" do
    should_raise(TypeError) { frozen_array.sort! }
  end
end

describe "Array#to_a" do
  it "returns self" do
    a = [1, 2, 3]
    a.to_a.should == [1, 2, 3]
    a.equal?(a.to_a).should == true 
  end
  
  it "does not return subclass instance on Array subclasses" do
    e = MyArray.new
    e << 1
    e.to_a.class.should == Array
    e.to_a.should == [1]
  end
end

describe "Array#to_ary" do
  it "returns self" do
    a = [1, 2, 3]
    a.equal?(a.to_ary).should == true
    a = MyArray[1, 2, 3]
    a.equal?(a.to_ary).should == true
  end
end

describe "Array#to_s" do
  it "is equivalent to #join without a separator string" do
    old = $,
    a = [1, 2, 3, 4]
    a.to_s.should == a.join
    $, = '-'
    a.to_s.should == a.join
    $, = old
  end
end

describe "Array#transpose" do
  it "assumes an array of arrays and returns the result of transposing rows and columns" do
    [[1, 'a'], [2, 'b'], [3, 'c']].transpose.should == [[1, 2, 3], ["a", "b", "c"]]
    [[1, 2, 3], ["a", "b", "c"]].transpose.should == [[1, 'a'], [2, 'b'], [3, 'c']]
    [].transpose.should == []
    [[]].transpose.should == []
    [[], []].transpose.should == []
    [[0]].transpose.should == [[0]]
    [[0], [1]].transpose.should == [[0, 1]]
  end

  it "raises if the items aren't arrays and don't respond to to_ary" do
    g = Object.new
    def g.to_a() [1, 2] end
    h = Object.new
    def h.to_ary() [1, 2] end

    should_raise(TypeError) { [g, [:a, :b]].transpose } 
    [h, [:a, :b]].transpose.should == [[1, :a], [2, :b]]
    
    h = Object.new
    h.should_receive(:respond_to?, :with => [:to_ary], :returning => true)
    h.should_receive(:method_missing, :with => [:to_ary], :returning => [1, 2])
    [h, [:a, :b]].transpose.should == [[1, :a], [2, :b]]    
  end
  
  it "does not call to_ary on array subclass elements" do
    ary = [ToAryArray[1, 2], ToAryArray[4, 6]]
    ary.transpose.should == [[1, 4], [2, 6]]
  end

  it "raises IndexError if the arrays are not of the same length" do
    should_raise(IndexError) { [[1, 2], [:a]].transpose }
  end
  
  it "does not return subclass instance on Array subclasses" do
    result = MyArray[MyArray[1, 2, 3], MyArray[4, 5, 6]].transpose
    result.class.should == Array
    result[0].class.should == Array
    result[1].class.should == Array
  end
  
end

describe "Array#uniq" do
  it "returns an array with no duplicates" do
    ["a", "a", "b", "b", "c"].uniq.should == ["a", "b", "c"]
  end

  it "uses eql? semantics" do
    [1.0, 1].uniq.should == [1.0, 1]
  end

  it "compares elements first with hash" do
    # Can't use should_receive because it uses hash internally
    x = Object.new
    def x.hash() freeze; 0 end
    y = Object.new
    def y.hash() freeze; 0 end
    
    [x, y].uniq
    x.frozen?.should == true
    y.frozen?.should == true
  end
  
  it "does not compare elements with different hash codes via eql?" do
    # Can't use should_receive because it uses hash and eql? internally
    x = Object.new
    def x.eql?(o) raise("Shouldn't receive eql?") end
    y = Object.new
    def y.eql?(o) raise("Shouldn't receive eql?") end

    def x.hash() freeze; 0 end
    def y.hash() freeze; 1 end

    [x, y].uniq.should == [x, y]
    x.frozen?.should == true
    y.frozen?.should == true
  end
  
  it "compares elements with matching hash codes with #eql?" do
    # Can't use should_receive because it uses hash and eql? internally
    a = Array.new(2) do 
      obj = Object.new

      def obj.hash()
        # It's undefined whether the impl does a[0].eql?(a[1]) or
        # a[1].eql?(a[0]) so we taint both.
        def self.eql?(o) taint; o.taint; false; end
        return 0
      end

      obj
    end

    a.uniq.should == a
    a[0].tainted?.should == true
    a[1].tainted?.should == true

    a = Array.new(2) do 
      obj = Object.new

      def obj.hash()
        # It's undefined whether the impl does a[0].eql?(a[1]) or
        # a[1].eql?(a[0]) so we taint both.
        def self.eql?(o) taint; o.taint; true; end
        return 0
      end

      obj
    end

    a.uniq.size == 1
    a[0].tainted?.should == true
    a[1].tainted?.should == true
  end
  
  it "returns subclass instance on Array subclasses" do
    MyArray[1, 2, 3].uniq.class.should == MyArray
  end
end

describe "Array#uniq!" do
  it "modifies the array in place" do
    a = [ "a", "a", "b", "b", "c" ]
    a.uniq!
    a.should == ["a", "b", "c"]
  end
  
  it "returns self" do
    a = [ "a", "a", "b", "b", "c" ]
    a.equal?(a.uniq!).should == true
  end
  
  it "returns nil if no changes are made to the array" do
    [ "a", "b", "c" ].uniq!.should == nil
  end
  
  it "raises TypeError on a frozen array" do
    frozen_array.uniq! # ok, already uniq
    dup_ary = [1, 1, 2]
    dup_ary.freeze
    should_raise(TypeError) { dup_ary.uniq! }
  end
end

describe "Array#unshift" do
  it "prepends object to the original array" do
    a = [1, 2, 3]
    a.unshift("a").equal?(a).should == true
    a.should == ['a', 1, 2, 3]
    a.unshift().equal?(a).should == true
    a.should == ['a', 1, 2, 3]
    a.unshift(5, 4, 3)
    a.should == [5, 4, 3, 'a', 1, 2, 3]
  end

  it "raises TypeError on a frozen array" do
    frozen_array.unshift() # ok
    should_raise(TypeError) { frozen_array.unshift(1) }
  end  
end

describe "Array#values_at" do
  it "returns an array of elements at the indexes when passed indexes" do
    [1, 2, 3, 4, 5].values_at().should == []
    [1, 2, 3, 4, 5].values_at(1, 0, 5, -1, -8, 10).should == [2, 1, nil, 5, nil, nil]
  end

  it "calls to_int on its indices" do
    obj = Object.new
    def obj.to_int() 1 end
    [1, 2].values_at(obj, obj, obj).should == [2, 2, 2]
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_int], :returning => 1)
    [1, 2].values_at(obj).should == [2]
  end
  
  it "returns an array of elements in the ranges when passes ranges" do
    [1, 2, 3, 4, 5].values_at(0..2, 1...3, 4..6).should == [1, 2, 3, 2, 3, 5, nil]
    [1, 2, 3, 4, 5].values_at(6..4).should == []
  end

  it "calls to_int on arguments of ranges when passes ranges" do
    from = Object.new
    to = Object.new

    # So we can construct a range out of them...
    def from.<=>(o) 0 end
    def to.<=>(o) 0 end

    def from.to_int() 1 end
    def to.to_int() -2 end
      
    ary = [1, 2, 3, 4, 5]
    ary.values_at(from .. to, from ... to, to .. from).should == [2, 3, 4, 2, 3]

    from = Object.new
    to = Object.new
    
    def from.<=>(o) 0 end
    def to.<=>(o) 0 end
      
    from.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    from.should_receive(:method_missing, :with => [:to_int], :returning => 1)
    to.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    to.should_receive(:method_missing, :with => [:to_int], :returning => -2)
    ary.values_at(from .. to).should == [2, 3, 4]
  end
  
  it "does not return subclass instance on Array subclasses" do
    MyArray[1, 2, 3].values_at(0, 1..2, 1).class.should == Array
  end
  
end

describe "Array#zip" do
  it "returns an array of arrays containing corresponding elements of each array" do
    [1, 2, 3, 4].zip(["a", "b", "c", "d", "e"]).should ==
      [[1, "a"], [2, "b"], [3, "c"], [4, "d"]]
  end
  
  it "fills in missing values with nil" do
    [1, 2, 3, 4, 5].zip(["a", "b", "c", "d"]).should ==
      [[1, "a"], [2, "b"], [3, "c"], [4, "d"], [5, nil]]
  end
  
  # MRI 1.8.6 uses to_ary, but it's been fixed in 1.9
  compliant(:ruby) do
    it "calls to_ary on its arguments" do
      obj = Object.new
      obj.should_receive(:respond_to?, :with => [:to_ary], :returning => true)
      obj.should_receive(:method_missing, :with => [:to_ary], :returning => [3, 4])
    
      [1, 2].zip(obj).should == [[1, 3], [2, 4]]
    end
  end
  
  compliant(:r19) do
    it "calls to_a on its arguments" do
      [1, 2, 3].zip("f" .. "z", 1 .. 9).should ==
        [[1, "f", 1], [2, "g", 2], [3, "h", 3]]
      
      obj = Object.new
      obj.should_receive(:respond_to?, :with => [:to_a], :returning => true)
      obj.should_receive(:method_missing, :with => [:to_a], :returning => [3, 4])
    
      [1, 2].zip(obj).should == [[1, 3], [2, 4]]
    end
  end

  it "calls block if supplied" do
    values = []
    [1, 2, 3, 4].zip(["a", "b", "c", "d", "e"]) { |value|
      values << value
    }.should == nil
    
    values.should == [[1, "a"], [2, "b"], [3, "c"], [4, "d"]]
  end
  
  it "does not return subclass instance on Array subclasses" do
    MyArray[1, 2, 3].zip(["a", "b"]).class.should == Array
  end
end

array_slice = shared "Array#[]" do |cmd|
  describe "Array##{cmd}" do
    it "returns the element at index with [index]" do
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
  
    it "returns the element at index from the end of the array with [-index]" do
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
  
    it "return count elements starting from index with [index, count]" do
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
  
    it "returns count elements starting at index from the end of array with [-index, count]" do
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
  
    it "returns the first count elements with [0, count]" do
      [ "a", "b", "c", "d", "e" ].send(cmd, 0, 3).should == ["a", "b", "c"]
    end

    it "calls to_int on index and count arguments with [index, count]" do
      obj = Object.new
      def obj.to_int() 2 end
      
      a = [1, 2, 3, 4]
      a.send(cmd, obj).should == 3
      a.send(cmd, obj, 1).should == [3]
      a.send(cmd, obj, obj).should == [3, 4]
      a.send(cmd, 0, obj).should == [1, 2]

#  Not sure of the purpose of this -rue  
#      obj = Object.new
#      obj.should_receive(:respond_to?, :with => [:to_int], :returning => true, :count => :any)
#      obj.should_receive(:method_missing, :with => [:to_int], :returning => 2)
#      a.send(cmd, obj).should == 3
    end
  
    it "returns the elements specified by Range indexes with [m..n]" do
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
  
    it "returns elements specified by Range indexes except the element at index n with [m...n]" do
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
  
    it "returns elements that exist if range start is in the array but range end is not with [m..n]" do
      [ "a", "b", "c", "d", "e" ].send(cmd, 4..7).should == ["e"]
    end
  
    it "accepts Range instances having a negative m and both signs for n with [m..n] and [m...n]" do 
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

    it "calls to_int on Range arguments with [m..n] and [m...n]" do
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
    
      from = Object.new
      to = Object.new

      def from.<=>(o) 0 end
      def to.<=>(o) 0 end

      from.should_receive(:respond_to?, :with => [:to_int], :returning => true)
      from.should_receive(:method_missing, :with => [:to_int], :returning => 1)
      to.should_receive(:respond_to?, :with => [:to_int], :returning => true)
      to.should_receive(:method_missing, :with => [:to_int], :returning => -2)
      a.send(cmd, from..to).should == [2, 3]
    end
  
    it "returns the same elements as [m..n] and [m...n] with Range subclasses" do
      a = [1, 2, 3, 4]
      range_incl = MyRange.new(1, 2)
      range_excl = MyRange.new(-3, -1, true)

      a[range_incl].should == [2, 3]
      a[range_excl].should == [2, 3]
    end

    it "returns nil for a requested index not in the array with [index]" do
      [ "a", "b", "c", "d", "e" ].send(cmd, 5).should == nil
    end
  
    it "returns [] if the index is valid but length is zero with [index, length]" do
      [ "a", "b", "c", "d", "e" ].send(cmd, 0, 0).should == []
      [ "a", "b", "c", "d", "e" ].send(cmd, 2, 0).should == []
    end

    it "returns nil if length is zero but index is invalid with [index, length]" do
      [ "a", "b", "c", "d", "e" ].send(cmd, 100, 0).should == nil
      [ "a", "b", "c", "d", "e" ].send(cmd, -50, 0).should == nil
    end

    # This is by design. It is in the official documentation.
    it "returns [] if index == array.size with [index, length]" do
      %w|a b c d e|.send(cmd, 5, 2).should == []
    end

    it "returns nil if index > array.size with [index, length]" do
      %w|a b c d e|.send(cmd, 6, 2).should == nil
    end

    it "returns nil if length is negative with [index, length]" do
      %w|a b c d e|.send(cmd, 3, -1).should == nil
      %w|a b c d e|.send(cmd, 2, -2).should == nil
      %w|a b c d e|.send(cmd, 1, -100).should == nil
    end
  
    it "returns nil if no requested index is in the array with [m..n]" do
      [ "a", "b", "c", "d", "e" ].send(cmd, 6..10).should == nil
    end
  
    it "returns nil if range start is not in the array with [m..n]" do
      [ "a", "b", "c", "d", "e" ].send(cmd, -10..2).should == nil
      [ "a", "b", "c", "d", "e" ].send(cmd, 10..12).should == nil
    end
  
    it "returns an empty array when m == n with [m...n]" do
      [1, 2, 3, 4, 5].send(cmd, 1...1).should == []
    end
  
    it "returns an empty array with [0...0]" do
      [1, 2, 3, 4, 5].send(cmd, 0...0).should == []
    end
  
    it "returns a subarray where m, n negatives and m < n with [m..n]" do
      [ "a", "b", "c", "d", "e" ].send(cmd, -3..-2).should == ["c", "d"]
    end
  
    it "returns an array containing the first element with [0..0]" do
      [1, 2, 3, 4, 5].send(cmd, 0..0).should == [1]
    end
  
    it "returns the entire array with [0..-1]" do
      [1, 2, 3, 4, 5].send(cmd, 0..-1).should == [1, 2, 3, 4, 5]
    end
  
    it "returns all but the last element with [0...-1]" do
      [1, 2, 3, 4, 5].send(cmd, 0...-1).should == [1, 2, 3, 4]
    end

    it "returns [3] for [2..-1] out of [1, 2, 3] <Specifies bug found by brixen, Defiler, mae>" do
      [1,2,3].send(cmd, 2..-1).should == [3]
    end
  
    it "returns an empty array when m > n and m, n are positive with [m..n]" do
      [1, 2, 3, 4, 5].send(cmd, 3..2).should == []
    end
  
    it "returns an empty array when m > n and m, n are negative with [m..n]" do
      [1, 2, 3, 4, 5].send(cmd, -2..-3).should == []
    end

    it "does not expand array when the indices are outside of the array bounds" do
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
  
    it "returns a subclass instance when called on a subclass of Array" do
      ary = MyArray[1, 2, 3]
      ary.send(cmd, 0, 0).class.should == MyArray
      ary.send(cmd, 0, 2).class.should == MyArray
      ary.send(cmd, 0..10).class.should == MyArray
    end
  end
end

describe "Array#[]" do
  it_behaves_like(array_slice, :[])
end

describe "Array#slice" do
  it_behaves_like(array_slice, :slice)
end

describe "Array#[]=" do
  it "sets the value of the element at index" do
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
  
  it "removes the section defined by start, length when set to nil" do
    a = ['a', 'b', 'c', 'd', 'e']
    a[1, 3] = nil
    a.should == ["a", "e"]
  end
  
  it "sets the section defined by start, length to other" do
    a = [1, 2, 3, 4, 5, 6]
    a[0, 1] = 2
    a[3, 2] = ['a', 'b', 'c', 'd']
    a.should == [2, 2, 3, "a", "b", "c", "d", 6]
  end
  
  it "removes the section defined by range when set to nil" do
    a = [1, 2, 3, 4, 5]
    a[0..1] = nil
    a.should == [3, 4, 5]
  end
  
  it "sets the section defined by range to other" do
    a = [6, 5, 4, 3, 2, 1]
    a[1...2] = 9
    a[3..6] = [6, 6, 6]
    a.should == [6, 9, 4, 6, 6, 6]
  end

  it "calls to_int on its start and length arguments" do
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

  it "should set elements in the range arguments when passed ranges" do
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

  it "calls to_int on range arguments" do
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

    a[to .. from] = ["x"]
    a.should == [1, "a", "b", "x", "c", 4]
    should_raise(TypeError) { a["a" .. "b"] = [] }
    should_raise(TypeError) { a[from .. "b"] = [] }
    
    from = Object.new
    to = Object.new
    
    def from.<=>(o) 0 end
    def to.<=>(o) 0 end
      
    from.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    from.should_receive(:method_missing, :with => [:to_int], :returning => 1)
    to.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    to.should_receive(:method_missing, :with => [:to_int], :returning => -2)

    a = [1, 2, 3, 4]
    a[from .. to] = ["a", "b", "c"]
  end
  
  it "raises IndexError when passed indexes out of bounds" do
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
  
  it "calls to_ary on its rhs argument for multi-element sets" do
    obj = Object.new
    def obj.to_ary() [1, 2, 3] end
    ary = [1, 2]
    ary[0, 0] = obj
    ary.should == [1, 2, 3, 1, 2]
    ary[1, 10] = obj
    ary.should == [1, 1, 2, 3]
  end
  
  it "does not call to_ary on rhs array subclasses for multi-element sets" do
    ary = []
    ary[0, 0] = ToAryArray[5, 6, 7]
    ary.should == [5, 6, 7]
  end

  it "raises TypeError on a frozen array" do
    should_raise(TypeError) { frozen_array[0, 0] = [] }
  end  
end

describe "Array#[]= with [index]" do
  it "returns value assigned if idx is inside array" do
    a = [1, 2, 3, 4, 5]
    (a[3] = 6).should == 6
  end
  
  it "returns value assigned if idx is right beyond right array boundary" do
    a = [1, 2, 3, 4, 5]
    (a[5] = 6).should == 6
  end
  
  it "returns value assigned if idx far beyond right array boundary" do
    a = [1, 2, 3, 4, 5]
    (a[10] = 6).should == 6
  end

  it "sets the value of the element at index" do
      a = [1, 2, 3, 4]
      a[2] = 5
      a[-1] = 6
      a[5] = 3
      a.should == [1, 2, 5, 6, nil, 3]
    end

  it "sets the value of the element if it is right beyond the array boundary" do
    a = [1, 2, 3, 4]
    a[4] = 8
    a.should == [1, 2, 3, 4, 8]
  end
    
end

describe "Array#[]= with [index, count]" do
  it "returns non-array value if non-array value assigned" do
    a = [1, 2, 3, 4, 5]
    (a[2, 3] = 10).should == 10
  end

  it "returns array if array assigned" do
    a = [1, 2, 3, 4, 5]
    (a[2, 3] = [4, 5]).should == [4, 5]
  end

  it "removes the section defined by start, length when set to nil" do
      a = ['a', 'b', 'c', 'd', 'e']
      a[1, 3] = nil
      a.should == ["a", "e"]
    end
    
  it "removes the section when set to nil if negative index within bounds and cnt > 0" do
    a = ['a', 'b', 'c', 'd', 'e']
    a[-3, 2] = nil
    a.should == ["a", "b", "e"]
  end
  
  it "replaces the section defined by start, length to other" do
      a = [1, 2, 3, 4, 5, 6]
      a[0, 1] = 2
      a[3, 2] = ['a', 'b', 'c', 'd']
      a.should == [2, 2, 3, "a", "b", "c", "d", 6]
    end

  it "replaces the section to other if idx < 0 and cnt > 0" do
    a = [1, 2, 3, 4, 5, 6]
    a[-3, 2] = ["x", "y", "z"]
    a.should == [1, 2, 3, "x", "y", "z", 6]
  end

  it "replaces the section to other even if cnt spanning beyond the array boundary" do
    a = [1, 2, 3, 4, 5]
    a[-1, 3] = [7, 8]
    a.should == [1, 2, 3, 4, 7, 8]
  end

  it "pads the Array with nils if the span is past the end" do
    a = [1, 2, 3, 4, 5]
    a[10, 1] = [1]
    a.should == [1, 2, 3, 4, 5, nil, nil, nil, nil, nil, 1]

    b = [1, 2, 3, 4, 5]
    b[10, 0] = [1]
    a.should == [1, 2, 3, 4, 5, nil, nil, nil, nil, nil, 1]
  end

  it "inserts other section in place defined by idx" do
    a = [1, 2, 3, 4, 5]
    a[3, 0] = [7, 8]
    a.should == [1, 2, 3, 7, 8, 4, 5]
  end
  
  it "raises IndexError when passed start and negative length" do
    a = [1, 2, 3, 4]
    should_raise(IndexError) { a[-2, -1] = "" }
    should_raise(IndexError) { a[0, -1] = "" }
    should_raise(IndexError) { a[2, -1] = "" }
    should_raise(IndexError) { a[4, -1] = "" }
    should_raise(IndexError) { a[10, -1] = "" }
    should_raise(IndexError) { [1, 2, 3, 4,  5][2, -1] = [7, 8] }
  end

  it "sets elements when passed start, length" do
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
  
end

describe "Array#[]= with [m..n]" do
  it "returns non-array value if non-array value assigned" do
    a = [1, 2, 3, 4, 5]
    (a[2..4] = 10).should == 10
  end
  
  it "returns array if array assigned" do
    a = [1, 2, 3, 4, 5]
    (a[2..4] = [7, 8]).should == [7, 8]
  end
  
  it "removes the section defined by range when set to nil" do
      a = [1, 2, 3, 4, 5]
      a[0..1] = nil
      a.should == [3, 4, 5]
    end

  it "removes the section when set to nil if m and n < 0" do
    a = [1, 2, 3, 4, 5]
    a[-3..-2] = nil
    a.should == [1, 2, 5]
  end
    
  it "replaces the section defined by range" do
      a = [6, 5, 4, 3, 2, 1]
      a[1...2] = 9
      a[3..6] = [6, 6, 6]
      a.should == [6, 9, 4, 6, 6, 6]
    end

  it "replaces the section if m and n < 0" do
    a = [1, 2, 3, 4, 5]
    a[-3..-2] = [7, 8, 9]
    a.should == [1, 2, 7, 8, 9, 5]
  end

  it "replaces the section if m < 0 and n > 0" do
    a = [1, 2, 3, 4, 5]
    a[-4..3] = [8]
    a.should == [1, 8, 5]
  end

  it "inserts the other section at m if m > n" do
    a = [1, 2, 3, 4, 5]
    a[3..1] = [8]
    a.should == [1, 2, 3, 8, 4, 5]
  end
  
  it "accepts Range subclasses" do
    a = [1, 2, 3, 4]
    range_incl = MyRange.new(1, 2)
    range_excl = MyRange.new(-3, -1, true)

    a[range_incl] = ["a", "b"]
    a.should == [1, "a", "b", 4]
    a[range_excl] = ["A", "B"]
    a.should == [1, "A", "B", 4]
  end
end

#describe "Array#pack" do
#  #      Directive    Meaning
#  #      ---------------------------------------------------------------
#  #          @     |  Moves to absolute position
#  #          B     |  Bit string (descending bit order)
#  #          b     |  Bit string (ascending bit order)
#  #          D, d  |  Double-precision float, native format
#  #          E     |  Double-precision float, little-endian byte order
#  #          e     |  Single-precision float, little-endian byte order
#  #          F, f  |  Single-precision float, native format
#  #          G     |  Double-precision float, network (big-endian) byte order
#  #          g     |  Single-precision float, network (big-endian) byte order
#  #          H     |  Hex string (high nibble first)
#  #          h     |  Hex string (low nibble first)
#  #          I     |  Unsigned integer
#  #          i     |  Integer
#  #          L     |  Unsigned long
#  #          l     |  Long
#  #          N     |  Long, network (big-endian) byte order
#  #          n     |  Short, network (big-endian) byte-order
#  #          P     |  Pointer to a structure (fixed-length string)
#  #          p     |  Pointer to a null-terminated string
#  #          Q, q  |  64-bit number
#  #          S     |  Unsigned short
#  #          s     |  Short
#  #          U     |  UTF-8
#  #          V     |  Long, little-endian byte order
#  #          v     |  Short, little-endian byte order
#  #          w     |  BER-compressed integer\fnm
#
#  it "raises ArgumentError with ('%')" do
#    should_raise(ArgumentError) { [].pack("%") }
#  end
#
#  it "skips everything till the end of schema string with ('#')" do
#    ["abc", "def"].pack("A*#A10%").should == "abc"
#  end
#
#  it "skips everything till the end of schema line with ('#')" do
#    ["abc", "def"].pack("A*#A10%\nA*").should == "abcdef"
#  end
#
#  it "returns space padded string with ('A')" do
#    ['abcde'].pack('A7').should == 'abcde  '
#  end
#
#  it "cuts string if its size greater than directive count with ('A')" do
#    ['abcde'].pack('A3').should == 'abc'
#  end
#
#  it "consider count = 1 if count omited with ('A')" do
#    ['abcde'].pack('A').should == 'a'
#  end
#
#  it "returns empty string if count = 0 with ('A')" do
#    ['abcde'].pack('A0').should == ''
#  end
#
#  it "returns the whole argument string with star parameter with ('A')" do
#    ['abcdef'].pack('A*').should == 'abcdef'
#  end
#
#  it "raises TypeError if array item is not String with ('A')" do
#    should_raise(TypeError) { [123].pack('A5') }
#    should_raise(TypeError) { [:hello].pack('A5') }
#    should_raise(TypeError) { [Object.new].pack('A5') }
#  end
#
#  it "should work with multi-digit padding sizes with ('A')" do
#    ['abcdef'].pack('A10').should == "abcdef    "
#  end
#
#  it "returns null padded string with ('a')" do
#    ['abcdef'].pack('a7').should == "abcdef\x0"
#  end
#
#  it "cuts string if its size greater than directive count with ('a')" do
#    ['abcde'].pack('a3').should == 'abc'
#  end
#
#  it "considers count = 1 if count omited with ('a')" do
#    ['abcde'].pack('a').should == 'a'
#  end
#
#  it "returns empty string if count = 0 with ('a')" do
#    ['abcde'].pack('a0').should == ''
#  end
#
#  it "returns the whole argument string with star parameter with ('a')" do
#    ['abcdef'].pack('a*').should == 'abcdef'
#  end
#
#  it "raises TypeError if array item is not String with ('a')" do
#    should_raise(TypeError) { [123].pack('a5') }
#    should_raise(TypeError) { [:hello].pack('a5') }
#    should_raise(TypeError) { [Object.new].pack('a5') }
#  end
#
#  it "returns packed bit-string descending order with ('B')" do
#    ["011000010110001001100011"].pack('B24').should == 'abc'
#  end
#
#  it "uses char codes to determine if bit is set or not with ('B')" do
#    ["bccddddefgghhhijjkklllmm"].pack('B24').should == ["011000010110001001100011"].pack('B24')
#  end
#
#  it "conversion edge case: all zeros with ('B')" do
#    ["00000000"].pack('B8').should == "\000"
#  end
#
#  it "conversion edge case: all ones with ('B')" do
#    ["11111111"].pack('B8').should == "\377"
#  end
#
#  it "conversion edge case: left one with ('B')" do
#    ["10000000"].pack('B8').should == "\200"
#  end
#
#  it "conversion edge case: left one with ('B')" do
#    ["00000001"].pack('B8').should == "\001"
#  end
#
#  it "conversion edge case: edge sequences not in first char with ('B')" do
#    ["0000000010000000000000011111111100000000"].pack('B40').should == "\000\200\001\377\000"
#  end
#
#  it "uses zeros if count is not multiple of 8 with ('B')" do
#    ["00111111"].pack('B4').should == ["00110000"].pack('B8')
#  end
#
#  it "returns zero-char for each 2 of count that greater than string length with ('B')" do
#    [""].pack('B6').should == "\000\000\000"
#  end
#
#  it "returns extra zero char if count is odd and greater than string length with ('B')" do
#    [""].pack('B7').should == "\000\000\000\000"
#  end
#
#  it "starts new char if string is ended before char's 8 bits with ('B')" do
#    ["0011"].pack('B8').should == "0\000\000"
#  end
#
#  it "considers count = 1 if no explicit count it given with ('B')" do
#    ["10000000"].pack('B').should == ["10000000"].pack('B1')
#    ["01000000"].pack('B').should == ["01000000"].pack('B1')
#  end
#
#  it "returns empty string if count = 0 with ('B')" do
#    ["10101010"].pack('B0').should == ""
#  end
#
#  it "uses argument string length as count if count = * with ('B')" do
#    ["00111111010"].pack('B*').should == ["00111111010"].pack('B11')
#  end
#
#  it "consumes only one array item with ('B')" do
#    ["0011", "1111"].pack('B*').should == ["0011"].pack('B4')
#    ["0011", "1011"].pack('B*B*').should == ["0011"].pack('B4') + ["1011"].pack('B4')
#  end
#
#  it "raises TypeError if corresponding array item is not String with ('B')" do
#    should_raise(TypeError) { [123].pack('B8') }
#    should_raise(TypeError) { [:data].pack('B8') }
#    should_raise(TypeError) { [Object.new].pack('B8') }
#  end
#
#  it "returns packed bit-string descending order with ('b')" do
#    ["100001100100011011000110"].pack('b24').should == 'abc'
#  end
#
#  it "conversion edge case: all zeros with ('b')" do
#    ["00000000"].pack('b8').should == "\000"
#  end
#
#  it "conversion edge case: all ones with ('b')" do
#    ["11111111"].pack('b8').should == "\377"
#  end
#
#  it "conversion edge case: left one with ('b')" do
#    ["10000000"].pack('b8').should == "\001"
#  end
#
#  it "conversion edge case: left one with ('b')" do
#    ["00000001"].pack('b8').should == "\200"
#  end
#
#  it "conversion edge case: edge sequences not in first char with ('b')" do
#    ["0000000010000000000000011111111100000000"].pack('b40').should == "\000\001\200\377\000"
#  end
#
#  it "uses char codes to determine if bit is set or not with ('b')" do
#    ["abbbbccddefffgghiijjjkkl"].pack('b24').should == ["100001100100011011000110"].pack('b24')
#  end
#
#  it "uses zeros if count is not multiple of 8 with ('b')" do
#    ["00111111"].pack('b4').should == ["00110000"].pack('b8')
#  end
#
#  it "returns zero-char for each 2 of count that greater than string length with ('b')" do
#    [""].pack('b6').should == "\000\000\000"
#  end
#
#  it "returns extra zero char if count is odd and greater than string length with ('b')" do
#    [""].pack('b7').should == "\000\000\000\000"
#  end
#
#  it "starts new char if argument string is ended before char's 8 bits with ('b')" do
#    ["0011"].pack('b8').should == "\f\000\000"
#  end
#
#  it "considers count = 1 if no explicit count it given with ('b')" do
#    ["10000000"].pack('b').should == ["10000000"].pack('b1')
#    ["01000000"].pack('b').should == ["01000000"].pack('b1')
#  end
#
#  it "returns empty string if count = 0 with ('b')" do
#    ["10101010"].pack('b0').should == ""
#  end
#
#  it "uses argument string length as count if count = * with ('b')" do
#    ["00111111010"].pack('b*').should == ["00111111010"].pack('b11')
#  end
#
#  it "consumes only one array item with ('b')" do
#    ["0011", "1111"].pack('b*').should == ["0011"].pack('b4')
#    ["0011", "1011"].pack('b*b*').should == ["0011"].pack('b4') + ["1011"].pack('b4')
#  end
#
#  it "raises TypeError if corresponding array item is not String with ('b')" do
#    should_raise(TypeError) { [123].pack('b8') }
#    should_raise(TypeError) { [:data].pack('b8') }
#    should_raise(TypeError) { [Object.new].pack('b8') }
#  end
#
#  it "returns string with char of appropriate number with ('C')" do
#    [49].pack('C').should == '1'
#  end
#
#  it "reduces value to fit in byte with ('C')" do
#    [257].pack('C').should == "\001"
#  end
#
#  it "converts negative values to positive with ('C')" do
#    [-1].pack('C').should == [255].pack('C')
#    [-257].pack('C').should == [255].pack('C')
#  end
#
#  it "converts float to integer and returns char with that number with ('C')" do
#    [5.0].pack('C').should == [5].pack('C')
#  end
#
#  it "calls to_i on symbol and returns char with that number with ('C')" do
#    [:hello].pack('C').should == [:hello.to_i].pack('C')
#  end
#
#  it "raises TypeErorr if value is string with ('C')" do
#    should_raise(TypeError) { ["hello"].pack('C') }
#  end
#
#  it "processes count number of array elements if count given with ('C')" do
#    [1, 2, 3].pack('C3').should == "\001\002\003"
#  end
#
#  it "returns empty string if count = 0 with ('C')" do
#    [1, 2, 3].pack('C0').should == ''
#  end
#
#  it "with star parameter processes all remaining array items with ('C')" do
#    [1, 2, 3, 4, 5].pack('C*').should == "\001\002\003\004\005"
#  end
#
#  it "raises ArgumentError if count is greater than array elements left with ('C')" do
#    should_raise(ArgumentError) { [1, 2].pack('C3') }
#  end
#
#  it "returns string with char of appropriate number with ('c')" do
#    [49].pack('c').should == '1'
#  end
#
#  it "reduces value to fit in byte with ('c')" do
#    [257].pack('c').should == "\001"
#  end
#
#  it "converts negative values to positive with ('c')" do
#    [-1].pack('c').should == [255].pack('C')
#    [-257].pack('c').should == [255].pack('C')
#  end
#
#  it "converts float to integer and returns char with that number with ('c')" do
#    [5.0].pack('c').should == [5].pack('c')
#  end
#
#  it "calls to_i on symbol and returns char with that number with ('c')" do
#    [:hello].pack('c').should == [:hello.to_i].pack('c')
#  end
#
#  it "raises TypeErorr if value is string with ('c')" do
#    should_raise(TypeError) { ["hello"].pack('c') }
#  end
#
#  it "processes count number of array elements if count given with ('c')" do
#    [1, 2, 3].pack('c3').should == "\001\002\003"
#  end
#
#  it "returns empty string if count = 0 with ('c')" do
#    [1, 2, 3].pack('c0').should == ''
#  end
#
#  it "with star parameter processes all remaining array items with ('c')" do
#    [1, 2, 3, 4, 5].pack('c*').should == "\001\002\003\004\005"
#  end
#
#  it "raises ArgumentError if count is greater than array elements left with ('c')" do
#    should_raise(ArgumentError) { [1, 2].pack('c3') }
#  end
#
#  it "enocdes string with Qouted Printable encoding with ('M')" do
#    ["ABCDEF"].pack('M').should == "ABCDEF=\n"
#  end
#
#  it "doesn't encode new line chars with ('M')" do
#    ["\nA"].pack('M').should == "\nA=\n"
#  end
#
#  it "always appends soft line break at the end of encoded string with ('M')" do
#    ["ABC"].pack('M')[-2, 2].should == "=\n"
#  end
#
#  it "appends soft line break after each 72 chars + 1 encoded char in encoded string with ('M')" do
#    s = ["A"*150].pack('M')
#    s[73, 2].should == "=\n"
#    s[148, 2].should == "=\n"
#
#    s = ["A"*72+"\001"].pack('M')
#    s[75, 2].should == "=\n"
#  end
#
#  it "doesn't quote chars 32..60 and 62..126) with ('M')" do
#    32.upto(60) do |i|
#     [i.chr].pack('M').should == i.chr+"=\n"
#    end
#
#    62.upto(126) do |i|
#     [i.chr].pack('M').should == i.chr+"=\n"
#    end
#  end
#
#  it "quotes chars by adding equal sign and char's hex value with ('M')" do
#    ["\001"].pack('M').should == "=01=\n"
#  end
#
#  it "quotes equal sign with ('M')" do
#    ["="].pack('M').should == "=3D=\n"
#  end
#
#  it "doesn't quote \\t char with ('M')" do
#    ["\t"].pack('M').should == "\t=\n"
#  end
#
#  it "returns empty string if source string is empty with ('M')" do
#    [""].pack('M').should == ""
#  end
#
#  it "calls to_s on object to convert to string with ('M')" do
#    class X; def to_s; "unnamed object"; end; end
#
#    [123].pack('M').should == "123=\n"
#    [:hello].pack('M').should == "hello=\n"
#    [X.new].pack('M').should == "unnamed object=\n"
#  end
#
#  it "ignores count parameter with ('M')" do
#    ["ABC", "DEF", "GHI"].pack('M0').should == ["ABC"].pack('M')
#    ["ABC", "DEF", "GHI"].pack('M3').should == ["ABC"].pack('M')
#  end
#
#  it "ignores star parameter with ('M')" do
#    ["ABC", "DEF", "GHI"].pack('M*').should == ["ABC"].pack('M')
#  end
#
#  it "encodes string with Base64 encoding with ('m')" do
#    ["ABCDEF"].pack('m').should == "QUJDREVG\n"
#  end
#
#  it "converts series of 3-char sequences into four 4-char sequences with ('m')" do
#    ["ABCDEFGHI"].pack('m').size.should == 4+4+4+1
#  end
#
#  it "fills chars with non-significant bits with '=' sign with ('m')" do
#    ["A"].pack('m').should == "QQ==\n"
#  end
#
#  it "appends newline at the end of result string with ('m')" do
#    ["A"].pack('m')[-1, 1].should == "\n"
#  end
#
#  it "appends newline after each 60 chars in result string with ('m')" do
#    s = ["ABC"*31].pack('m')
#    s[60, 1].should == "\n"
#    s[121, 1].should == "\n"
#  end
#
#  it "encodes 6-bit char less than 26 with capital letters with ('m')" do
#    [( 0*4).chr].pack('m').should == "AA==\n"
#    [( 1*4).chr].pack('m').should == "BA==\n"
#
#    [(25*4).chr].pack('m').should == "ZA==\n"
#  end
#
#  it "encodes 6-bit char from 26 to 51 with lowercase letters with ('m')" do
#    [(26*4).chr].pack('m').should == "aA==\n"
#    [(27*4).chr].pack('m').should == "bA==\n"
#
#    [(51*4).chr].pack('m').should == "zA==\n"
#  end
#
#  it "encodes 6-bit char 62 with '+' with ('m')" do
#    [(62*4).chr].pack('m').should == "+A==\n"
#  end
#
#  it "encodes 6-bit char 63 with '/' with ('m')" do
#    [(63*4).chr].pack('m').should == "/A==\n"
#  end
#
#  it "returns empty string if source string is empty with ('m')" do
#    [""].pack('m').should == ""
#  end
#
#  it "raises TypeError if corresponding array item is not string with ('m')" do
#    should_raise(TypeError) { [123].pack('m') }
#    should_raise(TypeError) { [:hello].pack('m') }
#    should_raise(TypeError) { [Object.new].pack('m') }
#  end
#
#  it "ignores count parameter with ('m')" do
#    ["ABC", "DEF", "GHI"].pack('m0').should == ["ABC"].pack('m')
#    ["ABC", "DEF", "GHI"].pack('m3').should == ["ABC"].pack('m')
#  end
#
#  it "ignores star parameter with ('m')" do
#    ["ABC", "DEF", "GHI"].pack('m*').should == ["ABC"].pack('m')
#  end
#
#  it "encodes string with UU-encoding with ('u')" do
#    ["ABCDEF"].pack('u').should == "&04)#1$5&\n"
#  end
#
#  it "converts series of 3-char sequences into four 4-char sequences with ('u')" do
#    ["ABCDEFGHI"].pack('u').size.should == 4+4+4+1+1
#  end
#
#  it "appends zero-chars to source string if string length is not multiple of 3 with ('u')" do
#    ["A"].pack('u').should == "!00``\n"
#  end
#
#  it "appends newline at the end of result string with ('u')" do
#    ["A"].pack('u')[-1, 1].should == "\n"
#  end
#
#  it "splits source string into lines with no more than 45 chars with ('u')" do
#    s = ["A"*91].pack('u')
#    s[61, 1].should == "\n"
#    s[123, 1].should == "\n"
#  end
#
#  it "prepends encoded line length to each line with ('u')" do
#    s = ["A"*50].pack('u')
#    s[ 0].should == 45+32
#    s[62].should ==  5+32
#  end
#
#  it "encodes 6-bit char with another char starting from char 32 with ('u')" do
#    [( 1*4).chr].pack('u').should == "!!```\n"
#    [(16*4).chr].pack('u').should == "!0```\n"
#    [(25*4).chr].pack('u').should == "!9```\n"
#    [(63*4).chr].pack('u').should == "!_```\n"
#  end
#
#  it "replaces spaces in encoded string with grave accent (`) char with ('u')" do
#    [( 0*4).chr].pack('u').should == "!````\n"
#  end
#
#  it "returns empty string if source string is empty with ('u')" do
#    [""].pack('u').should == ""
#  end
#
#  it "raises TypeError if corresponding array item is not string with ('u')" do
#    should_raise(TypeError) { [123].pack('u') }
#    should_raise(TypeError) { [:hello].pack('u') }
#    should_raise(TypeError) { [Object.new].pack('u') }
#  end
#
#  it "ignores count parameter with ('u')" do
#    ["ABC", "DEF", "GHI"].pack('u0').should == ["ABC"].pack('u')
#    ["ABC", "DEF", "GHI"].pack('u3').should == ["ABC"].pack('u')
#  end
#
#  it "ignores star parameter with ('u')" do
#    ["ABC", "DEF", "GHI"].pack('u*').should == ["ABC"].pack('u')
#  end
#
#  it "decreases result string by one char with ('X')" do
#    ['abcdef'].pack('A4X').should == 'abc'
#  end
#
#  it "with count decreases result string by count chars with ('X')" do
#    ['abcdef'].pack('A6X4').should == 'ab'
#  end
#
#  it "with zero count doesnt change result string with ('X')" do
#    ['abcdef'].pack('A6X0').should == 'abcdef'
#  end
#
#  it "treats start parameter as zero count with ('X')" do
#    ['abcdef'].pack('A6X*').should == 'abcdef'
#  end
#
#  it "raises ArgumentError if count greater than already generated string length with ('X')" do
#    should_raise(ArgumentError) { ['abcdef'].pack('A6X7') }
#  end
#
#  it "raises ArgumentError if it is first directive with ('X')" do
#    should_raise(ArgumentError) { [].pack('X') }
#  end
#
#  it "returns zero-char string with ('x')" do
#    [].pack('x').should == "\000"
#  end
#
#  it "returns string of count zero chars with count and ('x')" do
#    [].pack('x5').should == "\000\000\000\000\000"
#  end
#
#  it "returns empty string with count == 0 and ('x')" do
#    [].pack('x0').should == ""
#  end
#
#  it "behaves like with count == 0 with star parameter and ('x')" do
#    [].pack('x*').should == ""
#  end
#
#  it "returns null padded string with ('Z')" do
#    ['abcdef'].pack('Z7').should == "abcdef\000"
#  end
#
#  it "cuts string if its size greater than directive count with ('Z')" do
#    ['abcde'].pack('Z3').should == 'abc'
#  end
#
#  it "considers count = 1 if count omited with ('Z')" do
#    ['abcde'].pack('Z').should == 'a'
#  end
#
#  it "returns empty string if count = 0 with ('Z')" do
#    ['abcde'].pack('Z0').should == ''
#  end
#
#  it "returns the whole argument string plus null char with star parameter with ('Z')" do
#    ['abcdef'].pack('Z*').should == "abcdef\000"
#  end
#
#  it "raises TypeError if array item is not String with ('Z')" do
#    should_raise(TypeError) { [123].pack('Z5') }
#    should_raise(TypeError) { [:hello].pack('Z5') }
#    should_raise(TypeError) { [Object.new].pack('Z5') }
#  end
#end
