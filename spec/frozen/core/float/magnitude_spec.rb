require File.dirname(__FILE__) + '/shared/abs'

ruby_version_is "1.9" do
  describe "Float#magnitude" do
    it_behaves_like(:float_abs, :magnitude)
  end
end
