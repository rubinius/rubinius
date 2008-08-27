require File.dirname(__FILE__) + '/../../spec_helper'

describe "Hash#entry" do
  before :each do
    @hash = Hash.allocate
  end

  it "returns an empty bucket for key hash if no bucket existed" do
    key = mock("key")
    key_hash = @hash.key_hash key
    b = @hash.entry key, key_hash
    b.key.should == key
    b.key_hash.should == key_hash
    b.value.should be_nil
    b.next.should be_nil
  end

  it "increments the size of the hash when adding a bucket" do
    key = mock("key")
    key_hash = @hash.key_hash key
    b = @hash.entry key, key_hash
    b.key.should == key
    @hash.size.should == 1
  end

  it "returns an existing bucket that matches key and key_hash" do
    @hash.store :key, "key"
    hsh = @hash.key_hash :key
    bin = @hash.entry_bin hsh
    @hash.bins[bin] = Hash::Bucket.new :key, "key", hsh
    entry = @hash.entry :key, hsh
    entry.value.should == "key"
  end
end
