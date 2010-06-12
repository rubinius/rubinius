require File.expand_path('../../../spec_helper', __FILE__)

describe "String#compare_substring" do
  it "returns -1 if self is less than a substring of other" do
    "abc".compare_substring("defghi", -6, 3).should == -1
    "def".compare_substring("aghib", 1, 3).should == -1
  end

  it "return 0 if self is equal to a substring of other" do
    "abc".compare_substring("ababcba", 2, 2).should == 0
    "bbc".compare_substring("abbc", -3, 3).should == 0
  end

  it "returns 1 if self is greater than a substring of other" do
    "def".compare_substring("xyzabc", -3, 3).should == 1
    "qrs".compare_substring("abc", 0, 2).should == 1
  end

  it "does not compare more characters than size of self" do
    "aaa".compare_substring("aaabbb", 0, 5).should == 0
  end

  it "does not compare beyond the end of other" do
    "aaaa".compare_substring("aaa", 1, 3).should == 0
  end

  it "raises Rubinius::ObjectBoundsExceededError if start is beyond the bounds of other" do
    lambda {
      "a".compare_substring("aa", -3, 1)
    }.should raise_error(Rubinius::ObjectBoundsExceededError)
    lambda {
      "a".compare_substring("aa", 2, 1)
    }.should raise_error(Rubinius::ObjectBoundsExceededError)
  end
end
