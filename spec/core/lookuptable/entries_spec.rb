require File.dirname(__FILE__) + '/../../spec_helper'

describe "LookupTable#entries" do
  before :each do
    @lt = LookupTable.new(:a => 1, :b => 2, :c => 3)
  end

  it "returns an Array of the entries in the LookupTable" do
    entries = @lt.entries.sort { |a, b| a[0].to_s <=> b[0].to_s }
    entries.collect { |e| e[0] }.should == [:a, :b, :c]
    entries.collect { |e| e[1] }.should == [1, 2, 3]
  end
end
