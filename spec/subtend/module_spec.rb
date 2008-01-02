require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/subtend_helper'

compile_extension('subtend_module')
require File.dirname(__FILE__) + '/ext/subtend_module'

context "Module" do

  module SubtendModuleTest
    BAR = 7
  end

  setup do
    @m = SubtendModule.new
  end
  
  specify "rb_define_const should define a constant on a module" do
    @m.rb_define_const(SubtendModuleTest, 7)
    SubtendModuleTest::FOO.should == 7
    (SubtendModuleTest::FOO != 5).should == true
  end
  
  specify "rb_const_defined should return true if a constant is defined" do
    @m.rb_const_defined(SubtendModuleTest, :BAR).should == true
  end
 
end
