require File.dirname(__FILE__) + '/../../spec_helper'

describe "LookupTable.allocate" do
  it "creates an instance of LookupTable" do
    LookupTable.allocate.should be_kind_of(LookupTable)
  end

  it "creates a fully-formed LookupTable instance" do
    lt = LookupTable.allocate
    lt.should be_kind_of(LookupTable)
    lt.size.should == 0
    lt[:a] = 1
    lt[:a].should == 1
    lt.size.should == 1
  end
end
