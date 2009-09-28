require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/complex/arg'

ruby_version_is ""..."1.9" do
  describe "Complex#angle" do
    it_behaves_like(:complex_arg, :angle)
  end
end
