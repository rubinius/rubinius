require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/arg.rb'

describe "Complex#arg" do
  it_behaves_like(:complex_arg, :arg)
end
