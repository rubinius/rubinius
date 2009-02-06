require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#exit" do
  it "is a private method" do
    Kernel.should have_private_instance_method(:exit)
  end
end

describe "Kernel#exit!" do
  it "is a private method" do
    Kernel.should have_private_instance_method(:exit!)
  end
end


describe "Kernel.exit" do
  it "needs to be reviewed for spec completeness"
end

describe "Kernel.exit!" do
  it "needs to be reviewed for spec completeness"
end
