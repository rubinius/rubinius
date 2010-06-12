require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::LookupTable#entries" do
  before :each do
    @lt = Rubinius::LookupTable.new(:a => 1, :b => 2, :c => 3)
  end

  it "returns an Array of the entries in the LookupTable" do
    entries = @lt.entries.sort { |a, b| a.key.to_s <=> b.key.to_s }
    entries.collect { |e| e.key }.should == [:a, :b, :c]
    entries.collect { |e| e.value }.should == [1, 2, 3]
  end
end
