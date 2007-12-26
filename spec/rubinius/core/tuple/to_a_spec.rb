require File.dirname(__FILE__) + '/../../../spec_helper'

extension :rubinius do
  describe "Tuple#to_a" do
    it "returns an array containing the elements of the tuple" do
      Tuple[:a, 1, :b, 2].to_a == [:a, 1, :b, 2]
      Tuple.new(0).to_a == []
    end
  end
end
