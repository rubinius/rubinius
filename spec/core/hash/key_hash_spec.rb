require File.dirname(__FILE__) + '/../../spec_helper'

describe "Hash#key_hash" do
  it "returns a hash value for a key object" do
    Hash.new.key_hash("key").should == "key".hash
  end

  it "ensures the hash value is a Fixnum" do
    o = mock("big hash")
    def o.hash; bignum_value; end

    h = Hash.new
    h.key_hash(o).should be_kind_of(Fixnum)
  end
end
