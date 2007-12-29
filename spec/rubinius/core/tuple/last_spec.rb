require File.dirname(__FILE__) + '/../../../spec_helper'

describe "Tuple#last" do
  it "returns the last element" do
    t = Tuple[:a, 'b', 3]
    t.last.should == 3
  end
end
