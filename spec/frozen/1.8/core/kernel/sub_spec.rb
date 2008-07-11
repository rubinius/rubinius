require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#sub" do
  it "is a private method" do
    Kernel.private_instance_methods.should include("sub")
  end
end

describe "Kernel#sub!" do
  it "is a private method" do
    Kernel.private_instance_methods.should include("sub!")
  end
end

describe "Kernel.sub" do
  it "needs to be reviewed for spec completeness"
end

describe "Kernel.sub!" do
  it "needs to be reviewed for spec completeness"
end
