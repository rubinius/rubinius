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

  it "rb_define_const should define a constant on a module" do
    @m.rb_define_const(SubtendModuleTest, 7)
    SubtendModuleTest::FOO.should == 7
    (SubtendModuleTest::FOO != 5).should == true
  end

  it "rb_const_defined should return true if a constant is defined" do
    @m.rb_const_defined(SubtendModuleTest, :BAR).should == true
  end

end
