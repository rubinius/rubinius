require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/complex/numeric/conj'

ruby_version_is "1.9" do
  describe "Numeric#conjugate" do
    it_behaves_like(:numeric_conj, :conjugate)
  end
end
