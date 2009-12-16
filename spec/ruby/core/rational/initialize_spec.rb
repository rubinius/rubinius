require File.dirname(__FILE__) + '/../../shared/rational/initialize'

ruby_version_is "1.9" do
  describe "Rational#initialize" do
    it_behaves_like(:rational_initialize, :initialize)
  end
end  


