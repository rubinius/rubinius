require File.dirname(__FILE__) + '/spec_helper'

load_extension('language')

describe "CApiBlockSpecs" do
  before :each do
    @f = CApiBlockSpecs.new
  end
  
  it "identifies blocks using rb_block_given_p" do
    @f.block_given?.should == false
    (@f.block_given? { puts "FOO" } ).should == true
  end

  it "correctly utilizes rb_yield" do
    (@f.do_yield { |x| x+1 }).should == 6
    lambda { @f.do_yield }.should raise_error(LocalJumpError)
  end

end

describe "CApiCallSuperSpecs" do
  before :each do
    @s = CApiCallSuperSpecs.new
  end

  class TheSuper
    # Set an instance variable to ensure we are doing rb_call_super on this
    # instance.
    def initialize(value)
      @value = value.to_i
    end

    def a_method
      return @value
    end
  end

  class TheSub < TheSuper
  end

  it "rb_call_super should call the method in the superclass" do
    instance = TheSub.new(1)
    instance.a_method.should == 1
    @s.override_method(TheSub) # Override the method, use rb_call_super
    instance.a_method.should == 2
  end

  it "should create a subclass and create a new instance" do
    @s.test_subclass_and_instantiation().should == true
    (instance = ModuleTest::ClassSuper.new()).should_not == nil
    (instance = ModuleTest::ClassSub.new()).should_not == nil
  end
end

class CApiLanguageSpecs
  class Super
    def self.inherited(child)
      @children ||= []
      @children << child
    end

    def self.children
      @children
    end
  end

  module Container
  end

  class Inner < CApiLanguageSpecs
  end
end

describe "CApiLanguageSpecs" do
  before :each do
    @s = CApiLanguageSpecs.new
  end

  describe "rb_define_module_under" do
    it "creates a new module inside the inner class" do
      mod = @s.rb_define_module_under(CApiLanguageSpecs::Inner,
                                     "Container")
      mod.should be_kind_of(Module)
      CApiLanguageSpecs::Container.should_not == CApiLanguageSpecs::Inner::Container
    end

    it "sets the module name" do
      mod = @s.rb_define_module_under(CApiLanguageSpecs::Inner, "Named")
      mod.name.should == "CApiLanguageSpecs::Inner::Named"
    end
  end

  describe "rb_define_class_under" do
    it "creates a subclass of the superclass contained in a module" do
      cls = @s.rb_define_class_under(CApiLanguageSpecs::Container,
                                     "Child",
                                     CApiLanguageSpecs::Super)
      cls.should be_kind_of(Class)
      CApiLanguageSpecs::Super.should be_ancestor_of(CApiLanguageSpecs::Container::Child)
    end

    it "uses Object as the superclass if NULL is passed" do
      @s.rb_define_class_under(CApiLanguageSpecs, "DefaultSuper", nil)
      Object.should be_ancestor_of(CApiLanguageSpecs::DefaultSuper)
    end

    it "sets the class name" do
      cls = @s.rb_define_class_under(CApiLanguageSpecs::Container, "Named", nil)
      cls.name.should == "CApiLanguageSpecs::Container::Named"
    end

    it "call #inherited on the superclass" do
      cls = @s.rb_define_class_under(CApiLanguageSpecs,
                                     "Inherited", CApiLanguageSpecs::Super)
      CApiLanguageSpecs::Super.children.should include(CApiLanguageSpecs::Inherited)
    end
  end
end
