require File.dirname(__FILE__) + '/spec_helper'

load_extension("class")

module CApiClassSpecsModule
  def im_included
    "YEP"
  end
end

class CApiClassSpecsA
  attr_reader :foo

  def initialize(v)
    @foo = v
  end
end

describe "CApiClassSpecs" do
  before :each do
    @s = CApiClassSpecs.new
  end

  it "rb_obj_alloc should allocate a new uninitialized object" do
    o = @s.rb_obj_alloc(CApiClassSpecsA)
    o.class.should == CApiClassSpecsA
    o.foo.should == nil
  end

  it "rb_obj_call_init should send #initialize" do
    o = @s.rb_obj_alloc(CApiClassSpecsA)
    @s.rb_obj_call_init(o, 1, [100])
    o.foo.should == 100
  end

  it "rb_class_new_instance should allocate and initialize a new object" do
    o = @s.rb_class_new_instance(1, ["yo"], CApiClassSpecsA)
    o.class.should == CApiClassSpecsA
    o.foo.should == "yo"
  end

  it "rb_include_module should include a module into a class" do
    CApiClassSpecsA.new(4).respond_to?(:im_included).should == false
    @s.rb_include_module(CApiClassSpecsA, CApiClassSpecsModule)
    CApiClassSpecsA.new(4).respond_to?(:im_included).should == true
    CApiClassSpecsA.new(4).im_included.should == "YEP"
  end

  it "rb_define_attr should be able to define attributes" do
    @s.rb_define_attr(CApiClassSpecsA, :bar, true, false)
    @s.rb_define_attr(CApiClassSpecsA, :baz, false, true)
    @s.rb_define_attr(CApiClassSpecsA, :bat, true, true)
    s = CApiClassSpecsA.new(7)
    s.respond_to?(:bar).should == true
    s.respond_to?(:bar=).should == false
    s.respond_to?(:baz).should == false
    s.respond_to?(:baz=).should == true
    s.respond_to?(:bat).should == true
    s.respond_to?(:bat=).should == true
  end

  it "rb_class2name should return the classname" do
    @s.rb_class2name(CApiClassSpecs).should == "CApiClassSpecs"
  end
end

describe "CApiCVGetSetSpecs" do
  before :each do
    @s = CApiCVGetSetSpecs.new
  end

  class CVTest
  end

#  Actually, this causes a segfault on MRI
#  it "rb_cv_get should return nil when there's no class variable" do
#    @s.rb_cv_get(CVTest, "@@no_variable").should == nil
#    @s.rb_cvar_get(CVTest, "@@no_variable").should == nil
#  end

  it "rb_cv_set should set a class variable" do
    @s.rb_cv_set(CVTest, "@@class_variable_1", 1).should == 1
    @s.rb_cvar_set(CVTest, "@@class_variable_2", 2).should == 2
  end

  it "rb_cv_get should get a class variable" do
    @s.rb_cv_set(CVTest, "@@class_variable_3", 3).should == 3
    @s.rb_cv_get(CVTest, "@@class_variable_3").should == 3
    @s.rb_cvar_get(CVTest, "@@class_variable_3").should == 3
  end

  it "rb_cv_set should allow changing class variable" do
    @s.rb_cv_set(CVTest, "@@class_variable_4", 4).should == 4
    @s.rb_cv_get(CVTest, "@@class_variable_4").should == 4
    @s.rb_cv_set(CVTest, "@@class_variable_4", 5).should == 5
    @s.rb_cv_get(CVTest, "@@class_variable_4").should == 5
    @s.rb_cvar_set(CVTest, "@@class_variable_4", 6).should == 6
    @s.rb_cv_get(CVTest, "@@class_variable_4").should == 6
  end
end

describe "CApiCVarDefinedSpecs" do
  before :each do
    @s = CApiCVarDefinedSpecs.new
  end

  class CVarTest
    @@class_var = 1

    def initialize
      @instance_var = 2
    end
  end


  it "rb_cvar_defined should return false when variable is not defined" do
    @s.rb_cvar_defined(CVarTest, "@@not_class_var").should_not == true
  end

  it "rb_cvar_defined should return true when variable is defined" do
    @s.rb_cvar_defined(CVarTest, "@@class_var").should == true
  end

  it "rb_cvar_defined should ignore instance variables" do
    @s.rb_cvar_defined(CVarTest, "@instance_var").should_not == true
  end
end
