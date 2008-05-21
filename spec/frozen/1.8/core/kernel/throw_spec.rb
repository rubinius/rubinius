require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#throw" do
  it "is a private method" do
    Kernel.private_instance_methods.should include("throw")
  end
end

describe "Kernel.throw" do
  it "needs to be reviewed for spec completeness" do
  end
end
