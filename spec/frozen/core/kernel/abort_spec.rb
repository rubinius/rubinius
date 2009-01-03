require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#abort" do
  it "is a private method" do
    Kernel.private_instance_methods.should include("abort")
  end
end

describe "Kernel.abort" do
  it "needs to be reviewed for spec completeness"
end
