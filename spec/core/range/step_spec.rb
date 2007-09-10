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
    (0.5..2.4).step(0.5) { |x| a << x}
    a.should == [0.5, 1, 1.5,2]
  end

  it "does not allow negative numbers in stepsize" do
    should_raise(ArgumentError) do
      a = []
      (-5..5).step(-2) { |x| a << x }
    end
  end

  it "does not allow zero in stepsize" do
    should_raise(ArgumentError) do
      a = []
      (-5..5).step(0) { |x| a << x }
    end

    should_raise(ArgumentError) do
      a = []
      (-5.5..5.7).step(0.0) { |x| a << x }
    end
  end
end
