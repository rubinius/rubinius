require File.expand_path('../../../spec_helper', __FILE__)

describe "Hash.allocate" do
  it "returns an instance of Hash" do
    hsh = hash_class.allocate
    hsh.should be_kind_of(hash_class)
  end

  it "returns a fully-formed instance of Hash" do
    hsh = hash_class.allocate
    hsh.size.should == 0
    hsh[:a] = 1
    hsh.should == new_hash({ :a => 1 })
  end
end
