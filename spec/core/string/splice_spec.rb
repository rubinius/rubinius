require File.expand_path('../../../spec_helper', __FILE__)

describe "String#splice!" do
  it "replaces count characters starting at index with the replacement string" do
    "aaa".splice!(1, 1, "bbb").should == "abbba"
    "a".splice!(0, 1, "xxx").should == "xxx"
    "aaa".splice!(3, 0, "xxx").should == "aaaxxx"
    "aaa".splice!(3, 1, "xxx").should == "aaaxxx"
    "aaa".splice!(3, 2, "xxx").should == "aaaxxx"
    "aaaa".splice!(2, 0, "bb").should == "aabbaa"
  end

  it "removes a section of string when passed an empty replacement string" do
    "aaa".splice!(1, 1, "").should == "aa"
    "aaa".splice!(0, 2, "").should == "a"
    "aaa".splice!(0, 3, "").should == ""
  end

  it "accepts a negative start index" do
    "aaaa".splice!(-2, 2, "yy").should == "aayy"
    "aaa".splice!(-3, 2, "yy").should == "yya"
    "aaa".splice!(-3, 3, "xx").should == "xx"
  end

  it "raises IndexError if start is outside the bounds of the string" do
    lambda { "aaa".splice!(-4, 2, "yy") }.should raise_error(IndexError)
    lambda { "aaa".splice!(4, 1, "yy") }.should raise_error(IndexError)
  end

  it "replaces to the end of string if start + count is greater than the size of string" do
    "aaa".splice!(1, 5, "xx").should == "axx"
    "aaa".splice!(0, 5, "").should == ""
  end

  it "raises IndexError if count is negative" do
    lambda { "aaa".splice!(1, -3, "bb") }.should raise_error(IndexError)
  end
end
