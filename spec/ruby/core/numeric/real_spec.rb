require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../../../shared/complex/numeric/real', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

ruby_version_is "1.9" do
  describe "Numeric#real" do
    it_behaves_like(:numeric_real, :real)
  end
end

ruby_version_is "1.9" do
  describe "Numeric#real?" do
    it "returns true" do
      NumericSpecs::Subclass.new.real?.should == true
    end
  end
end
