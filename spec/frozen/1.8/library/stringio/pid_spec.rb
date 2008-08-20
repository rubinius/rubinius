require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "StringIO#pid" do
  it "returns nil" do
    StringIO.new("pid").pid.should be_nil
  end
end
