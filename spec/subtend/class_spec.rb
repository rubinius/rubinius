require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/subtend_helper'

compile_extension('subtend_class')
require File.dirname(__FILE__) + '/ext/subtend_class'

module SubtendModuleTest
  def im_included
    "YEP"
  end
end

class SubtendClassTest
  attr_reader :foo

  def initialize(v)
    @foo = v
  end
end

describe "SubtendClass" do
  setup do
    @s = SubtendClass.new
  end

  it "rb_obj_alloc should allocate a new uninitialized object" do
    o = @s.rb_obj_alloc(SubtendClassTest)
    o.class.should == SubtendClassTest
    o.foo.should == nil
  end

  it "rb_obj_call_init should send #initialize" do
    o = @s.rb_obj_alloc(SubtendClassTest)
#     @s.rb_obj_call_init(o, 1, [100])
#     o.foo.should == 100
  end

  it "rb_class_new_instance should allocate and initialize a new object" do
#     o = @s.rb_class_new_instance(1, ["yo"], SubtendClassTest)
#     o.class.should == SubtendClassTest
#     o.foo.should == "yo"
  end
  
  it "rb_include_module should include a module into a class" do
    SubtendClassTest.new(4).respond_to?(:im_included).should == false
    @s.rb_include_module(SubtendClassTest, SubtendModuleTest)
    SubtendClassTest.new(4).respond_to?(:im_included).should == true
    SubtendClassTest.new(4).im_included.should == "YEP"
  end
  
  it "rb_define_attr should be able to define attributes" do
    @s.rb_define_attr(SubtendClassTest, :bar, true, false)
    @s.rb_define_attr(SubtendClassTest, :baz, false, true)
    @s.rb_define_attr(SubtendClassTest, :bat, true, true)      
    s = SubtendClassTest.new(7)
    s.respond_to?(:bar).should == true
    s.respond_to?(:bar=).should == false
    s.respond_to?(:baz).should == false
    s.respond_to?(:baz=).should == true
    s.respond_to?(:bat).should == true
    s.respond_to?(:bat=).should == true      
  end
end
