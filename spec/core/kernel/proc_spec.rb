require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/lambda_spec'

describe "Kernel.proc" do
  it_behaves_like(@kernel_lambda, :proc)
end
