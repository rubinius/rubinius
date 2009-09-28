
ruby_version_is "1.9" do
  
  require File.dirname(__FILE__) + '/../../shared/complex/denominator'
  
  describe "Complex#denominator" do
    it_behaves_like(:complex_denominator, :denominator)
  end
end
