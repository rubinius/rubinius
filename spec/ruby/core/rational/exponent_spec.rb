ruby_version_is "1.9" do
  require File.expand_path('../../../shared/rational/exponent', __FILE__)

  describe "Rational#**" do
    it_behaves_like(:rational_exponent, :**)
  end

  describe "Rational#** when passed Rational" do
    it_behaves_like(:rational_exponent_rat, :**)
  end

  describe "Rational#** when passed Float" do
    it_behaves_like(:rational_exponent_float, :**)
  end

  describe "Rational#** when pass Integer" do
    it_behaves_like(:rational_exponent_int, :**)
  end
end
