require File.expand_path('../../../spec_helper', __FILE__)

describe "Array#new_range" do
  it "returns an Array of length specified by the second parameter" do
    [].new_range(0, 4).length.should == 4
  end

  it "returns an Array with a subsequence contained in the original Array" do
    [1, 2, 3, 4].new_range(1, 3).should == [2, 3, 4]
  end

  it "returns an Arary with a partial subsequence when the range exceeds the Array size" do
    ary = [1, 2, 3].new_range(1, 4)
    ary[0].should == 2
    ary[1].should == 3
  end

  it "initializes elements outside the original Array to nil" do
    [1, 2, 3].new_range(1, 4).should == [2, 3, nil, nil]
  end
end
