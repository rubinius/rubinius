require File.expand_path('../../../shared/rational/new', __FILE__)

ruby_version_is ""..."1.9" do
  describe "Rational.new" do
    it_behaves_like(:rational_new, :new)
  end

  describe "Rational.new!" do
    it_behaves_like(:rational_new, :new!)
  end
end
