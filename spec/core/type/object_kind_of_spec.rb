require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Rubinius::Type.object_kind_of?" do
  it "returns true if the object is an instance of the class" do
    Rubinius::Type.object_kind_of?(TypeSpecs::A.new, TypeSpecs::A).should be_true
  end

  it "returns true if the object is an instance of a subclass of a class" do
    Rubinius::Type.object_kind_of?(TypeSpecs::B.new, TypeSpecs::A).should be_true
  end

  it "returns true if the object's class includes the module" do
    Rubinius::Type.object_kind_of?(TypeSpecs::C.new, TypeSpecs::M).should be_true
  end

  it "returns true if the object's class's superclass includes the module" do
    Rubinius::Type.object_kind_of?(TypeSpecs::D.new, TypeSpecs::M).should be_true
  end

  it "returns false if the object is not an instance of the class or the module is not included in the hierarchy" do
    Rubinius::Type.object_kind_of?(TypeSpecs::C.new, TypeSpecs::A).should be_false
  end

  it "does not consider the value returned by #kind_of?, #is_a?, or #instance_of?" do
    Rubinius::Type.object_kind_of?(TypeSpecs::E.new, TypeSpecs::A).should be_false
  end
end
