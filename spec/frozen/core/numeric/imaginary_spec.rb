require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/complex/numeric/imag'

ruby_version_is "1.9" do
  describe "Numeric#imaginary" do
    it_behaves_like(:numeric_imag, :imaginary)
  end
end
