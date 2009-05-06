require File.dirname(__FILE__) + '/../../spec_helper'

describe "Hash.allocate" do
  it "returns an instance of Hash" do
    hsh = hash_class.allocate
    hsh.should be_kind_of(Hash)
  end
  
  it "returns a fully-formed instance of Hash" do
    hsh = hash_class.allocate
    hsh.size.should == 0
    hsh[:a] = 1
    hsh.should == { :a => 1 }
  end
end
