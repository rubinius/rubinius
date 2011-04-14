require File.expand_path('../../../spec_helper', __FILE__)

describe "String.allocate" do
  it "returns an instance of String" do
    str = String.allocate
    str.should be_kind_of(String)
  end

  it "returns a fully-formed String" do
    str = String.allocate
    str.size.should == 0
    str << "more"
    str.should == "more"
  end
end
