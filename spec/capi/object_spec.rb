require File.dirname(__FILE__) + '/spec_helper'

load_extension("object")

# TODO: fix all these specs

class CApiObjectSpecs
  class Alloc
    attr_reader :initialized, :arguments

    def initialize(*args)
      @initialized = true
      @arguments   = args
    end
  end
end

describe "CApiObject" do

  before do
    @o = CApiObjectSpecs.new
  end

  class ObjectTest
    def initialize
      @foo = 7
    end

    def foo
    end
  end

  class AryChild < Array
  end

  class StrChild < String
  end

  class DescObjectTest < ObjectTest
  end

  it "rb_obj_alloc should allocate a new uninitialized object" do
    o = @o.rb_obj_alloc(CApiObjectSpecs::Alloc)
    o.class.should == CApiObjectSpecs::Alloc
    o.initialized.should be_nil
  end

  it "rb_obj_call_init should send #initialize" do
    o = @o.rb_obj_alloc(CApiObjectSpecs::Alloc)
    o.initialized.should be_nil

    @o.rb_obj_call_init(o, 2, [:one, :two])
    o.initialized.should be_true
    o.arguments.should == [:one, :two]
  end

  it "rb_is_instance_of should return true if an object is an instance" do
    @o.rb_obj_is_instance_of(ObjectTest.new, ObjectTest).should == true
    @o.rb_obj_is_instance_of(DescObjectTest.new, ObjectTest).should == false
  end

  it "rb_is_kind_of should return true if an object is an instance or descendent" do
    @o.rb_obj_is_kind_of(ObjectTest.new, ObjectTest).should == true
    @o.rb_obj_is_kind_of(DescObjectTest.new, ObjectTest).should == true
    @o.rb_obj_is_kind_of(Object.new, ObjectTest).should == false
  end

  it "rb_respond_to should return 1 if respond_to? is true and 0 if respond_to? is false" do
    @o.rb_respond_to(ObjectTest.new, :foo).should == true
    @o.rb_respond_to(ObjectTest.new, :bar).should == false
  end

  it "rb_to_id should return a symbol representation of the object" do
    @o.rb_to_id("foo").should == :foo
    @o.rb_to_id(:foo).should == :foo
  end

  it "rb_require should require a ruby file" do
    $foo.should == nil
    $:.unshift File.dirname(__FILE__)
    @o.rb_require()
    $foo.should == 7
  end

  it "rb_attr_get should get an instance variable" do
    o = ObjectTest.new
    @o.rb_attr_get(o, :@foo).should == 7
  end

  it "rb_check_array_type should try to coerce to array, otherwise return nil" do
    ac = AryChild.new
    ao = Array.new
    h = Hash.new
    @o.rb_check_array_type(ac).should == []
    @o.rb_check_array_type(ao).should == []
    @o.rb_check_array_type(h).should == nil
  end

  it "rb_check_convert_type should try to coerce to a type, otherwise return nil" do
    ac = AryChild.new
    ao = Array.new
    h = Hash.new
    # note that I force the ary information in the spec extension
    @o.rb_check_convert_type(ac).should == []
    @o.rb_check_convert_type(ao).should == []
    @o.rb_check_convert_type(h).should == nil
  end

  it "rb_check_string_type should try to coerce to a string, otherwise return nil" do
    sc = "Hello"
    so = StrChild.new("Hello")
    h = {:hello => :goodbye}
    @o.rb_check_string_type(sc).should == "Hello"
    @o.rb_check_string_type(so).should == "Hello"
    @o.rb_check_string_type(h).should == nil
  end

  it "rb_convert_type should try to coerce to a type, otherwise raise a TypeError" do
    ac = AryChild.new
    ao = Array.new
    h = Hash.new
    # note that the ary information is forced in the spec extension
    @o.rb_convert_type(ac).should == []
    @o.rb_convert_type(ao).should == []
    lambda { @o.rb_convert_type(h) }.should raise_error(TypeError)
  end

  it "rb_inspect should return a string with the inspect representation" do
    @o.rb_inspect(nil).should == "nil"
    @o.rb_inspect(0).should == '0'
    @o.rb_inspect([1,2,3]).should == '[1, 2, 3]'
    @o.rb_inspect("0").should == '"0"'
  end

  it "rb_class_of should return the class of a object" do
    @o.rb_class_of(nil).should == NilClass
    @o.rb_class_of(0).should == Fixnum
    @o.rb_class_of(0.1).should == Float
    @o.rb_class_of(ObjectTest.new).should == ObjectTest
  end

  it "rb_obj_classname should return the class name of a object" do
    @o.rb_obj_classname(nil).should == 'NilClass'
    @o.rb_obj_classname(0).should == 'Fixnum'
    @o.rb_obj_classname(0.1).should == 'Float'
    @o.rb_obj_classname(ObjectTest.new).should == 'ObjectTest'
  end

  it "rb_type should return the type constant for the object" do
    class DescArray < Array
    end
    @o.rb_is_type_nil(nil).should == true
    @o.rb_is_type_object([]).should == false
    @o.rb_is_type_object(ObjectTest.new).should == true
    @o.rb_is_type_array([]).should == true
    @o.rb_is_type_array(DescArray.new).should == true
    @o.rb_is_type_module(ObjectTest).should == false
    @o.rb_is_type_class(ObjectTest).should == true
  end

  describe "RTEST" do
    it "returns C false if passed Qfalse" do
      @o.RTEST(false).should be_false
    end

    it "returns C false if passed Qnil" do
      @o.RTEST(nil).should be_false
    end

    it "returns C true if passed Qtrue" do
      @o.RTEST(true).should be_true
    end

    it "returns C true if passed a Symbol" do
      @o.RTEST(:test).should be_true
    end

    it "returns C true if passed an Object" do
      @o.RTEST(Object.new).should be_true
    end
  end

  describe "rb_special_const_p" do
    it "returns true if passed Qfalse" do
      @o.rb_special_const_p(false).should be_true
    end

    it "returns true if passed Qtrue" do
      @o.rb_special_const_p(true).should be_true
    end

    it "returns true if passed Qnil" do
      @o.rb_special_const_p(nil).should be_true
    end

    it "returns true if passed a Symbol" do
      @o.rb_special_const_p(:test).should be_true
    end

    it "returns true if passed a Fixnum" do
      @o.rb_special_const_p(10).should be_true
    end

    it "returns false if passed an Object" do
      @o.rb_special_const_p(Object.new).should be_false
    end
  end

  describe "rb_extend_object" do
    it "add the module's instance methods to the object" do
      module CApiObjectSpecs::Extend
        def reach
          :extended
        end
      end

      obj = mock("extended object")
      @o.rb_extend_object(obj, CApiObjectSpecs::Extend)
      obj.reach.should == :extended
    end
  end

  describe "OBJ_TAINT" do
    it "taints the object" do
      obj = mock("tainted")
      @o.OBJ_TAINT(obj)
      obj.tainted?.should be_true
    end
  end

  describe "OBJ_TAINTED" do
    it "returns C true if the object is tainted" do
      obj = mock("tainted")
      obj.taint
      @o.OBJ_TAINTED(obj).should be_true
    end

    it "returns C false if the object is not tainted" do
      obj = mock("untainted")
      @o.OBJ_TAINTED(obj).should be_false
    end
  end

  describe "rb_obj_freeze" do
    it "freezes the object passed to it" do
      obj = ""
      @o.rb_obj_freeze(obj).should == obj
      obj.frozen?.should be_true
    end
  end

  describe "rb_obj_frozen_p" do
    it "returns true if object passed to it is frozen" do
      obj = ""
      obj.freeze
      @o.rb_obj_frozen_p(obj).should == true
    end

    it "returns false if object passed to it is not frozen" do
      obj = ""
      @o.rb_obj_frozen_p(obj).should == false
    end
  end

  describe "rb_obj_taint" do 
    it "marks the object passed as tainted" do
      obj = ""
      obj.tainted?.should == false
      @o.rb_obj_taint(obj)
      obj.tainted?.should == true
    end

    it "raises an error if the object passed is frozen" do
      obj = ""
      obj.freeze
      lambda { @o.rb_obj_taint(obj) }.should raise_error(TypeError)
    end
  end

  describe "rb_check_frozen" do 
    it "raises an error if the obj is frozen" do 
      obj = ""
      obj.freeze
      lambda { @o.rb_check_frozen(obj) }.should raise_error(TypeError)
    end

    it "does nothing when object isn't frozen" do 
      obj = ""
      lambda { @o.rb_check_frozen(obj) }.should_not raise_error(TypeError)
    end
  end

  describe "rb_any_to_s" do
    it "converts obj to string" do 
      obj = 1
      i = @o.rb_any_to_s(obj)
      i.should be_kind_of(String)
    end
  end
end
