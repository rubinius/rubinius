require File.dirname(__FILE__) + '/../../../spec_helper'

describe "Hash::Bucket#value" do
  it "returns the value object" do
    Hash::Bucket.new(1, 2).value.should == 2
  end
end

describe "Hash::Bucket#value=" do
  it "sets the value object" do
    b = Hash::Bucket.new
    b.value = 3
    b.value.should == 3
  end
end
