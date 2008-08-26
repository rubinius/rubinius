require File.dirname(__FILE__) + '/../../../spec_helper'

describe "Hash::Bucket.new" do
  it "returns a nil bucket with no initializers" do
    b = Hash::Bucket.new
    b.key.should be_nil
    b.value.should be_nil
    b.key_hash.should be_nil
    b.next.should be_nil
  end

  it "accepts key object initializer" do
    b = Hash::Bucket.new(1)
    b.key.should == 1
    b.value.should be_nil
    b.key_hash.should be_nil
    b.next.should be_nil
  end

  it "accepts value object initializer" do
    b = Hash::Bucket.new(1, 2)
    b.key.should == 1
    b.value.should == 2
    b.key_hash.should be_nil
    b.next.should be_nil
  end

  it "accepts key_hash object initializer" do
    b = Hash::Bucket.new(1, 2, 3)
    b.key.should == 1
    b.value.should == 2
    b.key_hash.should == 3
    b.next.should be_nil
  end

  it "accepts next bucket initializer" do
    n = Hash::Bucket.new
    b = Hash::Bucket.new(1, 2, 3, n)
    b.key.should == 1
    b.value.should == 2
    b.key_hash.should == 3
    b.next.should == n
  end
end
