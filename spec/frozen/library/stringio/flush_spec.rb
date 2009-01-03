require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "StringIO#flush" do
  it "returns self" do
    io = StringIO.new("flush")
    io.flush.should equal(io)
  end
end
