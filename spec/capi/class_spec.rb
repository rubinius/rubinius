require File.dirname(__FILE__) + '/spec_helper'

load_extension("class")

class CApiClassSpecs
  module M
    def included?
      true
    end
  end

  class IncludesM
  end

  class Alloc
    attr_reader :initialized
    attr_reader :arguments

    def initialize(*args)
      @initialized = true
      @arguments   = args
    end
  end

  class Attr
    def initialize
      @foo, @bar, @baz = 1, 2, 3
    end
  end

  class CVars
    @@cvar  = :cvar
    @c_ivar = :c_ivar

    def new_cv
      @@new_cv if defined? @@new_cv
    end

    def new_cvar
      @@new_cvar if defined? @@new_cvar
    end

    def rbdcv_cvar
      @@rbdcv_cvar if defined? @@rbdcv_cvar
    end
  end
end

describe "C-API Class function" do
  before :each do
    @s = CApiClassSpecs.new
  end

  describe "rb_class_new_instance" do
    it "allocates and initializes a new object" do
      o = @s.rb_class_new_instance(0, nil, CApiClassSpecs::Alloc)
      o.class.should == CApiClassSpecs::Alloc
      o.initialized.should be_true
    end

    it "passes arguments to the #initialize method" do
      o = @s.rb_class_new_instance(2, [:one, :two], CApiClassSpecs::Alloc)
      o.arguments.should == [:one, :two]
    end
  end

  describe "rb_include_module" do
    it "includes a module into a class" do
      o = CApiClassSpecs::IncludesM.new
      lambda { o.included? }.should raise_error(NameError)
      @s.rb_include_module(CApiClassSpecs::IncludesM, CApiClassSpecs::M)
      o.included?.should be_true
    end
  end

  describe "rb_define_attr" do
    before :each do
      @a = CApiClassSpecs::Attr.new
    end

    it "defines an attr_reader when passed true, false" do
      @s.rb_define_attr(CApiClassSpecs::Attr, :foo, true, false)
      @a.foo.should == 1
      lambda { @a.foo = 5 }.should raise_error(NameError)
    end

    it "defines an attr_writer when passed false, true" do
      @s.rb_define_attr(CApiClassSpecs::Attr, :bar, false, true)
      lambda { @a.bar }.should raise_error(NameError)
      @a.bar = 5
      @a.instance_variable_get(:@bar).should == 5
    end

    it "defines an attr_accessor when passed true, true" do
      @s.rb_define_attr(CApiClassSpecs::Attr, :baz, true, true)
      @a.baz.should == 3
      @a.baz = 6
      @a.baz.should == 6
    end
  end

  describe "rb_class2name" do
    it "returns the class name" do
      @s.rb_class2name(CApiClassSpecs).should == "CApiClassSpecs"
    end
  end

  describe "rb_cvar_defined" do
    it "returns false when the class variable is not defined" do
      @s.rb_cvar_defined(CApiClassSpecs::CVars, "@@nocvar").should be_false
    end

    it "returns true when the class variable is defined" do
      @s.rb_cvar_defined(CApiClassSpecs::CVars, "@@cvar").should be_true
    end

    it "returns true if the class instance variable is defined" do
      @s.rb_cvar_defined(CApiClassSpecs::CVars, "@c_ivar").should be_true
    end
  end

  describe "rb_cv_set" do
    it "sets a class variable" do
      o = CApiClassSpecs::CVars.new
      o.new_cv.should be_nil
      @s.rb_cv_set(CApiClassSpecs::CVars, "@@new_cv", 1)
      o.new_cv.should == 1
    end
  end

  describe "rb_cv_get" do
    it "returns the value of the class variable" do
      @s.rb_cvar_get(CApiClassSpecs::CVars, "@@cvar").should == :cvar
    end

    it "raises a NameError if the class variable is not defined" do
      lambda {
        @s.rb_cv_get(CApiClassSpecs::CVars, "@@no_cvar")
      }.should raise_error(NameError)
    end
  end

  describe "rb_cvar_set" do
    it "sets a class variable" do
      o = CApiClassSpecs::CVars.new
      o.new_cvar.should be_nil
      @s.rb_cvar_set(CApiClassSpecs::CVars, "@@new_cvar", 1)
      o.new_cvar.should == 1
    end
    
  end
  
  describe "rb_define_class_variable" do
    it "sets a class variable" do 
      o = CApiClassSpecs::CVars.new
      o.rbdcv_cvar.should be_nil
      @s.rb_define_class_variable(CApiClassSpecs::CVars, "@@rbdcv_cvar", 1)
      o.rbdcv_cvar.should == 1
    end
  end

  describe "rb_cvar_get" do
    it "returns the value of the class variable" do
      @s.rb_cvar_get(CApiClassSpecs::CVars, "@@cvar").should == :cvar
    end

    it "raises a NameError if the class variable is not defined" do
      lambda {
        @s.rb_cvar_get(CApiClassSpecs::CVars, "@@no_cvar")
      }.should raise_error(NameError)
    end
  end
end
