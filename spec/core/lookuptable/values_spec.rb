require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::LookupTable#values" do
  before :each do
    @lt = Rubinius::LookupTable.new(:a => 1, :b => 2, :c => 3)
  end

  it "returns an Array of the values in the LookupTable" do
    @lt.values.sort.should == [1, 2, 3]
  end
end
