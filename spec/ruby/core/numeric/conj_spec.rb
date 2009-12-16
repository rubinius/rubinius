require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/complex/numeric/conj'

ruby_version_is "1.9" do
  describe "Numeric#conj" do
    it_behaves_like(:numeric_conj, :conj)
  end
end
