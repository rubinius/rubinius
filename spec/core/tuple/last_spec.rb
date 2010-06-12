require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::Tuple#last" do
  it "returns the last element" do
    t = Rubinius::Tuple[:a, 'b', 3]
    t.last.should == 3
  end
end
