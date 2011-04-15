ruby_version_is "1.9" do

  require File.expand_path('../../../shared/complex/exponent', __FILE__)

  describe "Complex#** when given 0" do
    it_behaves_like(:complex_exponent_zero, :**)
  end

  describe "Complex#** with Complex" do
    it_behaves_like(:complex_exponent_complex, :**)
  end

  describe "Complex#** with Integer" do
    it_behaves_like(:complex_exponent_integer, :**)
  end

  describe "Complex#** with Rational" do
    it_behaves_like(:complex_exponent_rational, :**)
  end

  describe "Complex#** with Object" do
    it_behaves_like(:complex_exponent_object, :**)
  end
end
