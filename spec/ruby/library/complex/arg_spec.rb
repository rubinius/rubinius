require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/complex/arg'

ruby_version_is ""..."1.9" do
  describe "Complex#arg" do
    it_behaves_like(:complex_arg, :arg)
  end
end
