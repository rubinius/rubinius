require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::Tuple#first" do
  it "returns the first element" do
    t = Rubinius::Tuple[:foo, :bar, :baz]
    t.first.should == :foo
  end
end
