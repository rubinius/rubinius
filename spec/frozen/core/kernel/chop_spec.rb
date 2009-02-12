require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#chop" do
  it "is a private method" do
    Kernel.should have_private_instance_method(:chop)
  end
end

describe "Kernel#chop!" do
  it "is a private method" do
    Kernel.should have_private_instance_method(:chop!)
  end
end

describe "Kernel.chop" do
  it "needs to be reviewed for spec completeness"
end

describe "Kernel.chop!" do
  it "needs to be reviewed for spec completeness"
end
