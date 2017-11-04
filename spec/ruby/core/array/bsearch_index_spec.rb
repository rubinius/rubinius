require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../../enumerable/shared/enumeratorized', __FILE__)

describe "Array#bsearch_index" do
  it "returns an Enumerator when not passed a block" do
    [1].bsearch_index.should be_an_instance_of(Enumerator)
  end

  it_behaves_like :enumeratorized_with_unknown_size, :bsearch_index, [1, 2, 3]

  it "raises a TypeError if the block returns an Object" do
    lambda { [1].bsearch_index { Object.new } }.should raise_error(TypeError)
  end

  it "raises a TypeError if the block returns a String" do
    lambda { [1].bsearch_index { "1" } }.should raise_error(TypeError)
  end

  context "with a block returning true or false" do
    it "returns nil if the block returns false for every element" do
      [0, 1, 2, 3].bsearch_index { |x| x > 3 }.should be_nil
    end

    it "returns nil if the block returns nil for every element" do
      [0, 1, 2, 3].bsearch_index { |x| nil }.should be_nil
    end

    it "returns zero if the block returns true for every element" do
      [0, 1, 2, 3].bsearch_index { |x| x < 4 }.should == 0
    end

    it "returns the smallest index for which block returns true" do
      [0, 1, 3, 4].bsearch_index { |x| x >= 2 }.should == 2
      [0, 1, 3, 4].bsearch_index { |x| x >= 1 }.should == 1
    end
  end

  context "with a block returning negative, zero, positive numbers" do
    it "returns nil if the block returns less than zero for every element" do
      [0, 1, 2, 3].bsearch_index { |x| x <=> 5 }.should be_nil
    end

    it "returns nil if the block returns greater than zero for every element" do
      [0, 1, 2, 3].bsearch_index { |x| x <=> -1 }.should be_nil
    end

    it "returns nil if the block never returns zero" do
      [0, 1, 3, 4].bsearch_index { |x| x <=> 2 }.should be_nil
    end

    it "accepts (+/-)Float::INFINITY from the block" do
      [0, 1, 3, 4].bsearch_index { |x| Float::INFINITY }.should be_nil
      [0, 1, 3, 4].bsearch_index { |x| -Float::INFINITY }.should be_nil
    end

    it "returns the index for which block returns 0.0" do
      result = [0, 1, 2, 3, 4].bsearch_index { |x| x < 2 ? 1.0 : x > 2 ? -1.0 : 0.0 }
      result.should == 2
    end

    it "returns the index for which block returns 0" do
      result = [0, 1, 2, 3, 4].bsearch_index { |x| x < 1 ? 1 : x > 3 ? -1 : 0 }
      result.should == 2
    end
  end
end
