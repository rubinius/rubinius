require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#print" do
  it "is a private method" do
    Kernel.private_instance_methods.should include("print")
  end
end

describe "Kernel.print" do
  it "needs to be reviewed for spec completeness"
end
