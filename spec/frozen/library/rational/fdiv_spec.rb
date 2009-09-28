require File.dirname(__FILE__) + '/../../shared/rational/fdiv'

ruby_version_is ""..."1.9" do
  describe "Rational#fdiv" do
    it_behaves_like(:rational_fdiv, :fdiv)
  end
end
