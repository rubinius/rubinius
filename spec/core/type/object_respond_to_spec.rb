require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Rubinius::Type.object_respond_to?" do
  before(:each) do
    @obj = TypeSpecs::MethodVisibility.new
  end

  it "returns true when an object responds to a method" do
    Rubinius::Type.object_respond_to?(@obj, :public_method).should be_true
  end

  it "returns false when an object does not respond to a method" do
    Rubinius::Type.object_respond_to?(@obj, :public_method2).should be_false
  end

  it "returns false when the method is private" do
    Rubinius::Type.object_respond_to?(@obj, :private_method).should be_false
  end

  it "returns true when the method is private and should look for private methods" do
    Rubinius::Type.object_respond_to?(@obj, :private_method, true).should be_true
  end
end
