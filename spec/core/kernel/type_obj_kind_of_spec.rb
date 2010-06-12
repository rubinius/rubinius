require File.expand_path('../../../spec_helper', __FILE__)

module TypeSpecMod; end

class TypeSpecSuper; end
class TypeSpecSub < TypeSpecSuper; include TypeSpecMod; end
class TypeSpecSubSub < TypeSpecSub; end

class TypeSpecOtherClass; end

class TypeSpecFakeClass
  def kind_of?(other)
    true
  end

  alias is_a? kind_of?
  alias instance_of? kind_of?
end

describe "Type.obj_kind_of?" do
  it "returns true if obj is an instance of the given class" do
    Type.obj_kind_of?(TypeSpecSuper.new, TypeSpecSuper).should == true
  end

  it "returns true if obj is an instance of given class or its subclass" do
    Type.obj_kind_of?(TypeSpecSub.new, TypeSpecSuper).should == true
  end

  it "returns true if given module is included in object's class or its superclasses" do
    Type.obj_kind_of?(TypeSpecSubSub.new, TypeSpecSuper).should == true
  end

  it "returns false if obj is not an instance, subclass instance " \
     "nor has module in its hierarchy" do
    Type.obj_kind_of?(TypeSpecOtherClass.new, TypeSpecSuper).should == false
  end

  it "does not honour overridden #kind_of?, #is_a? nor #instance_of?" do
    t  = TypeSpecFakeClass.new
    t.kind_of?(Array).should == true
    t.is_a?(Array).should == true
    t.instance_of?(Array).should == true

    Type.obj_kind_of?(t, Array).should == false
  end
end
