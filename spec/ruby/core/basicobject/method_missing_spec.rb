require File.expand_path('../../../shared/kernel/method_missing', __FILE__)

ruby_version_is "1.9" do
  describe "BasicObject#method_missing" do
    it_behaves_like(:method_missing, :method_missing, BasicObject)
  end

  describe "BasicObject#method_missing is undefined" do
    it_behaves_like(:method_missing_undef, :method_missing, BasicObject)
  end

  describe "BasicObject.method_missing" do
    it_behaves_like(:method_missing_class_method, :method_missing, BasicObject)
  end
end
