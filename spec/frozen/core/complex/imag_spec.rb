require File.dirname(__FILE__) + '/../../shared/complex/image'

ruby_version_is "1.9" do
  describe "Complex#imag" do
    it_behaves_like(:complex_image, :imag)
  end
end
