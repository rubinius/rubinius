require File.dirname(__FILE__) + '/../spec_helper'

# all?, any?, collect, detect, 
# each_with_index, entries,
# find, find_all, grep, include?, inject, map, max, member?, min,
# partition, reject, select, sort, sort_by, to_a, zip

context "A class with Enumerable mixin, method" do
  setup do
    @src = <<-CODE
    class Numerous
      include Enumerable
      
      def initialize(*list)
        @list = list.empty? ? [2, 5, 3, 6, 1, 4] : list
      end
      
      def each
        @list.each { |i| yield i }
      end
      
    end
    CODE
  end
  
  specify "each should be provided" do
    example(@src) do
      Numerous.new.respond_to?(:each)
    end.should == true
  end
  
  specify "each should provide each element to the block" do
    example(@src) do
      @a = []
      Numerous.new.each { |i| @a << i }
      @a
    end.should == [2, 5, 3, 6, 1, 4]
  end
  
  specify "all? with no block should return true if no elements are false or nil" do
    example(@src) do
      [Numerous.new.all?, Numerous.new(1, nil, 2).all?, Numerous.new(false).all?]
    end.should == [true, false, false]
  end
  
  specify "all? should return true if the block never returns false or nil" do
    example(@src) do
      Numerous.new.all? { true }
    end.should == true
  end
  
  specify "all? should return false if the block ever returns false or nil" do
    example(@src) do
      [Numerous.new.all? { |i| i > 5 }, Numerous.new.all? { |i| i == 3 ? nil : true }]
    end.should == [false, false]
  end
  
  specify "any? with no block should return true if any element is not false or nil" do
    example(@src) do
      [Numerous.new.any?, Numerous.new(1, nil, 2).any?, Numerous.new(false).any?]
    end.should == [true, true, false]
  end
  
  specify "any? should return true if the block ever returns other than false or nil" do
    example(@src) do
      Numerous.new.any? { |i| i == 5 }
    end.should == true
  end
  
  specify "any? should return false if the block never returns other than false or nil" do
    example(@src) do
      [Numerous.new.any? { false }, Numerous.new.any? { nil }]
    end.should == [false, false]
  end
  
  specify "collect should return a new array with the results of passing each element to block" do
    example(@src) do
      Numerous.new.collect { |i| i % 2 }
    end.should == [0, 1, 1, 0, 1, 0]
  end
  
  specify "detect should return the first element for which block is not false" do
    example(@src) do
      Numerous.new.detect { |i| i > 3 }
    end.should == 5
  end
  
  specify "each_with_index should pass each element and its index to block" do
    example(@src) do
      @a = []
      Numerous.new.each_with_index { |o, i| @a << [o, i] }
      @a
    end.should == [[2, 0], [5, 1], [3, 2], [6, 3], [1, 4], [4, 5]]
  end
  
  specify "entries should return an array containing the elements" do
    example(@src) do
      Numerous.new(1, nil, 'a', 2, false, true).entries
    end.should == [1, nil, "a", 2, false, true]
  end
  
  specify "find should be a synonym for detect" do
    example(@src) do
      Numerous.new.detect { |i| i > 3 }
    end.should == 5
  end
  
  specify "find_all should be a synonym for select" do
    example(@src) do
      Numerous.new.find_all { |i| i % 2 == 0 }
    end.should == [2, 6, 4]
  end
  
  specify "grep without a block should return an array of all elements === pattern" do
    example(@src) do
      Numerous.new('2', 'a', 'nil', '3', false).grep(/[235]/)
    end.should == ["2", "3"]
  end
  
  specify "grep with a block should return an array of elements === pattern passed to block" do
    example(@src) do
      Numerous.new("cat", "coat", "car", "cadr", "cost").grep(/^ca/) { |i| i.upcase }
    end.should == ["CAT", "CAR", "CADR"]
  end
  
  specify "include? should return true if any element == argument" do
    example(@src) do
      [Numerous.new.include?(5), Numerous.new.include?(10)]
    end.should == [true, false]
  end
    
  specify "inject without argument should combine the elements by passing accumulator value and each element (skipping the first) in turn to block" do
    example(@src) do
      Numerous.new.inject { |sum, n| sum + n }
    end.should == 21
  end
  
  specify "inject without argument should combine the elements by passing accumulator value and each element (including the first) in turn to block" do
    example(@src) do
      Numerous.new.inject(5) { |diff, n| diff - n }
    end.should == -16
  end
  
  specify "map should be a synonym for collect" do
    example(@src) do
      Numerous.new.collect { |i| i % 2 }
    end.should == [0, 1, 1, 0, 1, 0]
  end
  
  specify "member? should be a synonym for include?" do
    example(@src) do
      [Numerous.new.include?(5), Numerous.new.include?(10)]
    end.should == [true, false]
  end
  
  specify "partition should return two arrays, the first containing elements for which the block is true, the second containing the rest" do
    example(@src) do
      Numerous.new.partition { |i| i % 2 == 0 }
    end.should == [[2, 6, 4], [5, 3, 1]]
  end
  
  specify "reject should return an array of the elements for which block is false" do
    example(@src) do
      Numerous.new.reject { |i| i > 3 }
    end.should == [2, 3, 1]
  end
  
  specify "select should return an array of all elements for which block is not false" do
    example(@src) do
      Numerous.new.find_all { |i| i % 2 == 0 }
    end.should == [2, 6, 4]
  end
  
  specify "to_a should should be a synonym for entries" do
    example(@src) do
      Numerous.new(1, nil, 'a', 2, false, true).entries
    end.should == [1, nil, "a", 2, false, true]
  end
  
  specify "zip without a block should merge elements with corresponding elements from each argument" do
    example(@src) do
      Numerous.new.zip(['a', 'b', 'c', 'c', 'd'], [false, nil, 1, true, 2, 0])
    end.should == [[2, "a", false], [5, "b", nil], [3, "c", 1], [6, "c", true], [1, "d", 2], [4, nil, 0]]
  end
  
  specify "zip should merge elements with corresponding elements from each argument and return an array from passing each of these to block" do
    example(@src) do
      @a = []
      Numerous.new.zip(1..6) { |a| @a << a }
      @a
    end.should == [[2, 1], [5, 2], [3, 3], [6, 4], [1, 5], [4, 6]]
  end
end

context "A class with Enumerable mixin and <=> defined, method" do
  setup do
    @src = <<-CODE
    class Numerous
      include Enumerable
      
      def initialize(*list)
        @list = list.empty? ? [2, 5, 3, 6, 1, 4] : list
      end
      
      def each
        @list.each { |i| yield i }
      end
      
      def <=>(other)
        not self <=> other
      end
    end
    CODE
  end
  
  specify "max should return the maximum element" do
    example(@src) do
      Numerous.new.max
    end.should == 6
  end
  
  specify "min should return the minimum element" do
    example(@src) do
      Numerous.new.min
    end.should == 1
  end
  
  specify "sort without a block should should return an array of elements ordered lowest to highest" do
    example(@src) do
      Numerous.new.sort
    end.should == [1, 2, 3, 4, 5, 6]
  end
  
  specify "sort with a block should return an array of elements ordered by the result of block" do
    example(@src) do
      Numerous.new.sort { |a, b| b <=> a }
    end.should == [6, 5, 4, 3, 2, 1]
  end
  
  specify "sort_by should return an array of elements ordered by the result of block" do
    example(@src) do
      Numerous.new("once", "upon", "a", "time").sort_by { |i| i[0] }
    end.should == ["a", "once", "time", "upon"]
  end
end
