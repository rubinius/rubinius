require File.dirname(__FILE__) + '/../spec_helper'

# all?, any?, collect, detect, 
# each_with_index, entries,
# find, find_all, grep, include?, inject, map, max, member?, min,
# partition, reject, select, sort, sort_by, to_a, zip

context "A class with Enumerable mixin, cmd" do
  
  it "detect should return the first element for which block is not false" do
    Numerous.new.detect { |i| i > 3 }.should == 5
  end  
    
  it "map should be a synonym for collect" do
    Numerous.new.map { |i| i % 2 }.should == Numerous.new.collect { |i| i % 2 }
  end
    
  it "partition should return two arrays, the first containing elements for which the block is true, the second containing the rest" do
    Numerous.new.partition { |i| i % 2 == 0 }.should == [[2, 6, 4], [5, 3, 1]]
  end
  
  it "reject should return an array of the elements for which block is false" do
    Numerous.new.reject { |i| i > 3 }.should == [2, 3, 1]
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

context "A class with Enumerable mixin and <=> defined, cmd" do  
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



