require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/subtend_helper'

compile_extension('subtend_cvar_defined')
require File.dirname(__FILE__) + '/ext/subtend_cvar_defined'

describe "SubtendCVarDefined" do
  before :each do
    @s = SubtendCVarDefined.new
  end

  class CVarTest
    @@class_var = 1
  
    def initialize
      @instance_var = 2
    end
  end
  

  it "rb_cvar_defined should return false when variable is not defined" do
    @s.rb_cvar_defined(CVarTest, "@@not_class_var").should_not == true
  end

  it "rb_cvar_defined should return true when variable is defined" do
    @s.rb_cvar_defined(CVarTest, "@@class_var").should == true
  end

  it "rb_cvar_defined should ignore instance variables" do
    @s.rb_cvar_defined(CVarTest, "@instance_var").should_not == true
  end
end
