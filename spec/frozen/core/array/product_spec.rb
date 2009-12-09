require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#product" do
  ruby_version_is "1.8.7" do
    it "returns converted arguments using :to_ary" do
      lambda{ [1].product(2..3) }.should raise_error(TypeError)
      ar = ArraySpecs::ArrayConvertable.new(2,3)
      [1].product(ar).should == [[1,2],[1,3]]
      ar.called.should == :to_ary
    end

    it "returns the expected result" do
      [1,2].product([3,4,5],[6,8]).should == [[1, 3, 6], [1, 3, 8], [1, 4, 6], [1, 4, 8], [1, 5, 6], [1, 5, 8],
                                              [2, 3, 6], [2, 3, 8], [2, 4, 6], [2, 4, 8], [2, 5, 6], [2, 5, 8]]
    end

    it "has no required argument" do
      [1,2].product.should == [[1],[2]]
    end

    it "returns an empty array when the argument is an empty array" do
      [1, 2].product([]).should == []
    end
  end
end
