require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::LookupTable#[]=" do
  before :each do
    @lt = Rubinius::LookupTable.new
  end

  it "adds an entry for the given key, value" do
    @lt[:a] = 1
    @lt[:b] = "2"
    
    @lt[:a].should == 1
    @lt[:b].should == "2"
  end

  it "converts a String key argument to a Symbol" do
    @lt["c"] = 3
    @lt["c"].should == 3
  end
end
