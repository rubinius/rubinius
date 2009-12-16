require File.dirname(__FILE__) + '/../../shared/rational/to_i'

ruby_version_is ""..."1.9" do
  describe "Rational#to_i" do
    it_behaves_like(:rational_to_i, :to_i)
  end
end
