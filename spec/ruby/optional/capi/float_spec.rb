require File.expand_path('../spec_helper', __FILE__)

load_extension("float")

describe "CApiFloatSpecs" do
  before :each do
    @f = CApiFloatSpecs.new
  end

  describe "rb_float_new" do
    it "should create a new float" do
      ((@f.new_zero - 0).abs < 0.000001).should == true
      ((@f.new_point_five - 0.555).abs < 0.000001).should == true
    end
  end

  describe "RFLOAT" do
    it "returns a struct with the Float value" do
      f = @f.RFLOAT_value(1.0)
      f.should eql(1.0)

      f = @f.RFLOAT_value(-10.5)
      f.should eql(-10.5)
    end

    it "allows changing the Float value" do
      f = 3.14
      @f.RFLOAT_value_set(f, 4)
      f.should eql(4.0)
    end
  end

  describe "rb_Float" do
    it "should create a new Float from a String" do
      f = @f.rb_Float("101.99")
      f.should be_kind_of(Float)
      f.should eql(101.99)
    end
  end
end
