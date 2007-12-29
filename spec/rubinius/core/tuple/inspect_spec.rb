require File.dirname(__FILE__) + '/../../../spec_helper'

describe "Tuple#inspect" do
  it "returns a string representation" do
    Tuple[:a, 1, :b, 2].inspect.should == "#<Tuple: :a, 1, :b, 2>"
  end
end
