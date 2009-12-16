require File.dirname(__FILE__) + '/../../shared/rational/div'

ruby_version_is ""..."1.9" do
  describe "Rational#div" do
    it_behaves_like(:rational_div, :div)
  end

  describe "Rational#div passed a Rational" do
    it_behaves_like(:rational_div_rat, :div)
  end

  describe "Rational#div passed an Integer" do
    it_behaves_like(:rational_div_int, :div)
  end

  describe "Rational#div passed a Float" do
    it_behaves_like(:rational_div_float, :div)
  end
end
