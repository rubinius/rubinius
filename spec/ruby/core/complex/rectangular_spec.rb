require File.expand_path('../../../spec_helper', __FILE__)

ruby_version_is "1.9" do

  require File.expand_path('../../../shared/complex/rect', __FILE__)

  describe "Complex#rectangular" do
    it_behaves_like(:complex_rect, :rectangular)
  end

  describe "Complex.rectangular" do
    it "needs specs"
  end
end
