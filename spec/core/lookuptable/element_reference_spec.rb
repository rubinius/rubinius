require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::LookupTable#[]" do
  before :each do
    @lt = Rubinius::LookupTable.new(:a => 1, :b => 2, :c => 3)
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
end
