require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Enumerable#inject" do  
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
  
  it "inject with inject arguments(legacy rubycon)" do
    # with inject argument
    EachDefiner.new().inject(1) {|acc,x| 999 }.should == 1
    EachDefiner.new(2).inject(1) {|acc,x| 999 }.should ==  999
    EachDefiner.new(2).inject(1) {|acc,x| acc }.should == 1
    EachDefiner.new(2).inject(1) {|acc,x| x }.should == 2

    EachDefiner.new(1,2,3,4).inject(100) {|acc,x| acc + x }.should == 110
    EachDefiner.new(1,2,3,4).inject(100) {|acc,x| acc * x }.should == 2400

    EachDefiner.new('a','b','c').inject("z") {|result, i| i+result}.should == "cbaz"
  end
  
  it "inject withou inject arguments(legacy rubycon)" do
    # no inject argument
    EachDefiner.new(2).inject {|acc,x| 999 } .should == 2
    EachDefiner.new(2).inject {|acc,x| acc }.should == 2
    EachDefiner.new(2).inject {|acc,x| x }.should == 2

    EachDefiner.new(1,2,3,4).inject {|acc,x| acc + x }.should == 10
    EachDefiner.new(1,2,3,4).inject {|acc,x| acc * x }.should == 24

    EachDefiner.new('a','b','c').inject {|result, i| i+result}.should == "cba"
    EachDefiner.new(3, 4, 5).inject {|result, i| result*i}.should == 60
    EachDefiner.new([1], 2, 'a','b').inject{|r,i| r<<i}.should == [1, 2, 'a', 'b']

  end
  it "return nil when fails(legacy rubycon)" do
    EachDefiner.new().inject {|acc,x| 999 }.should == nil 
  end
end
