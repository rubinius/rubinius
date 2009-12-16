ruby_version_is "1.9" do
  
  require File.dirname(__FILE__) + '/../../shared/complex/coerce'
  
  describe "Complex#coerce" do
    it_behaves_like(:complex_coerce, :coerce)
  end
end
