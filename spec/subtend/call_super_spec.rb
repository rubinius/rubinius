require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/subtend_helper'

compile_extension('subtend_call_super')
require File.dirname(__FILE__) + '/ext/subtend_call_super'

describe "SubtendCallSuper" do
  before :each do
    @s = SubtendCallSuper.new
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
