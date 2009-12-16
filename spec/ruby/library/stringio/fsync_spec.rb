require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "StringIO#fsync" do
  it "returns zero" do
    io = StringIO.new("fsync")
    io.fsync.should eql(0)
  end
end
