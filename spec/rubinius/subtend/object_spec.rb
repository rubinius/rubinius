require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/subtend_helper'

extension :rubinius do
  compile_extension('subtend_object')
  require File.dirname(__FILE__) + '/ext/subtend_object'
  
  context "SubtendObject" do
    
    setup do
      @o = SubtendObject.new
    end
    
    class ObjectTest
      def foo
      end
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
    
  end
end