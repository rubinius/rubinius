require File.expand_path('../../../spec_helper', __FILE__)

ruby_version_is "1.9" do
  describe "Array#drop" do
    it "removes the specified number of elements from the start of the array" do
      [1, 2, 3, 4, 5].drop(2).should == [3, 4, 5]
    end

    it "raises an exception if the number of elements specified is negative" do
     lambda { [1, 2].drop(-3) }.should raise_error(ArgumentError)
    end

    it "returns an empty array if all elements are dropped" do
      [1, 2].drop(2).should == []
    end

    it "returns an empty array if more elements than exist are dropped" do
      [1, 2].drop(3).should == []
    end
  end
end
