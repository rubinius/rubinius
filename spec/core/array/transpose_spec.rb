require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

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
