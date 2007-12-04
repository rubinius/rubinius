require File.dirname(__FILE__) + '/../../spec_helper'

describe "Float#/" do
  it "returns self divided by other" do
    (5.75 / -2).should be_close(-2.875,TOLERANCE)
    (451.0 / 9.3).should be_close(48.494623655914,TOLERANCE)
    (91.1 / -0xffffffff).should be_close(-2.12108716418061e-08, TOLERANCE)
  end
  
  it "does NOT raise ZeroDivisionError if other is zero" do
    (1.0 / 0.0).to_s.should == 'Infinity'
    (-1.0 / 0.0).to_s.should == '-Infinity'
    (1.0 / -0.0).to_s.should == '-Infinity'
    (-1.0 / -0.0).to_s.should == 'Infinity'
    (0.0 / 0.0).to_s.should == 'NaN'
    (-0.0 / 0.0).to_s.should == 'NaN'
    (-0.0 / -0.0).to_s.should == 'NaN'
  end
end
