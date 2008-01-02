require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/subtend_helper'

compile_extension('subtend_object')
require File.dirname(__FILE__) + '/ext/subtend_object'

context "SubtendObject" do
  
  setup do
    @o = SubtendObject.new
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
  
  specify "rb_is_instance_of should return true if an object is an instance" do
    @o.rb_obj_is_instance_of(ObjectTest.new, ObjectTest).should == true
    @o.rb_obj_is_instance_of(DescObjectTest.new, ObjectTest).should == false
  end 

  specify "rb_is_kind_of should return true if an object is an instance or descendent" do
    @o.rb_obj_is_kind_of(ObjectTest.new, ObjectTest).should == true
    @o.rb_obj_is_kind_of(DescObjectTest.new, ObjectTest).should == true
    @o.rb_obj_is_kind_of(Object.new, ObjectTest).should == false
  end 
  
  specify "rb_respond_to should return 1 if respond_to? is true and 0 if respond_to? is false" do
    @o.rb_respond_to(ObjectTest.new, :foo).should == true
    @o.rb_respond_to(ObjectTest.new, :bar).should == false
  end
  
  specify "rb_to_id should return a symbol representation of the object" do
    @o.rb_to_id("foo").should == :foo
    @o.rb_to_id(:foo).should == :foo
  end
  
  specify "rb_require should require a ruby file" do
    $foo.should == nil
    $:.unshift File.dirname(__FILE__)
    @o.rb_require()
    $foo.should == 7
  end
  
  specify "rb_attr_get should get an instance variable" do
    o = ObjectTest.new
    @o.rb_attr_get(o, :@foo).should == 7
  end
  
  specify "rb_check_array_type should try to coerce to array, otherwise return nil" do
    ac = AryChild.new
    ao = Array.new
    h = Hash.new
    @o.rb_check_array_type(ac).should == []
    @o.rb_check_array_type(ao).should == []
    @o.rb_check_array_type(h).should == nil
  end
  
  specify "rb_check_convert_type should try to coerce to a type, otherwise return nil" do    
    ac = AryChild.new
    ao = Array.new
    h = Hash.new
    # note that I force the ary information in the spec extension
    @o.rb_check_convert_type(ac).should == []
    @o.rb_check_convert_type(ao).should == []
    @o.rb_check_convert_type(h).should == nil
  end
  
  specify "rb_check_string_type should try to coerce to a string, otherwise return an empty string" do
    sc = "Hello"
    so = StrChild.new("Hello")
    h = {:hello => :goodbye}
    @o.rb_check_string_type(sc).should == "Hello"
    @o.rb_check_string_type(so).should == "Hello"
    @o.rb_check_string_type(h).should == ""
  end
  
end
