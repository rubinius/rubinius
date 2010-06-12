require File.expand_path('../../../spec_helper', __FILE__)

describe "String#copy_from" do
  it "copies other into self" do
    a = "abcdefghijkl"
    b = "ihgfed"
    a.copy_from b, 1, 4, 4
    a.should == "abcdhgfeijkl"
  end

  it "does not copy more characters than other contains" do
    a = "xxxxxx"
    b = "yy"
    a.copy_from b, 0, 3, 2
    a.should == "xxyyxx"
  end

  it "sets the start index to 0 if it is less than 0" do
    a = "xxx"
    b = "yy"
    a.copy_from b, -1, 2, 1
    a.should == "xyy"
  end

  it "does not copy any characters if start index is greater than the size of other" do
    a = "xxx"
    b = "yy"
    a.copy_from b, 2, 2, 0
    a.should == "xxx"
  end

  it "does not copy beyond the end of self" do
    a = "xx"
    b = "yyyy"
    a.copy_from b, 0, 4, 0
    a.should == "yy"
  end

  it "returns self unchanged if the destination index is greater than the size of self" do
    a = "xx"
    b = "yy"
    a.copy_from b, 0, 2, 2
    a.should == "xx"
  end

  it "sets the destination index to 0 if it is less than 0" do
    a = "xx"
    b = "yy"
    a.copy_from b, 0, 2, -1
    a.should == "yy"
  end
end
