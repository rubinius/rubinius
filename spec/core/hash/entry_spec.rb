require File.dirname(__FILE__) + '/../../spec_helper'

describe "Hash#entry" do
  before :each do
    @hash = Hash.new
  end

  it "returns an empty bucket for key hash" do
    key = mock("key")
    key_hash = @hash.key_hash key
    b = @hash.entry key, key_hash
    b.key.should == key
    b.key_hash.should == key_hash
    b.value.should be_nil
    b.next.should be_nil
  end
end
