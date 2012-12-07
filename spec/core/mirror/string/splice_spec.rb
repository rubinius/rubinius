require File.expand_path('../../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/common', __FILE__)

# The Rubinius::Mirror::String#splice method is a helper method. Because of
# the semantics of Ruby methods that use this method, bounds checking and data
# conversions need to happen before this method is called. Hence, to improve
# the performance of this method, its API expects valid in-range values and
# its behavior is UNDEFINED for values out-of-range. The underlying primitive
# that manipulates vectors of bytes is boundary safe (as all such primitives
# must be).

describe "Rubinius::Mirror::String#splice" do
  it "replaces zero characters an the beginning of the String" do
    string_mirror("abc").splice(0, 0, "").object.should == "abc"
  end

  it "replaces zero characters within the String" do
    string_mirror("abc").splice(1, 0, "").object.should == "abc"
  end

  it "replaces zero characters at the end of the String" do
    string_mirror("abc").splice(3, 0, "").object.should == "abc"
  end

  it "replaces characters at the beginning of the String" do
    string_mirror("abc").splice(0, 2, "xyz").object.should == "xyzc"
  end

  it "replaces characters within the String" do
    string_mirror("abcde").splice(1, 2, "xyz").object.should == "axyzde"
  end

  it "replaces characters at the end of the String" do
    string_mirror("abc").splice(2, 1, "xyz").object.should == "abxyz"
  end

  it "inserts characters an the beginning of the String" do
    string_mirror("abc").splice(0, 0, "x").object.should == "xabc"
  end

  it "inserts characters within the String" do
    string_mirror("abc").splice(1, 0, "x").object.should == "axbc"
  end

  it "inserts characters at the end of the String" do
    string_mirror("abc").splice(3, 0, "x").object.should == "abcx"
  end

  it "inserts characters after the end of the String" do
    string_mirror("abc").splice(3, 0, "xyz").object.should == "abcxyz"
  end

  it "removes characters at the beginning of the String" do
    string_mirror("abc").splice(0, 1, "").object.should == "bc"
  end

  it "removes characters within the String" do
    string_mirror("abcde").splice(2, 2, "").object.should == "abe"
  end

  it "removes characters at the end of the String" do
    string_mirror("abcde").splice(3, 2, "").object.should == "abc"
  end
end
