require File.dirname(__FILE__) + '/../../shared/rational/denominator'

ruby_version_is ""..."1.9" do
  describe "Rational#denominator" do
    it_behaves_like(:rational_denominator, :denominator)
  end
end
