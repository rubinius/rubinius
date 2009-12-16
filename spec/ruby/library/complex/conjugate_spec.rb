require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/complex/conjugate'

ruby_version_is ""..."1.9" do
  describe "Complex#conjugate" do
    it_behaves_like(:complex_conjugate, :conjugate)
  end
end
