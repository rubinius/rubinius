require File.expand_path('../../../shared/kernel/method_missing', __FILE__)

ruby_version_is "1.9" do
  describe "BasicObject#method_missing" do
    it_behaves_like(:method_missing, :method_missing, BasicObject)

    it "should raise NoMethodError if undefined method is called" do
      lambda { BasicObject.new.unknown }.should raise_error(NoMethodError)
    end
  end

  describe "BasicObject#method_missing is undefined" do
    it_behaves_like(:method_missing_undef, :method_missing, BasicObject)
  end

  describe "BasicObject.method_missing" do
    it_behaves_like(:method_missing_class_method, :method_missing, BasicObject)
  end
end
