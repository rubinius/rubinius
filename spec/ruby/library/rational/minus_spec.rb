require File.expand_path('../../../shared/rational/minus', __FILE__)
require 'rational'

ruby_version_is ""..."1.9" do
  describe "Rational#-" do
    it_behaves_like(:rational_minus, :-)
  end
end
