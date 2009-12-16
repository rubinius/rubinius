require File.dirname(__FILE__) + '/../../spec_helper'

ruby_version_is "1.9" do
  
  require File.dirname(__FILE__) + '/../../shared/complex/conjugate'
  
  describe "Complex#conj" do
    it_behaves_like(:complex_conjugate, :conj)
  end
end
