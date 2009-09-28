require File.dirname(__FILE__) + '/../../shared/rational/divide'

ruby_version_is ""..."1.9" do
  describe "Rational#quo" do
    it_behaves_like(:rational_divide, :quo)
  end
end
