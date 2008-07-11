require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#putc" do
  it "is a private method" do
    Kernel.private_instance_methods.should include("putc")
  end
end

describe "Kernel.putc" do
  it "needs to be reviewed for spec completeness"
end
