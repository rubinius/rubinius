require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/complex/Complex'

ruby_version_is ""..."1.9" do

  require 'complex'

  describe "Complex when passed [Complex, Complex]" do
    it_behaves_like(:complex_Complex_complex_complex, :Complex)
  end

  describe "Complex when passed [Complex]" do
    it_behaves_like(:complex_Complex_complex, :Complex)
  end

  describe "Complex when passed [Integer, Integer]" do
    it_behaves_like(:complex_Complex_integer_integer, :Complex)
  end

  describe "Complex when passed [Integer]" do
    it_behaves_like(:complex_Complex_integer, :Complex)
  end
end
