require File.dirname(__FILE__) + '/../../spec_helper'
require 'complex'

describe "Complex#% with [Complex]" do
  it "needs to be reviewed for spec completeness" do
  end

quarantine! do # second part of the test fails on MRI, RBX, JRuby!!!
  it "returns the remainder from complex division" do
  # Is this spec even correct? It doesn't work on MRI or rbx, at least not for the floating-point case.
    a = 1
    b = 2
    c = 10
    d = 20
    e = 3
    f = 4
    x = (Complex(a, b) * Complex(c, d)) + Complex(e, f)
    (x % Complex(c, d)).should == Complex(e, f)
    
    a = 1.5
    b = 2.1
    c = 100.2
    d = -30.3
    e = 0.3
    f = 0.2
    x = (Complex(a, b) * Complex(c, d)) + Complex(e, f)
    (x % Complex(c, d)).should be_close(Complex(e, f), TOLERANCE)
  end
end

end

describe "Complex#% with [real]" do
  it "needs to be reviewed for spec completeness" do
  end
  
  it "returns the remainder from dividing both parts of the complex number by the real number" do
    (Complex(21, 42) % 10).should == Complex(1, 2)
    (Complex(15.5, 16.5) % 2.0).should be_close(Complex(1.5, 0.5), TOLERANCE)
  end
end