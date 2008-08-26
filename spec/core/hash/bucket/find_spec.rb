require File.dirname(__FILE__) + '/../../../spec_helper'

describe "Hassh::Bucket#find" do
  before :each do
    @a = Hash::Bucket.new 1, 2, 3
    @b = Hash::Bucket.new 4, 5, 6
    @c = Hash::Bucket.new 7, 8, 9

    @a.next = @b
    @b.next = @c

  end

  it "returns returns the bucket in this chain containing key and key_hash" do
    @a.find(4, 6).should == @b
  end

  it "returns nil if there exists no bucket containing key and key_hash" do
    @b.find(1, 3).should be_nil
  end
end
