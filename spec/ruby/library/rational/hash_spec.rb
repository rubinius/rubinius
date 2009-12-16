require File.dirname(__FILE__) + '/../../shared/rational/hash'

ruby_version_is ""..."1.9" do
  describe "Rational#hash" do
    it_behaves_like(:rational_hash, :hash)
  end
end
