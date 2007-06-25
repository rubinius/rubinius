require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/subtend_helper'

extension :rubinius do
  compile_extension('subtend_class')
  require File.dirname(__FILE__) + '/ext/subtend_class'

  class SubtendClassTest
    attr_reader :foo

    def initialize(v)
      @foo = v
    end
  end

  context "SubtendClass" do
    setup do
      @s = SubtendClass.new
    end

    specify "rb_obj_alloc should allocate a new uninitialized object" do
      o = @s.rb_obj_alloc(SubtendClassTest)
      o.class.should == SubtendClassTest
      o.foo.should == nil
    end

    specify "rb_obj_call_init should send #initialize" do
      o = @s.rb_obj_alloc(SubtendClassTest)
      @s.rb_obj_call_init(o, 1, [100])
      o.foo.should == 100
    end

    specify "rb_class_new_instance should allocate and initialize a new object" do
      o = @s.rb_class_new_instance(1, ["yo"], SubtendClassTest)
      o.class.should == SubtendClassTest
      o.foo.should == "yo"
    end
  end
end
