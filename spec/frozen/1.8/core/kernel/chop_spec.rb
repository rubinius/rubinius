require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#chop" do
  it "is a private method" do
    Kernel.private_instance_methods.should include("chop")
  end
end

describe "Kernel#chop!" do
  it "is a private method" do
    Kernel.private_instance_methods.should include("chop!")
  end
end

describe "Kernel.chop" do
  it "needs to be reviewed for spec completeness"
end

describe "Kernel.chop!" do
  it "needs to be reviewed for spec completeness"
end
