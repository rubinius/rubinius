require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::LookupTable#keys" do
  before :each do
    @lt = Rubinius::LookupTable.new(:a => 1, :b => 2, :c => 3)
  end

  it "returns an Array of the keys in the LookupTable" do
    @lt.keys.sort { |a,b| a.to_s <=> b.to_s }.should == [:a, :b, :c]
  end
end
