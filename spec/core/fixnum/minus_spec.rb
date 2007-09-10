require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#-" do
  it "returns self minus other fixnum" do
    (5 - 10).should == -5
    (9237212 - 5_280).should == 9231932
  end
  
  it "coerces fixnum and return self minus other fixnum" do
    (2_560_496 - 0xfffffffff).should == -68716916239
    (781 - 0.5).to_s.should == '780.5'
  end
end
