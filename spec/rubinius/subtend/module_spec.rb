require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/subtend_helper'

extension :rubinius do
  compile_extension('subtend_module')
  require File.dirname(__FILE__) + '/ext/subtend_module'
  
  context "Module" do
  
    module SubtendModuleTest
    end
  
    setup do
      @m = SubtendModule.new
    end
    
    specify "rb_define_const should define a constant on a module" do
      @m.rb_define_const(SubtendModuleTest, 7)
      SubtendModuleTest::FOO.should == 7
      (SubtendModuleTest::FOO != 5).should == true
    end
   
  end
end