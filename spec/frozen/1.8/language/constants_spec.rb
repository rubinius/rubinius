require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/fixtures/constants'

class StaticScope
  def chain
    ary = []
    ss = self
    while ss
      ary << ss.module
      ss = ss.parent
    end
    
    return ary
  end
end

describe "Constant lookup rule" do
  it "finds a toplevel constant" do
    Exception.should == ::Exception
  end
  
  it "looks up the static, lexical scope in a class method" do
    ConstantSpecs::A::B::C.number.should == 47
    ConstantSpecs::A::B::C.name.should == "specs"
    ConstantSpecs::A::B::C.place.should == "boston"
  end
  
  it "looks up the static, lexical scope in an instance method" do
    ConstantSpecs::A::B::C.new.number.should == 47
    ConstantSpecs::A::B::C.new.name.should == "specs"
    ConstantSpecs::A::B::C.new.place.should == "boston"
  end
  
  it "looks up the superclass chain" do
    ConstantSpecs::D.new.number.should == 47
    ConstantSpecs::D.number.should == 47
  end
  
  it "isn't influenced by the calling scope" do
    ConstantSpecs::E.new.go.should == 8
  end
  
  it "isn't influenced by the calling scope, in modules" do
    ConstantSpecs::I.new.go.should == ::Exception
  end
  
  it "calls const_missing on the original scope" do
    ConstantSpecs::A::B::C.new.fire_missing.should == :missing!
  end
  
  it "is bound in blocks properly" do
    ConstantSpecs::Foo.foo.should == 47
  end
  
  it "is bound in blocks, then singletons properly" do
    ConstantSpecs::Foo.foo2.should == 47
  end
  
  # This expectation leaves the 'LeftoverConstant' laying around in
  # the Object class.  Unfortunately, due to the nature of includes,
  # you can't remove constants from included modules.
  it "looks up in modules included in Object" do
    begin
      module M; LeftoverConstant = 42; end
      Object.send(:include, M)
      lambda { Object::LeftoverConstant }.should_not raise_error()
    ensure
      Object.send :remove_const, :M
    end
  end
  
  it "only searches a Module or Class" do
    lambda { :File::TEST }.should raise_error(TypeError)
  end
end

describe "Constant declaration" do
  it "can be done under modules" do
    begin
      module M; end
      proc{ M::Z = 3 }.should_not raise_error()
    ensure
      Object.send :remove_const, :M
    end
  end

  it "can be done under classes" do
    begin
      class C; end
      proc{ C::Z = 3 }.should_not raise_error()
    ensure
      Object.send :remove_const, :C
    end
  end

  it "cannot be done under other types of constants" do
    begin
      V = 3
      proc{ V::Z = 3 }.should raise_error(TypeError)
    ensure
      Object.send :remove_const, :V
    end
  end

  it "returns the assigned variable" do
    begin
      module M; end
      (Y = 3).should == 3
      (M::Z = 3).should == 3
    ensure
      Object.send :remove_const, :Y
      Object.send :remove_const, :M rescue nil
    end
  end
end

