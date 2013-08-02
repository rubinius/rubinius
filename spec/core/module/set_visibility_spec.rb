require File.expand_path('../../../spec_helper', __FILE__)

describe "Module#set_visibility" do
  before :each do
    @module = Module.new { def public_method; end }
  end

  it "modifies instance method visibility" do
    @module.set_visibility :public_method, :protected
    @module.should have_protected_instance_method(:public_method, false)

    @module.set_visibility :public_method, :private
    @module.should have_private_instance_method(:public_method, false)

    @module.set_visibility :public_method, :public
    @module.should have_public_instance_method(:public_method, false)
  end

  it "accepts a String name as Symbol" do
    @module.set_visibility "public_method", :private
    @module.should have_private_instance_method(:public_method, false)
  end

  it "returns the coerced name" do
    @module.set_visibility("public_method", :private).should equal(:public_method)
  end
end
