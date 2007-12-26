require File.dirname(__FILE__) + '/../../../spec_helper'

extension :rubinius do
  describe "Tuple#first" do
    it "returns the first element" do
      t = Tuple[:foo, :bar, :baz]
      t.first.should == :foo
    end
  end
end
