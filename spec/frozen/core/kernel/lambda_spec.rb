require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/lambda'

describe "Kernel.lambda" do
  it "is a private method" do
    Kernel.should have_private_instance_method(:lambda)
  end

  it_behaves_like(:kernel_lambda, :lambda)
end

describe "Kernel#lambda" do
  it "needs to be reviewed for spec completeness"
end
