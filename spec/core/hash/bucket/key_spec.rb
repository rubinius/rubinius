require File.dirname(__FILE__) + '/../../../spec_helper'

describe "Hash::Bucket#key" do
  it "returns the key object" do
    Hash::Bucket.new(1, 2).key.should == 1
  end
end

describe "Hash::Bucket#key=" do
  it "sets the key object" do
    b = Hash::Bucket.new
    b.key = 2
    b.key.should == 2
  end
end
