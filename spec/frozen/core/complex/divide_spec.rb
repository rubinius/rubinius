ruby_version_is "1.9" do
  
  require File.dirname(__FILE__) + '/../../shared/complex/divide'
  
  describe "Complex#/ with Complex" do
    it_behaves_like(:complex_divide_complex, :/)
  end

  describe "Complex#/ with Fixnum" do
    it_behaves_like(:complex_divide_fixnum, :/)
  end

  describe "Complex#/ with Bignum" do
    it_behaves_like(:complex_divide_bignum, :/)
  end
  
  describe "Complex#/ with Float" do
    it_behaves_like(:complex_divide_float, :/)
  end

  describe "Complex#/ with Object" do
    it_behaves_like(:complex_divide_object, :/)
  end
end
