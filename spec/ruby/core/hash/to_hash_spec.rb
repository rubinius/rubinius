require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Hash#to_hash" do
  it "returns self" do
    h = new_hash
    h.to_hash.should equal(h)
  end
end
