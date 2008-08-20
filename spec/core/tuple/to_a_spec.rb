require File.dirname(__FILE__) + '/../../spec_helper'

describe "Tuple#to_a" do
  it "returns an array containing the elements of the tuple" do
    Tuple[:a, 1, :b, 2].to_a.should == [:a, 1, :b, 2]
    Tuple.new(0).to_a.should == []
  end

  it "does not skip nil values" do
    Tuple[:a, nil, :b, 2, nil].to_a.should == [:a, nil, :b, 2, nil]
    Tuple.new(2).to_a.should == [nil, nil]
  end
end
