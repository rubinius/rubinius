require File.dirname(__FILE__) + '/../../shared/rational/numerator'

ruby_version_is "1.9" do
  describe "Rational#numerator" do
    it_behaves_like(:rational_numerator, :numerator)
  end
end
