require File.dirname(__FILE__) + '/../../spec_helper'
require 'complex'

describe "Complex#+ with [Complex]" do
  it "needs to be reviewed for spec completeness" do
  end
  
  it "adds both the real and imaginary components" do
    a = 1
    b = 2
    c = 10
    d = 20
    (Complex(a, b) + Complex(c, d)).should == Complex(a + c, b + d)
    
    e = 1.5
    f = 2.1
    g = 100.2
    h = -30.3
    (Complex(e, f) + Complex(g, h)).should == Complex(e + g, f + h)
  end
end

describe "Complex#+ with [real]" do
  it "needs to be reviewed for spec completeness" do
  end
  
  it "adds the real number to the real part of the complex number" do
    (Complex(1, 2) + 50).should == Complex(51, 2)
    (Complex(1, 2) + 50.5).should == Complex(51.5, 2)
  end
end