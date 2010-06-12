require File.expand_path('../../../../spec_helper', __FILE__)

describe "Hash::Entry#key" do
  it "returns the key object" do
    Hash::Entry.new(1, 1.hash, 2).key.should == 1
  end
end

describe "Hash::Entry#key=" do
  it "sets the key object" do
    b = Hash::Entry.new 1, 2, 3
    b.key = 2
    b.key.should == 2
  end
end
