require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "StringIO#path" do
  it "returns nil" do
    StringIO.new("path").path.should be_nil
  end
end
