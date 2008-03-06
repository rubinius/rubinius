require File.dirname(__FILE__) + '/../../spec_helper'

describe "LookupTable.new" do
  it "creates a LookupTable instance" do
    LookupTable.new.should be_kind_of(LookupTable)
  end

  it "creates a LookupTable instance from hash arguments" do
    lt = LookupTable.new(:a => 1, :b => 2)
    lt.should be_kind_of(LookupTable)
    lt.size.should == 2
    lt[:a].should == 1
    lt[:b].should == 2
  end
end
