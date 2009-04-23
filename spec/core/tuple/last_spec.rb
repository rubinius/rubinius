require File.dirname(__FILE__) + '/../../spec_helper'

describe "Rubinius::Tuple#last" do
  it "returns the last element" do
    t = Rubinius::Tuple[:a, 'b', 3]
    t.last.should == 3
  end
end
