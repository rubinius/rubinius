require File.dirname(__FILE__) + '/../../shared/rational/modulo'

ruby_version_is ""..."1.9" do
  describe "Rational#%" do
    it_behaves_like(:rational_modulo, :%)
  end
end
