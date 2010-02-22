ruby_version_is "1.9" do
  require File.expand_path('../../../shared/rational/Rational', __FILE__)

  describe "Rational() passed two arguments" do
    it_behaves_like(:rational_rational_two, :Rational)
  end

  describe "Rational() passed Integer" do
    it_behaves_like(:rational_rational_int, :Rational)
  end
end
