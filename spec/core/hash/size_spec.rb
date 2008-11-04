require File.dirname(__FILE__) + '/../../spec_helper'

describe "Hash#count" do
  it "returns the number of pairs in the Hash" do
    hash = Hash.allocate
    hash.entry :key, hash.key_hash(:key)
    hash.count.should == hash.instance_variable_get(:@count)
  end
end

describe "Hash#count=" do
  before :each do
    @hash = Hash.allocate
  end

  it "sets the number of pairs in the hash" do
    @hash.count = 1
    @hash.instance_variable_get(:@count).should == 1
  end

  it "calls #redistribute" do
    @hash.should_receive(:redistribute)
    @hash.count = 1
  end
end
