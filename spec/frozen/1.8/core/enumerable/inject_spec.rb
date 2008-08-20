require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Enumerable#inject" do  
  it "inject with argument takes a block with an accumulator (with argument as initial value) and the current element. Value of block becomes new accumulator" do
    a = []
    EnumerableSpecs::Numerous.new.inject(0) { |memo, i| a << [memo, i]; i }
    a.should == [[0, 2], [2, 5], [5, 3], [3, 6], [6, 1], [1, 4]]
    EnumerableSpecs::EachDefiner.new(true, true, true).inject(nil) {|result, i| i && result}.should == nil
  end

  it "produces an array of the accumulator and the argument when given a block with a *arg" do
    a = []
    [1,2].inject(0) {|*args| a << args; args[0] + args[1]}
    a.should == [[0, 1], [1, 2]]
  end
  
  it "only takes one argument" do
    lambda { EnumerableSpecs::Numerous.new.inject(0, 1) { |memo, i| i } }.should raise_error(ArgumentError)
  end
    
  it "inject without argument takes a block with an accumulator (with first element as initial value) and the current element. Value of block becomes new accumulator" do
    a = []
    EnumerableSpecs::Numerous.new.inject { |memo, i| a << [memo, i]; i }
    a.should == [[2, 5], [5, 3], [3, 6], [6, 1], [1, 4]]
  end  
  
  it "inject with inject arguments(legacy rubycon)" do
    # with inject argument
    EnumerableSpecs::EachDefiner.new().inject(1) {|acc,x| 999 }.should == 1
    EnumerableSpecs::EachDefiner.new(2).inject(1) {|acc,x| 999 }.should ==  999
    EnumerableSpecs::EachDefiner.new(2).inject(1) {|acc,x| acc }.should == 1
    EnumerableSpecs::EachDefiner.new(2).inject(1) {|acc,x| x }.should == 2

    EnumerableSpecs::EachDefiner.new(1,2,3,4).inject(100) {|acc,x| acc + x }.should == 110
    EnumerableSpecs::EachDefiner.new(1,2,3,4).inject(100) {|acc,x| acc * x }.should == 2400

    EnumerableSpecs::EachDefiner.new('a','b','c').inject("z") {|result, i| i+result}.should == "cbaz"
  end
  
  it "inject withou inject arguments(legacy rubycon)" do
    # no inject argument
    EnumerableSpecs::EachDefiner.new(2).inject {|acc,x| 999 } .should == 2
    EnumerableSpecs::EachDefiner.new(2).inject {|acc,x| acc }.should == 2
    EnumerableSpecs::EachDefiner.new(2).inject {|acc,x| x }.should == 2

    EnumerableSpecs::EachDefiner.new(1,2,3,4).inject {|acc,x| acc + x }.should == 10
    EnumerableSpecs::EachDefiner.new(1,2,3,4).inject {|acc,x| acc * x }.should == 24

    EnumerableSpecs::EachDefiner.new('a','b','c').inject {|result, i| i+result}.should == "cba"
    EnumerableSpecs::EachDefiner.new(3, 4, 5).inject {|result, i| result*i}.should == 60
    EnumerableSpecs::EachDefiner.new([1], 2, 'a','b').inject{|r,i| r<<i}.should == [1, 2, 'a', 'b']

  end
  it "return nil when fails(legacy rubycon)" do
    EnumerableSpecs::EachDefiner.new().inject {|acc,x| 999 }.should == nil 
  end
end
