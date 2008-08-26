require File.dirname(__FILE__) + '/../../../spec_helper'

describe "Hash::Bucket#key_hash" do
  it "returns the key hash object" do
    Hash::Bucket.new(1, 2, 33).key_hash.should == 33
  end
end

describe "Hash::Bucket#key_hash=" do
  it "sets the value object" do
    b = Hash::Bucket.new 1, 2, 33
    b.key_hash = 33
    b.key_hash.should == 33
  end
end
