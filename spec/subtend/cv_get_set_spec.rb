require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/subtend_helper'

compile_extension('subtend_cv_get_set')
require File.dirname(__FILE__) + '/ext/subtend_cv_get_set'

describe "SubtendCVGetSet" do
  before :each do
    @s = SubtendCVGetSet.new
  end

  class CVTest
  end

#  Actually, this causes a segfault on MRI
#  it "rb_cv_get should return nil when there's no class variable" do
#    @s.rb_cv_get(CVTest, "@@no_variable").should == nil
#    @s.rb_cvar_get(CVTest, "@@no_variable").should == nil
#  end

  it "rb_cv_set should set a class variable" do
    @s.rb_cv_set(CVTest, "@@class_variable_1", 1).should == 1
    @s.rb_cvar_set(CVTest, "@@class_variable_2", 2).should == 2
  end

  it "rb_cv_get should get a class variable" do
    @s.rb_cv_set(CVTest, "@@class_variable_3", 3).should == 3
    @s.rb_cv_get(CVTest, "@@class_variable_3").should == 3
    @s.rb_cvar_get(CVTest, "@@class_variable_3").should == 3
  end
  
  it "rb_cv_set should allow changing class variable" do
    @s.rb_cv_set(CVTest, "@@class_variable_4", 4).should == 4
    @s.rb_cv_get(CVTest, "@@class_variable_4").should == 4
    @s.rb_cv_set(CVTest, "@@class_variable_4", 5).should == 5
    @s.rb_cv_get(CVTest, "@@class_variable_4").should == 5
    @s.rb_cvar_set(CVTest, "@@class_variable_4", 6).should == 6
    @s.rb_cv_get(CVTest, "@@class_variable_4").should == 6
  end
end
