require File.dirname(__FILE__) + '/../../../spec_helper'

describe "Hash::Bucket#delete" do
  before :each do
    @a = Hash::Bucket.new 1, 2, 3
    @b = Hash::Bucket.new 4, 5, 6
    @c = Hash::Bucket.new 7, 8, 9
    @d = Hash::Bucket.new 0, 1, 2

    @a.next = @b
    @b.next = @c

  end

  it "returns false and removes the bucket from the end of the chain" do
    @a.delete(7, 9).should == [@c, false]
    @b.next.should be_nil
  end

  it "returns false and removes the bucket from the middle of the chain" do
    @a.delete(4, 6).should == [@b, false]
    @a.next.should == @c
  end

  it "returns nil if the entire chain should be removed" do
    @d.delete(0, 2).should == [@d, nil]
  end

  it "returns true if no bucket matches key and key_hash" do
    @a.delete(0, 2).should == [nil, true]
  end
end
