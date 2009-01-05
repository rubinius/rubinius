require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/../../shared/kernel/raise'

describe "Kernel#raise" do
  it "is a private method" do
    Kernel.private_instance_methods.should include("raise")
  end
end

describe "Kernel#raise" do
  it_behaves_like :kernel_raise, :raise, Kernel
end

describe "Kernel.raise" do
  it "needs to be reviewed for spec completeness"
end
