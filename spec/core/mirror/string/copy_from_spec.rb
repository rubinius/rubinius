require File.expand_path('../../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/common', __FILE__)

describe "Rubinius::Mirror::String#copy_from" do
  it "copies other into self" do
    a = string_mirror "abcdefghijkl"
    b = "ihgfed"
    a.copy_from b, 1, 4, 4
    a.object.should == "abcdhgfeijkl"
  end

  it "does not copy more characters than other contains" do
    a = string_mirror "xxxxxx"
    b = "yy"
    a.copy_from b, 0, 3, 2
    a.object.should == "xxyyxx"
  end

  it "sets the start index to 0 if it is less than 0" do
    a = string_mirror "xxx"
    b = "yy"
    a.copy_from b, -1, 2, 1
    a.object.should == "xyy"
  end

  it "does not copy any characters if start index is greater than the size of other" do
    a = string_mirror "xxx"
    b = "yy"
    a.copy_from b, 2, 2, 0
    a.object.should == "xxx"
  end

  it "does not copy beyond the end of self" do
    a = string_mirror "xx"
    b = "yyyy"
    a.copy_from b, 0, 4, 0
    a.object.should == "yy"
  end

  it "returns self unchanged if the destination index is greater than the size of self" do
    a = string_mirror "xx"
    b = "yy"
    a.copy_from b, 0, 2, 2
    a.object.should == "xx"
  end

  it "sets the destination index to 0 if it is less than 0" do
    a = string_mirror "xx"
    b = "yy"
    a.copy_from b, 0, 2, -1
    a.object.should == "yy"
  end
end
