require File.dirname(__FILE__) + '/../../shared/complex/polar'

ruby_version_is "1.9" do

  describe "Complex.polar" do
    it_behaves_like(:complex_polar, :polar)
  end

  describe "Complex#polar" do
    it_behaves_like(:complex_polar, :polar)
  end
end
