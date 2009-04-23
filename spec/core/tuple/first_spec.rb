require File.dirname(__FILE__) + '/../../spec_helper'

describe "Rubinius::Tuple#first" do
  it "returns the first element" do
    t = Rubinius::Tuple[:foo, :bar, :baz]
    t.first.should == :foo
  end
end
