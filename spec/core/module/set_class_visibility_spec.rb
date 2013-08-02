require File.expand_path('../../../spec_helper', __FILE__)

describe "Module#set_class_visibility" do
  before :each do
    @module = Module.new { def self.public_method; end }
  end

  it "is a private method" do
    Module.should have_private_instance_method(:set_class_visibility, false)
  end

  it "modifies class method visibility" do
    @module.send :set_class_visibility, :public_method, :protected
    @module.protected_methods(false).should include(stasy(:public_method))

    @module.send :set_class_visibility, :public_method, :private
    @module.should have_private_method(:public_method, false)

    @module.send :set_class_visibility, :public_method, :public
    @module.public_methods(false).should include(stasy(:public_method))
  end

  it "accepts a String name as Symbol" do
    @module.send :set_class_visibility, "public_method", :private
    @module.should have_private_method(:public_method, false)
  end

  it "returns the coerced name" do
    @module.send(:set_class_visibility, "public_method", :private).should equal(:public_method)
  end
end
