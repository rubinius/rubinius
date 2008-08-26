require File.dirname(__FILE__) + '/../../../spec_helper'

describe "Hash::Bucket#delete" do
  before :each do
    @a = Hash::Bucket.new 1, 2, 3
    @b = Hash::Bucket.new 4, 5, 6
    @c = Hash::Bucket.new 7, 8, 9

    @a.next = @b
    @b.next = @c

  end

  it "returns false and removes the bucket from the end of the chain" do
    @a.delete(4, 6).should be_false
    @a.next.should == @c
  end

  it "returns false and removes the bucket from the middle of the chain" do
    @a.delete(7, 9).should be_false
    @b.next.should be_nil
  end

  it "returns true if the entire chain should be removed" do
    @c.delete(7, 9, nil).should be_true
  end
end
