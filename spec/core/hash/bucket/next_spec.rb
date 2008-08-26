require File.dirname(__FILE__) + '/../../../spec_helper'

describe "Hash::Bucket#next" do
  it "returns nil if there is no chained bucket" do
    Hash::Bucket.new(1, 2).next.should be_nil
  end

  it "returns the next bucket" do
    b = Hash::Bucket.new
    Hash::Bucket.new(1, 2, nil, b).next.should == b
  end
end
