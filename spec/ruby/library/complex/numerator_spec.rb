require File.dirname(__FILE__) + '/../../shared/complex/numerator'

ruby_version_is ""..."1.9" do
  
  require 'complex'
  # Depends on Rational to be loaded.
  require "rational"

  describe "Complex#numerator" do
    it_behaves_like(:complex_numerator, :numerator)
  end
end
