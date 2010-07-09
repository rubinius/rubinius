require File.expand_path('../spec_helper', __FILE__)

load_extension('module')

class CApiModuleSpecs
  class A
    X = 1
  end

  class B < A
    Y = 2
  end

  class C
    Z = 3
  end

  module M
  end

  class Super
  end
end

describe "CApiModule" do

  before :each do
    @m = CApiModuleSpecs.new
  end

  describe "rb_define_global_const" do
    it "defines a constant on Object" do
      @m.rb_define_global_const("CApiModuleSpecsGlobalConst", 7)
      ::CApiModuleSpecsGlobalConst.should == 7
      Object.send :remove_const, :CApiModuleSpecsGlobalConst
    end
  end

  describe "rb_const_set given a symbol name and a value" do
    it "sets a new constant on a module" do
      @m.rb_const_set(CApiModuleSpecs::C, :W, 7)
      CApiModuleSpecs::C::W.should == 7
    end

    it "sets an existing constant's value" do
      @m.rb_const_set(CApiModuleSpecs::C, :Z, 8)
      CApiModuleSpecs::C::Z.should == 8
    end
  end

  describe "rb_define_class_under" do
    it "creates a subclass of the superclass contained in a module" do
      cls = @m.rb_define_class_under(CApiModuleSpecs,
                                     "ModuleSpecsClassUnder1",
                                     CApiModuleSpecs::Super)
      cls.should be_kind_of(Class)
      CApiModuleSpecs::Super.should be_ancestor_of(CApiModuleSpecs::ModuleSpecsClassUnder1)
    end

    it "uses Object as the superclass if NULL is passed" do
      @m.rb_define_class_under(CApiModuleSpecs, "ModuleSpecsClassUnder2", nil)
      Object.should be_ancestor_of(CApiModuleSpecs::ModuleSpecsClassUnder2)
    end

    it "sets the class name" do
      cls = @m.rb_define_class_under(CApiModuleSpecs, "ModuleSpecsClassUnder3", nil)
      cls.name.should == "CApiModuleSpecs::ModuleSpecsClassUnder3"
    end

    it "call #inherited on the superclass" do
      CApiModuleSpecs::Super.should_receive(:inherited)
      cls = @m.rb_define_class_under(CApiModuleSpecs,
                                     "ModuleSpecsClassUnder4", CApiModuleSpecs::Super)
    end
  end

  describe "rb_define_module_under" do
    it "creates a new module inside the inner class" do
      mod = @m.rb_define_module_under(CApiModuleSpecs, "ModuleSpecsModuleUnder1")
      mod.should be_kind_of(Module)
    end

    it "sets the module name" do
      mod = @m.rb_define_module_under(CApiModuleSpecs, "ModuleSpecsModuleUnder2")
      mod.name.should == "CApiModuleSpecs::ModuleSpecsModuleUnder2"
    end
  end

  describe "rb_define_const given a String name and a value" do
    it "defines a new constant on a module" do
      @m.rb_define_const(CApiModuleSpecs::C, "V", 7)
      CApiModuleSpecs::C::V.should == 7
    end

    it "sets an existing constant's value" do
      @m.rb_define_const(CApiModuleSpecs::C, "Z", 9)
      CApiModuleSpecs::C::Z.should == 9
    end
  end

  describe "rb_const_defined" do
    # The fixture converts C boolean test to Ruby 'true' / 'false'
    it "returns C non-zero if a constant is defined" do
      @m.rb_const_defined(CApiModuleSpecs::A, :X).should be_true
    end

    it "returns C non-zero if a constant is defined in Object" do
      @m.rb_const_defined(CApiModuleSpecs::A, :Module).should be_true
    end
  end

  describe "rb_const_defined_at" do
    # The fixture converts C boolean test to Ruby 'true' / 'false'
    it "returns C non-zero if a constant is defined" do
      @m.rb_const_defined_at(CApiModuleSpecs::A, :X).should be_true
    end

    it "does not search in ancestors for the constant" do
      @m.rb_const_defined_at(CApiModuleSpecs::B, :X).should be_false
    end

    it "does not search in Object" do
      @m.rb_const_defined_at(CApiModuleSpecs::A, :Module).should be_false
    end
  end

  describe "rb_const_get" do
    it "returns a constant defined in the module" do
      @m.rb_const_get(CApiModuleSpecs::A, :X).should == 1
    end

    it "returns a constant defined at toplevel" do
      @m.rb_const_get(CApiModuleSpecs::A, :Fixnum).should == Fixnum
    end

    it "returns a constant defined in a superclass" do
      @m.rb_const_get_from(CApiModuleSpecs::B, :X).should == 1
    end

    it "calls #const_missing if the constant is not defined in the class or ancestors" do
      CApiModuleSpecs::A.should_receive(:const_missing).with(:CApiModuleSpecsUndefined)
      @m.rb_const_get_from(CApiModuleSpecs::A, :CApiModuleSpecsUndefined)
    end
  end

  describe "rb_const_get_from" do
    it "returns a constant defined in the module" do
      @m.rb_const_get_from(CApiModuleSpecs::B, :Y).should == 2
    end

    it "returns a constant defined in a superclass" do
      @m.rb_const_get_from(CApiModuleSpecs::B, :X).should == 1
    end

    it "calls #const_missing if the constant is not defined in the class or ancestors" do
      CApiModuleSpecs::M.should_receive(:const_missing).with(:Fixnum)
      @m.rb_const_get_from(CApiModuleSpecs::M, :Fixnum)
    end
  end

  describe "rb_const_get_at" do
    it "returns a constant defined in the module" do
      @m.rb_const_get_at(CApiModuleSpecs::B, :Y).should == 2
    end

    it "calls #const_missing if the constant is not defined in the module" do
      CApiModuleSpecs::B.should_receive(:const_missing).with(:X)
      @m.rb_const_get_at(CApiModuleSpecs::B, :X)
    end
  end

  describe "rb_define_alias" do
    it "defines an alias for an existing method" do
      cls = Class.new do
        def method_to_be_aliased
          :method_to_be_aliased
        end
      end

      @m.rb_define_alias cls, "method_alias", "method_to_be_aliased"
      cls.new.method_alias.should == :method_to_be_aliased
    end
  end

  describe "rb_define_global_function" do
    it "defines a method on Object" do
      @m.rb_define_global_function("module_specs_global_function")
      Kernel.should have_method(:module_specs_global_function)
      module_specs_global_function.should == :test_method
    end
  end

  describe "rb_define_method" do
    it "defines a method on a class" do
      cls = Class.new
      @m.rb_define_method(cls, "test_method")
      cls.should have_instance_method(:test_method)
      cls.new.test_method.should == :test_method
    end

    it "defines a method on a module" do
      mod = Module.new
      @m.rb_define_method(mod, "test_method")
      mod.should have_instance_method(:test_method)
    end
  end

  describe "rb_define_module_function" do
    before :each do
      @mod = Module.new
      @m.rb_define_module_function @mod, "test_module_function"
    end

    it "defines a module function" do
      @mod.test_module_function.should == :test_method
    end

    it "defines a private instance method" do
      cls = Class.new
      cls.send :include, @mod

      cls.should have_private_instance_method(:test_module_function)
    end
  end

  describe "rb_define_private_method" do
    it "defines a private method on a class" do
      cls = Class.new
      @m.rb_define_private_method(cls, "test_method")
      cls.should have_private_instance_method(:test_method)
      cls.new.send(:test_method).should == :test_method
    end

    it "defines a private method on a module" do
      mod = Module.new
      @m.rb_define_private_method(mod, "test_method")
      mod.should have_private_instance_method(:test_method)
    end
  end

  describe "rb_define_protected_method" do
    it "defines a protected method on a class" do
      cls = Class.new
      @m.rb_define_protected_method(cls, "test_method")
      cls.should have_protected_instance_method(:test_method)
      cls.new.send(:test_method).should == :test_method
    end

    it "defines a protected method on a module" do
      mod = Module.new
      @m.rb_define_protected_method(mod, "test_method")
      mod.should have_protected_instance_method(:test_method)
    end
  end

  describe "rb_define_singleton_method" do
    it "defines a method on the singleton class" do
      cls = Class.new
      a = cls.new
      @m.rb_define_singleton_method a, "module_specs_singleton_method"
      a.module_specs_singleton_method.should == :test_method
      lambda { cls.new.module_specs_singleton_method }.should raise_error(NoMethodError)
    end
  end

  describe "rb_undef_method" do
    it "undef'ines a method on a class" do
      cls = Class.new do
        def ruby_test_method
          :ruby_test_method
        end
      end

      cls.new.ruby_test_method.should == :ruby_test_method
      @m.rb_undef_method cls, "ruby_test_method"
      cls.should_not have_instance_method(:ruby_test_method)
    end
  end

  describe "rb_class2name" do
    it "returns the module name" do
      @m.rb_class2name(CApiModuleSpecs::M).should == "CApiModuleSpecs::M"
    end
  end
end
