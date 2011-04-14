ruby_version_is "1.9" do

  require File.expand_path('../../../shared/complex/equal_value', __FILE__)

  describe "Complex#== with Complex" do
    it_behaves_like(:complex_equal_value_complex, :shared => true)
  end

  describe "Complex#== with Numeric" do
    it_behaves_like(:complex_equal_value_numeric, :shared => true)
  end

  describe "Complex#== with Object" do
    it_behaves_like(:complex_equal_value_object, :shared => true)
  end
end
