require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#trace_var" do
  it "is a private method" do
    Kernel.private_instance_methods.should include("trace_var")
  end
end

describe "Kernel.trace_var" do
  it "needs to be reviewed for spec completeness"
end
