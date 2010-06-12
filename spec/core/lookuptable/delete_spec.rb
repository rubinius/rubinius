require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::LookupTable#delete" do
  before :each do
    @lt = Rubinius::LookupTable.new(:a => 1, :b => 2, :c => 3)
  end

  it "removes and returns the entry if it exists" do
    @lt.size.should == 3
    @lt.delete(:c).should == 3
    @lt.size.should == 2
    @lt[:c].should == nil
  end

  it "returns nil if the entry does not exist" do
    @lt.delete(:d).should == nil
    @lt.size.should == 3
  end

  it "converts a String key argument to a Symbol" do
    @lt.delete("c").should == 3
    @lt.size.should == 2
    @lt[:c].should == nil
  end
end
