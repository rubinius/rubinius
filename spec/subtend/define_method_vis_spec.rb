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
  before :each do
    @c = SubtendMethodVisModuleTest.new
  end

  it "should have singleton method smv_test_singleton" do
    m = SubtendMethodVisModule.singleton_methods
    m.include?("smv_test_singleton").should == true
    m.include?("smv_test_module_function").should == true
  end

  it "should inherit method function from mixin" do
    @c.test_mf.should == "test"
  end

  it "should not inherit singleton method from mixin" do
    lambda { @c.test_s }.should raise_error(NameError)
  end

end

describe "SubtendMethodVis" do
  before :each do
    @c = SubtendMethodVis.new
  end

  it "should have public method smv_test_public" do
    m = @c.methods
    m.include?("smv_test_public").should == true
    m.include?("smv_test_protected").should == true  # note this is true..?
    m.include?("smv_test_private").should == false
  end
  
  it "should have protected method smv_test_protected" do
    m = @c.protected_methods
    m.include?("smv_test_public").should == false
    m.include?("smv_test_protected").should == true
    m.include?("smv_test_private").should == false
  end
  
  it "should have private method smv_test_private" do
    m = @c.private_methods
    m.include?("smv_test_public").should == false
    m.include?("smv_test_protected").should == false
    m.include?("smv_test_private").should == true
  end  
  
  it "should have global method smv_test_global_function" do
    Kernel.methods.include?("smv_test_global_function").should == true
  end
end
