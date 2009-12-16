require File.dirname(__FILE__) + '/../../shared/rational/Rational'

ruby_version_is ""..."1.9" do
  describe "Rational() passed Integer, Integer" do
    it_behaves_like(:rational_rational_int_int, :Rational)
  end

  describe "Rational() passed Integer" do
    it_behaves_like(:rational_rational_int, :Rational)
  end

  describe "Rational() passed Integer when Unify is defined" do
    it_behaves_like(:rational_rational_int_rat_unify, :Rational)
  end  
end
