ruby_version_is "1.9" do
  require File.expand_path('../../../shared/rational/new', __FILE__)

  describe "Rational.new" do
    it_behaves_like(:rational_new, :new)
  end
end
