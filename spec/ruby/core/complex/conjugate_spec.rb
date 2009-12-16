require File.dirname(__FILE__) + '/../../spec_helper'

ruby_version_is "1.9" do
  
  require File.dirname(__FILE__) + '/../../shared/complex/conjugate'
  
  describe "Complex#conjugate" do
    it_behaves_like(:complex_conjugate, :conjugate)
  end
end
