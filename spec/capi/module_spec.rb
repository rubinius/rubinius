require File.dirname(__FILE__) + '/spec_helper'

load_extension('module')

describe "CApiModule" do

  module SubtendModuleTest
    BAR = 7
  end

  before :each do
    @m = CApiModuleSpecs.new
  end

  it "rb_const_set should set a constant on a module" do
    @m.rb_const_set(SubtendModuleTest, 7)
    SubtendModuleTest::FOO.should == 7
    (SubtendModuleTest::FOO != 5).should == true
  end

  it "rb_define_global_const should define a constant on Object" do
    @m.rb_define_global_const(7)
    ::GC_FOO_TEST.should == 7
    Object.send :remove_const, "GC_FOO_TEST"
  end

  it "rb_define_const should define a constant on a module" do
    @m.rb_define_const(SubtendModuleTest, 7)
    SubtendModuleTest::FOO.should == 7
    (SubtendModuleTest::FOO != 5).should == true
  end

  it "rb_const_defined should return true if a constant is defined" do
    @m.rb_const_defined(SubtendModuleTest, :BAR).should == true
  end

  it "rb_const_defined should return true if a constant is defined in Object" do
    @m.rb_const_defined(SubtendModuleTest, :Module).should == true
  end

  it "rb_const_get should lookup a constant" do
    @m.rb_const_get(SubtendModuleTest, :BAR).should == 7
  end

  it "rb_const_get should lookup a constant at toplevel" do
    @m.rb_const_get(SubtendModuleTest, :Fixnum).should == Fixnum
  end

end
