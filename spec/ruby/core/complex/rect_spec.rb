ruby_version_is "1.9" do
  require File.expand_path('../../../shared/complex/rect', __FILE__)

  describe "Complex#rect" do
    it_behaves_like(:complex_rect, :rect)
  end

  describe "Complex.rect" do
    it "needs specs"
  end
end
