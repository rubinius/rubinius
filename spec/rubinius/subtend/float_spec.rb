require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/subtend_helper'

compile_extension('subtend_float')
require File.dirname(__FILE__) + '/ext/subtend_float'

context "SubtendFloat" do
  setup do
    @f = SubtendFloat.new
  end
  
  specify "rb_float_new should create a new float" do
    ((@f.sf_new_zero - 0).abs < 0.000001).should == true
    ((@f.sf_new_point_five - 0.555).abs < 0.000001).should == true
  end
end
