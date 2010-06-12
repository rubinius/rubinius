require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::Tuple#inspect" do
  it "returns a string representation" do
    Rubinius::Tuple[:a, 1, :b, 2].inspect.should == "#<Rubinius::Tuple: :a, 1, :b, 2>"
  end
end
