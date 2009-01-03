require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#trap" do
  it "is a private method" do
    Kernel.private_instance_methods.should include("trap")
  end
end

describe "Kernel.trap" do
  it "needs to be reviewed for spec completeness"
end
