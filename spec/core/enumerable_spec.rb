require File.dirname(__FILE__) + '/../spec_helper'

# all?, any?, collect, detect, 
# each_with_index, entries,
# find, find_all, grep, include?, inject, map, max, member?, min,
# partition, reject, select, sort, sort_by, to_a, zip
class Numerous
  include Enumerable
    
  def initialize(*list)
    @list = list.empty? ? [2, 5, 3, 6, 1, 4] : list
  end
    
  def each
    @list.each { |i| yield i }
  end      
end

# (Legacy form rubycon)
class EachDefiner

  include Enumerable

  attr_reader :arr

  def initialize(*arr)
    @arr = arr
  end
  
  def each
    i = 0
    loop do
      break if i == @arr.size
      yield @arr[i]
      i += 1
    end
  end

end 

context "A class with Enumerable mixin, cmd" do
  
  it "detect should return the first element for which block is not false" do
    Numerous.new.detect { |i| i > 3 }.should == 5
  end
  
  it "each_with_index should pass each element and its index to block" do
    @a = []
    Numerous.new.each_with_index { |o, i| @a << [o, i] }
    @a.should == [[2, 0], [5, 1], [3, 2], [6, 3], [1, 4], [4, 5]]
  end
  
  it "find should be a synonym for detect" do
    Numerous.new.find { |i| i > 3 }.should == Numerous.new.detect { |i| i > 3 }
  end
  
  it "find_all should be a synonym for select" do
    Numerous.new.find_all { |i| i % 2 == 0 }.should == Numerous.new.select { |i| i % 2 == 0 }
  end
  
  it "grep without a block should return an array of all elements === pattern" do
    class EnumerableSpecGrep; def ===(obj); obj == '2'; end; end

    Numerous.new('2', 'a', 'nil', '3', false).grep(EnumerableSpecGrep.new).should == ['2']
  end
  
  it "grep with a block should return an array of elements === pattern passed through block" do
    class EnumerableSpecGrep2; def ===(obj); /^ca/ =~ obj; end; end

    Numerous.new("cat", "coat", "car", "cadr", "cost").grep(EnumerableSpecGrep2.new) { |i| i.upcase }.should == ["CAT", "CAR", "CADR"]
  end
  
  it "include? should return true if any element == argument" do
    class EnumerableSpecIncludeP; def ==(obj) obj == 5; end; end

    Numerous.new.include?(5).should == true 
    Numerous.new.include?(10).should == false
    Numerous.new.include?(EnumerableSpecIncludeP.new).should == true
  end
  
  it "inject with argument takes a block with an accumulator (with argument as initial value) and the current element. Value of block becomes new accumulator" do
    a = []
    Numerous.new.inject(0) { |memo, i| a << [memo, i]; i }
    a.should == [[0, 2], [2, 5], [5, 3], [3, 6], [6, 1], [1, 4]]
  end
    
  it "inject without argument takes a block with an accumulator (with first element as initial value) and the current element. Value of block becomes new accumulator" do
    a = []
    Numerous.new.inject { |memo, i| a << [memo, i]; i }
    a.should == [[2, 5], [5, 3], [3, 6], [6, 1], [1, 4]]
  end
  
  it "map should be a synonym for collect" do
    Numerous.new.map { |i| i % 2 }.should == Numerous.new.collect { |i| i % 2 }
  end
  
  it "member? should be a synonym for include?" do
    Numerous.new.include?(5).should ==  Numerous.new.member?(5)
    Numerous.new.include?(10).should ==  Numerous.new.member?(10)
  end
  
  it "partition should return two arrays, the first containing elements for which the block is true, the second containing the rest" do
    Numerous.new.partition { |i| i % 2 == 0 }.should == [[2, 6, 4], [5, 3, 1]]
  end
  
  it "reject should return an array of the elements for which block is false" do
    Numerous.new.reject { |i| i > 3 }.should == [2, 3, 1]
  end
  
  it "select should return an array of all elements for which block is not false" do
    Numerous.new.find_all { |i| i % 2 == 0 }.should == [2, 6, 4]
  end 
  
  it "zip without a block should merge elements with corresponding indexes from each #to_a argument" do
    class EnumerableSpecZip; def to_a(); [1, 2, 3, 4, 5, 6]; end; end

    a, b, c = ['a', 'b', 'c', 'c', 'd'], [false, nil, 1, true, 2, 0], EnumerableSpecZip.new

    Numerous.new.zip(a, b).should == [[2, "a", false],[5, "b", nil],[3, "c", 1],[6, "c", true],[1, "d", 2],[4, nil, 0]]
    Numerous.new.zip(c).should == [[2, 1], [5, 2], [3, 3], [6, 4], [1, 5], [4, 6]]
  end
  
  it "zip with a block merges corresponding indexes and passes these to the block" do
    @a = []
    Numerous.new.zip(1..6) { |a| @a << a }
    @a.should == [[2, 1], [5, 2], [3, 3], [6, 4], [1, 5], [4, 6]]
  end
end


describe "File.each" do    
  it "be provided" do
    Numerous.new.respond_to?(:each).should == true
  end
  
  it "provide each element to the block" do 
    @b=[]
    Numerous.new.each { |i| @b << i }
    @b.should == [2, 5, 3, 6, 1, 4]
  end 
end

describe "File.each_with_index" do
  before :each do
    @b = [2, 5, 3, 6, 1, 4]  
  end
  
  it "provide each element to the block" do  
    acc = []
    obj = EachDefiner.new()
    res = obj.each_with_index {|a,i| acc << [a,i]}
    acc.should == []
    obj.should == res
  end
  
  it "" do
    acc = [] 
    res = @b.each_with_index {|a,i| acc << [a,i]}
    [[2, 0], [5, 1], [3, 2], [6, 3], [1, 4], [4, 5]].should == acc
    @b.should == res
  end
end

context "A class with Enumerable mixin and <=> defined, cmd" do 
  
  it "min should return the minimum element" do
    Numerous.new.min.should == 1
  end
  
  it "sort without a block should should return an array of elements ordered lowest to highest" do
    Numerous.new.sort.should == [1, 2, 3, 4, 5, 6]
  end
  
  it "sort with a block should return an array of elements ordered by the result of block" do
    Numerous.new.sort { |a, b| b <=> a }.should == [6, 5, 4, 3, 2, 1]
  end
  
  it "sort_by should return an array of elements ordered by the result of block" do
    Numerous.new("once", "upon", "a", "time").sort_by { |i| i[0] }.should == ["a", "once", "time", "upon"]
  end
end

describe "Enumerable#all" do   
  before :each do
    @enum1 = [0, 1, 2, -1]
    @enum2 = [nil, false, true]
  end 

  it "fail when have a wrong argument "  do
    should_raise(ArgumentError){ @enum1.all?(1) }
  end

  it "with no block should return true if no elements are false or nil" do
    Numerous.new.all?.should == true 
    Numerous.new(1, nil, 2).all?.should == false 
    Numerous.new(false).all?.should == false 
    
    @enum1.all?.should == true
    @enum2.all?.should == false
  end
  
  it "return true if the block never returns false or nil" do
    Numerous.new.all? { true }.should == true 
    @enum1.all?{ |o| o < 5 }.should == true
    @enum1.all?{ |o| o > 2 }.should == false
  end
  
  it "return false if the block ever returns false or nil" do
    Numerous.new.all? { |i| i > 5 }.should == false
    Numerous.new.all? { |i| i == 3 ? nil : true }.should == false
  end  
  
  it "Passes each element of the collection to the given block. (Legacy form rubycon)" do
    @enum1.all?{ |o| 5.times{ @enum1.shift } }.should == true  
    EachDefiner.new().all?.should == true
    EachDefiner.new('a','b','c').all?.should == true
    EachDefiner.new(0, "x", true).all?.should == true
    EachDefiner.new(nil).all?.should == false
    EachDefiner.new(nil, nil).all?.should == false
    EachDefiner.new(false).all?.should == false
    EachDefiner.new(false, false).all?.should == false
    EachDefiner.new(0, "x", false, true).all?.should == false
  end
  
  after :each do
    @enum1 = nil
  end
  
end

describe "Enumerable#any?" do
  it "any? with no block should return true if any element is not false or nil" do
    Numerous.new.any?.should == true 
    Numerous.new(1, nil, 2).any?.should == true 
    Numerous.new(false).any?.should == false     
  end
  
  it "any? should return true if the block ever returns other than false or nil" do
    Numerous.new.any? { |i| i == 5 }.should == true
  end
  
  it "any? should return false if the block never returns other than false or nil" do
    Numerous.new.any? { false }.should == false
    Numerous.new.any? { nil }.should == false
  end
  
  it "any? with no block should return true if any element is not false or nil (Legacy form rubycon)" do
    EachDefiner.new('a','b','c').any?.should == true
    EachDefiner.new(false, 0, nil).any?.should == true
    EachDefiner.new().any?.should == false
    EachDefiner.new(false, nil).any?.should == false    
    EachDefiner.new(true).any?.should == true
  end
end

describe "Enumerable.max" do  
  before :each do
    @a = EachDefiner.new( 2, 4, 6, 8, 10 )

    @e_strs = EachDefiner.new("333", "22", "666666", "1", "55555", "1010101010")
    @e_ints = EachDefiner.new( 333,   22,   666666,   1,   55555,   1010101010)
  end
  
  it "max should return the maximum element" do
    Numerous.new.max.should == 6
  end
  
  it "return the maximum element (basics cases)" do 
    EachDefiner.new(55).max.should == 55

    EachDefiner.new(11,99).max.should == 99
    EachDefiner.new(99,11).max.should == 99
    EachDefiner.new(2, 33, 4, 11).max.should == 33

    EachDefiner.new(1,2,3,4,5).max.should == 5
    EachDefiner.new(5,4,3,2,1).max.should == 5
    EachDefiner.new(1,4,3,5,2).max.should == 5
    EachDefiner.new(5,5,5,5,5).max.should == 5

    EachDefiner.new("aa","tt").max.should == "tt"
    EachDefiner.new("tt","aa").max.should == "tt"
    EachDefiner.new("2","33","4","11").max.should == "4"

    @e_strs.max.should == "666666"
    @e_ints.max.should == 1010101010
  end
  
  it "return an error when introduce the wrong kind or number of parameters " do
    # error cases
    EachDefiner.new().max.should == nil
    should_raise(NoMethodError) { EachDefiner.new(Object.new, Object.new).max }
    should_raise(ArgumentError) { EachDefiner.new(11,"22").max }
  end
 
  it "return the maximum element (with block" do
    # with a block
    EachDefiner.new("2","33","4","11").max {|a,b| a <=> b }.should == "4"
    EachDefiner.new( 2 , 33 , 4 , 11 ).max {|a,b| a <=> b }.should == 33

    EachDefiner.new("2","33","4","11").max {|a,b| b <=> a }.should == "11"
    EachDefiner.new( 2 , 33 , 4 , 11 ).max {|a,b| b <=> a }.should == 2

    @e_strs.max {|a,b| a.length <=> b.length }.should == "1010101010"

    @e_strs.max {|a,b| a <=> b }.should == "666666"
    @e_strs.max {|a,b| a.to_i <=> b.to_i }.should == "1010101010"

    @e_ints.max {|a,b| a <=> b }.should == 1010101010
    @e_ints.max {|a,b| a.to_s <=> b.to_s }.should == 666666
  end
end

generic_collect_map = shared "Range#collect" do |cmd|
  describe "Range##{cmd}" do
    before :each do
      @a = EachDefiner.new( 2, 4, 6, 8, 10 )
    end
    
    it "collect should return a new array with the results of passing each element to block" do
      Numerous.new.send(cmd) { |i| i % 2 }.should == [0, 1, 1, 0, 1, 0]
    end 
    
    it "returns true if other has same begin, end, and exclude_end?" do
      #@a.send(cmd).should == [2,4,6,8,10]
      EachDefiner.new().send(cmd) {|a| a }.should == []
      @a.send(cmd) {|a| a-1 }.should == [1,3,5,7,9]
      @a.send(cmd) {|a| 1 }.should == [1,1,1,1,1]
      @a.send(cmd) {|a| [a-1] }.should == [[1],[3],[5],[7],[9]]
    end    
  end
end

describe "File.collect" do   
  it_behaves_like(generic_collect_map , :collect) 
end

describe "File.map" do   
  it_behaves_like(generic_collect_map , :map) 
end

generic_entries_to_a = shared "Range#collect" do |cmd|
  describe "Range##{cmd}" do
    before :each do
      @a = EachDefiner.new( 2, 4, 6, 8, 10 )
    end
    
    it "returns an array containing the items in enum." do
      EachDefiner.new().send(cmd).should ==[]
      EachDefiner.new(3).send(cmd).should ==[3]
      @a.send(cmd).should ==@a.arr
      EachDefiner.new(3,4,5).send(cmd).should ==[3,4,5]
      EachDefiner.new(false,3,nil,4,5,nil).send(cmd).should ==[false,3,nil,4,5,nil]
    end
 
    it "entries should return an array containing the elements" do
      Numerous.new(1, nil, 'a', 2, false, true).send(cmd).should == [1, nil, "a", 2, false, true]
    end
  
    it "to_a should should be a synonym for entries" do
      Numerous.new(1, nil, 'a', 2, false, true).to_a.should == Numerous.new(1, nil, 'a', 2, false, true).entries
    end
  end
end

describe "File.entries" do   
  it_behaves_like(generic_entries_to_a , :entries) 
end

describe "File.to_a" do   
  it_behaves_like(generic_entries_to_a , :to_a) 
end

