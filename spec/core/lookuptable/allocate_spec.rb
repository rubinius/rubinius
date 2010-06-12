require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::LookupTable.allocate" do
  it "creates an instance of LookupTable" do
    Rubinius::LookupTable.allocate.should be_kind_of(Rubinius::LookupTable)
  end

  it "creates a fully-formed LookupTable instance" do
    lt = Rubinius::LookupTable.allocate
    lt.should be_kind_of(Rubinius::LookupTable)
    lt.size.should == 0
    lt[:a] = 1
    lt[:a].should == 1
    lt.size.should == 1
  end
end
