require File.dirname(__FILE__) + '/../../shared/rational/minus'

ruby_version_is ""..."1.9" do
  describe "Rational#-" do
    it_behaves_like(:rational_minus, :-)
  end
end
