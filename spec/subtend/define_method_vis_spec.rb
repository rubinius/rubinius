require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/subtend_helper'

compile_extension('subtend_define_method_vis')
require File.dirname(__FILE__) + '/ext/subtend_define_method_vis'

class SubtendMethodVisModuleTest
  include SubtendMethodVisModule

  def test_mf
    smv_test_module_function
  end

  def test_s
    smb_test_singleton
  end  

end

describe "SubtendMethodVisModule" do
  setup do
    @c = SubtendMethodVisModuleTest.new
  end

  specify "should have singleton method smv_test_singleton" do
    m = SubtendMethodVisModule.singleton_methods
    m.include?("smv_test_singleton").should == true
    m.include?("smv_test_module_function").should == true
  end

  specify "should inherit method function from mixin" do
    @c.test_mf.should == "test"
  end

  specify "should not inherit singleton method from mixin" do
    lambda { @c.test_s }.should raise_error(NameError)
  end

end

describe "SubtendMethodVis" do
  setup do
    @c = SubtendMethodVis.new
  end

  specify "should have public method smv_test_public" do
    m = @c.methods
    m.include?("smv_test_public").should == true
    m.include?("smv_test_protected").should == true  # note this is true..?
    m.include?("smv_test_private").should == false
  end

  specify "should have protected method smv_test_protected" do
    m = @c.protected_methods
    m.include?("smv_test_public").should == false
    m.include?("smv_test_protected").should == true
    m.include?("smv_test_private").should == false
  end

  specify "should have private method smb_test_private" do
    m = @c.private_methods
    m.include?("smv_test_public").should == false
    m.include?("smv_test_protected").should == false
    m.include?("smv_test_private").should == true
  end

end
