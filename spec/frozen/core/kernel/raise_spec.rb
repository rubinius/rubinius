require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#raise" do
  it "is a private method" do
    Kernel.private_instance_methods.should include("raise")
  end
end

describe "Kernel.raise" do
  it "needs to be reviewed for spec completeness"
end
