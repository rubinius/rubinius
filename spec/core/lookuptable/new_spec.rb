require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::LookupTable.new" do
  it "creates a LookupTable instance" do
    Rubinius::LookupTable.new.should be_kind_of(Rubinius::LookupTable)
  end

  it "creates a LookupTable instance from hash arguments" do
    lt = Rubinius::LookupTable.new(:a => 1, :b => 2)
    lt.should be_kind_of(Rubinius::LookupTable)
    lt.size.should == 2
    lt[:a].should == 1
    lt[:b].should == 2
  end
end
