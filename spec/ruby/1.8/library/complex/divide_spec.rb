require File.dirname(__FILE__) + '/../../spec_helper'
require 'complex'

describe "Complex#/ with [Complex]" do
  it "needs to be reviewed for spec completeness" do
  end
  
  it "divides according to the usual rule for complex numbers" do
    a = 1
    b = 2
    c = 10
    d = 20
    (Complex((a * c) - (b * d), (a * d) + (b * c)) / Complex(a, b)).should == Complex(c, d)
    
    e = 1.5
    f = 2.1
    g = 100.2
    h = -30.3
    (Complex((e * g) - (f * h), (e * h) + (f * g)) / Complex(e, f)).should be_close(Complex(g, h), TOLERANCE) # remember the floating-point arithmetic
  end
end

describe "Complex#/ with [real]" do
  it "needs to be reviewed for spec completeness" do
  end
  
  it "divides both parts of the complex number by the real number" do
    (Complex(20, 40) / 2).should == Complex(10, 20)
    (Complex(15, 16) / 2.0).should be_close(Complex(7.5, 8), TOLERANCE)
  end
end