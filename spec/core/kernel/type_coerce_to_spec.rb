require File.expand_path('../../../spec_helper', __FILE__)

module TypeSpecMod; end

class TypeSpecSuper; end
class TypeSpecSub < TypeSpecSuper; include TypeSpecMod; end
class TypeSpecSubSub < TypeSpecSub; end

describe "Type.coerce_to" do
  it "returns obj if already correct type according to Type.obj_kind_of?" do
    o, a, t = Object.new, []
    t, s, s2 = TypeSpecSuper.new, TypeSpecSub.new, TypeSpecSubSub.new

    Type.coerce_to(o, Object, :nonesuch).object_id.should == o.object_id
    Type.coerce_to(a, Array, :nonesuch).object_id.should == a.object_id
    Type.coerce_to(t, TypeSpecSuper, :nonesuch).object_id.should == t.object_id
    Type.coerce_to(s, TypeSpecSuper, :nonesuch).object_id.should == s.object_id
    Type.coerce_to(s2, TypeSpecMod, :nonesuch).object_id.should == s2.object_id
  end

  it "calls given method on obj if is not of correct type according to Type.obj_kind_of?" do
    o = Object.new
    def o.to_whatever; $type_spec_1 = :worked; []; end

    $type_spec_1 = nil
    Type.coerce_to(o, Array, :to_whatever)
    $type_spec_1.should == :worked
  end

  it "raises a TypeError if any exception is raised when calling obj.method" do
    o = Object.new

    l = lambda { Type.coerce_to(o, Array, :to_whatever) }
    l.should raise_error(TypeError)

    def o.to_whatever
      raise ArgumentError, 'blah'
    end

    l = lambda { Type.coerce_to(o, Array, :to_whatever) }
    l.should raise_error(TypeError)
  end

  it "raises a TypeError if obj.method does not return correct type per Type.obj_kind_of?" do
    o = Object.new
    def o.to_whatever; Array.new; end

    l = lambda { Type.coerce_to(o, Hash, :to_whatever) }
    l.should raise_error(TypeError)
  end

  it "returns result of obj.method if result is of correct type per Type.obj_kind_of?" do
    o = Object.new
    def o.to_whatever; Hash.new; end

    Type.coerce_to(o, Hash, :to_whatever).class.should == Hash
  end
end
