require File.dirname(__FILE__) + '/../../spec_helper'

describe "LookupTable#[]" do
  before :each do
    @lt = LookupTable.new(:a => 1, :b => 2, :c => 3)
  end

  it "returns the entry for the key" do
    @lt[:a].should == 1
    @lt[:b].should == 2
  end

  it "returns nil if the key is not found" do
    @lt[:d].should == nil
  end

  it "converts a String key argument to a Symbol" do
    @lt["c"].should == 3
  end

  it "raises a TypeError if passed other than a Symbol or String" do
    key = mock("lookuptable key")
    lambda { @lt[key] }.should raise_error(TypeError)
  end
end
