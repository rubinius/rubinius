require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/lambda'

describe "Kernel.proc" do
  it "is a private method" do
    Kernel.should have_private_instance_method(:proc)
  end
  
  it_behaves_like(:kernel_lambda, :proc)
end

describe "Kernel#proc" do
  it "needs to be reviewed for spec completeness"
end
