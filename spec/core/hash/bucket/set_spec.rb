require File.dirname(__FILE__) + '/../../../spec_helper'

describe "Hash::Bucket#set" do
  before :each do
    @a = Hash::Bucket.new 1, 2, 3
    @b = Hash::Bucket.new 4, 5, 6
    @c = Hash::Bucket.new 7, 8, 9

    @a.next = @b
    @b.next = @c

  end

  it "updates the value of an existing bucket if key and key_hash match" do
    @a.set(1, 5, 3)
    @a.value.should == 5

    @a.set(7, 10, 9)
    @c.value.should == 10
  end

  it "appends a new bucket if no existing bucket in the chain matches key and key_hash" do
    b = @a.set(10, 11, 12)
    @c.next.should == b
    b.key.should == 10
    b.value.should == 11
    b.key_hash.should == 12
    b.next.should be_nil
  end
end
