require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

describe "String#to_f" do
  it "treats leading characters of self as a floating point number" do
   "123.45e1".to_f.should == 1234.5
   "45.67 degrees".to_f.should == 45.67
   "0".to_f.should == 0.0
   "123.45e1".to_f.should == 1234.5
   
   ".5".to_f.should == 0.5
   ".5e1".to_f.should == 5.0
   
   "0b5".to_f.should == 0
   "0d5".to_f.should == 0
   "0o5".to_f.should == 0
   "0x5".to_f.should == 0
   
   "NaN".to_f.should == 0
   "Infinity".to_f.should == 0
   "-Infinity".to_f.should == 0
  end

  it "takes an optional sign" do
    "-45.67 degrees".to_f.should == -45.67
    "+45.67 degrees".to_f.should == 45.67
    "-5_5e-5_0".to_f.should == -55e-50
    "-".to_f.should == -0.0
    "-0".to_f.should == -0.0
  end
  
  it "returns 0.0 if the conversion fails" do
    "bad".to_f.should == 0.0
    "thx1138".to_f.should == 0.0
  end
end
