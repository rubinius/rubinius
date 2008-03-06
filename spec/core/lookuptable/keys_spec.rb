require File.dirname(__FILE__) + '/../../spec_helper'

describe "LookupTable#keys" do
  before :each do
    @lt = LookupTable.new(:a => 1, :b => 2, :c => 3)
  end

  it "returns an Array of the keys in the LookupTable" do
    @lt.keys.sort { |a,b| a.to_s <=> b.to_s }.should == [:a, :b, :c]
  end
end
