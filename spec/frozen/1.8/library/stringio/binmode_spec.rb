require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "StringIO#binmode" do
  it "returns self" do
    io = StringIO.new("example")
    io.binmode.should equal(io)
  end
end
