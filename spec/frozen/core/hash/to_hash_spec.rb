require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Hash#to_hash" do
  it "returns self" do
    h = {}
    h.to_hash.should equal(h)
  end
end
