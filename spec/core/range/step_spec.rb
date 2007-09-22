require File.dirname(__FILE__) + '/../../spec_helper'

describe "Range#step" do
  it "passes each nth element to the block" do
    a = []
    (-5..5).step(2) { |x| a << x }
    a.should == [-5, -3, -1, 1, 3, 5]

    a = []
    ("A".."F").step(2) { |x| a << x }
    a.should == ["A", "C", "E"]

    a = []
    ("A"..."G").step(2) { |x| a << x }
    a.should == ["A", "C", "E"]
    
    a = []
    (0.5..2.4).step(0.5) { |x| a << x }
    a.should == [0.5, 1, 1.5, 2]
  end

  it "raises an ArgumentError if stepsize is 0 or negative" do
    should_raise(ArgumentError, "step can't be 0") do
      (-5..5).step(0) { |x| x }
    end

    should_raise(ArgumentError, "step can't be 0") do
      (-5.5..5.7).step(0.0) { |x| x }
    end

    should_raise(ArgumentError, "step can't be negative") do
      (-5..5).step(-2) { |x| x }
    end
  end

  it "raises a TypeError if the first element does not respond to #succ" do
    b = Object.new
    (a = Object.new).should_receive(:method_missing, :with => [:<=>, b], :returning => 1)
    
    should_raise(TypeError, "can't iterate from Object") do
      (a..b).step(1) { |i| i }
    end
  end

  it "returns self" do
    (1..10).step(1) {}.should == (1..10)
  end
end
