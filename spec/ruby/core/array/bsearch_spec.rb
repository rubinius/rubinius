require File.expand_path('../../../spec_helper', __FILE__)

describe "Array#bsearch" do
  context "with a block returning true or false" do
    it "returns nil if the block returns false for every element" do
      [0, 1, 2, 3].bsearch { |x| x > 3 }.should be_nil
    end

    it "returns element at zero if the block returns true for every element" do
      [0, 1, 2, 3].bsearch { |x| x < 4 }.should == 0

    end

    it "returns the element at the smallest index for which block returns true" do
      [0, 1, 3, 4].bsearch { |x| x >= 2 }.should == 3
    end
  end

  context "with a block returning negative, zero, positive numbers" do
    it "returns nil if the block returns less than zero for every element" do
      [0, 1, 2, 3].bsearch { |x| x <=> 5 }.should be_nil
    end

    it "returns nil if the block returns greater than zero for every element" do
      [0, 1, 2, 3].bsearch { |x| x <=> -1 }.should be_nil

    end

    it "returns nil if the block never returns zero" do
      [0, 1, 3, 4].bsearch { |x| x <=> 2 }.should be_nil
    end

    it "returns an element at an index for which block returns true" do
      result = [0, 1, 2, 3, 4].bsearch { |x| x < 1 ? 1 : x > 3 ? -1 : 0 }
      [1, 2].should include(result)
    end
  end
end
