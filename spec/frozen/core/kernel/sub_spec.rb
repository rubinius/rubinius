require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#sub" do
  it "is a private method" do
    Kernel.should have_private_instance_method(:sub)
  end
end

describe "Kernel#sub!" do
  it "is a private method" do
    Kernel.should have_private_instance_method(:sub!)
  end
end

describe "Kernel.sub" do
  it "needs to be reviewed for spec completeness"
end

describe "Kernel.sub!" do
  it "needs to be reviewed for spec completeness"
end
