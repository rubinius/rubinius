require File.dirname(__FILE__) + '/../../spec_helper'

ruby_version_is "1.9" do
  require File.dirname(__FILE__) + '/../../shared/enumerator/with_index'

  describe "Enumerator#with_index" do
    it_behaves_like(:enum_with_index, :with_index)

    it "accepts an optional argument when given a block" do
      lambda do
        @enum.with_index(1) { |f| f}
      end.should_not raise_error(ArgumentError)
    end

    it "accepts an optional argument when not given a block" do
      lambda do
        @enum.with_index(1)
      end.should_not raise_error(ArgumentError)
    end

    it "numbers indices from the given index when given an offset but no block" do
      @enum.with_index(1).to_a.should == [[1,1],[2,2],[3,3],[4,4]]
    end

    it "numbers indices from the given index when given an offset and block" do
      acc = []
      @enum.with_index(1) {|e,i| acc << [e,i] }
      acc.should == [[1,1],[2,2],[3,3],[4,4]]
    end
  end
end
