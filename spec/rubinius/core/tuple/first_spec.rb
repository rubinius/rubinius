require File.dirname(__FILE__) + '/../../../spec_helper'

describe "Tuple#first" do
  it "returns the first element" do
    t = Tuple[:foo, :bar, :baz]
    t.first.should == :foo
  end
end
