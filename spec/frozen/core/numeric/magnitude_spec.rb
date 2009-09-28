require File.dirname(__FILE__) + '/shared/abs'

ruby_version_is "1.9" do
  describe "Numeric#magnitude" do
    it_behaves_like(:numeric_abs, :magnitude)
  end
end
