require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#syscall" do
  it "is a private method" do
    Kernel.private_instance_methods.should include("syscall")
  end
end

describe "Kernel.syscall" do
  it "needs to be reviewed for spec completeness"
end
