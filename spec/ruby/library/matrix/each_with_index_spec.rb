require File.expand_path('../../../spec_helper', __FILE__)
require 'matrix'

ruby_version_is "1.9" do
  describe "Matrix#each_with_index" do
    before :all do
      @m = Matrix[ [1, 2, 3], [4, 5, 6] ]
      @result = [
        [1, 0, 0],
        [2, 0, 1],
        [3, 0, 2],
        [4, 1, 0],
        [5, 1, 1],
        [6, 1, 2]
      ]
    end

    it "returns an Enumerator when called without a block" do
      enum = @m.each_with_index
      enum.should be_an_instance_of(enumerator_class)
      enum.to_a.should == @result
    end

    it "returns self" do
      @m.each_with_index{}.should equal(@m)
    end

    it "yields the elements starting with the those of the first row" do
      a = []
      @m.each_with_index {|x, r, c| a << [x, r, c]}
      a.should == @result
    end
  end
end