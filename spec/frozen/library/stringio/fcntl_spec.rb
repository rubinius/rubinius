require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "StringIO#fcntl" do
  it "raises a NotImplementedError" do
    lambda { StringIO.new("boom").fcntl }.should raise_error(NotImplementedError)
  end
end
