require File.expand_path('../../../shared/rational/Rational', __FILE__)

ruby_version_is ""..."1.9" do
  describe "Rational()" do
    it_behaves_like(:Rational, :Rational)
  end
end
