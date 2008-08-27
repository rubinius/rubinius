require File.dirname(__FILE__) + '/../../spec_helper'

describe "Hash#entry_bin" do
  it "returns a number between 0 and #records - 1" do
    h = Hash.allocate
    hsh = h.key_hash("key")
    bin = h.entry_bin hsh
    bin.should >= 0
    bin.should < h.records
  end
end
