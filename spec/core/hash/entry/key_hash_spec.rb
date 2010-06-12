require File.expand_path('../../../../spec_helper', __FILE__)

describe "Hash::Entry#key_hash" do
  it "returns the key hash object" do
    Hash::Entry.new(1, 2, 33).key_hash.should == 2
  end
end

describe "Hash::Entry#key_hash=" do
  it "sets the value object" do
    b = Hash::Entry.new 1, 2, 3
    b.key_hash = 33
    b.key_hash.should == 33
  end
end
