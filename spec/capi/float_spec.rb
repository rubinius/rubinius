require File.dirname(__FILE__) + '/spec_helper'

load_extension("float")

describe "CApiFloatSpecs" do
  before :each do
    @f = CApiFloatSpecs.new
  end
  
  describe "rb_float_new" do
    it "should create a new float" do
      ((@f.sf_new_zero - 0).abs < 0.000001).should == true
      ((@f.sf_new_point_five - 0.555).abs < 0.000001).should == true
    end
  end

  describe "RFLOAT" do
    it "returns a struct with the Float value" do
      @f.RFLOAT_value(1.0).eql?(1.0).should == true
      @f.RFLOAT_value(-10.5).eql?(-10.5).should == true
    end

    it "allows changing the Float value" do
      f = 3.14
      @f.RFLOAT_value_set(f, 4)
      f.eql?(4.0).should == true
    end
  end
end
