require File.dirname(__FILE__) + '/../../spec_helper'
require 'complex'

describe "Complex#to_s" do
  it "needs to be reviewed for spec completeness" do
  end
  
  specify "if real component is 0, just return imaginary component" do
    Complex(0, 5).to_s.should == "5i"
    Complex(0.0, -3.2).to_s.should == "-3.2i"
  end
  
  specify "if real component is nonzero and imaginary component is nonnegative, return 'a+bi'" do
    Complex(1, 5).to_s.should == "1+5i"
    Complex(1, 0.0).to_s.should == "1+0.0i" # at least, that's how MRI does it, weird as it seems
    Complex(1, 0).to_s.should == "1+0i"
    Complex(-2.5, 1.5).to_s.should == "-2.5+1.5i"
  end
  
  specify "if real component is nonzero and imaginary component is negative, return 'a-bi'" do
    Complex(1, -5).to_s.should == "1-5i"
    Complex(-2.5, -1.5).to_s.should == "-2.5-1.5i"
  end
end
