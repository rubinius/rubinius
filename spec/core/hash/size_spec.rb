require File.dirname(__FILE__) + '/../../spec_helper'

describe "Hash#size" do
  it "returns the number of pairs in the Hash" do
    hash = Hash.allocate
    hash.entry :key, hash.key_hash(:key)
    hash.size.should == hash.instance_variable_get(:@size)
  end
end

describe "Hash#size=" do
  before :each do
    @hash = Hash.allocate
  end

  it "sets the number of pairs in the hash" do
    @hash.size = 1
    @hash.instance_variable_get(:@size).should == 1
  end

  it "calls #redistribute" do
    @hash.should_receive(:redistribute)
    @hash.size = 1
  end
end
