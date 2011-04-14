require File.expand_path('../../../spec_helper', __FILE__)

describe "String.new" do
  it "returns an instance of String" do
    str = String.new
    str.should be_kind_of(String)
  end

  it "returns a fully-formed String" do
    str = String.new
    str.size.should == 0
    str << "more"
    str.should == "more"
  end

  it "returns a new string given a string argument" do
    str1 = "test"
    str = String.new(str1)
    str.should be_kind_of(String)
    str.should == str
    str << "more"
    str.should == "testmore"
  end
end
