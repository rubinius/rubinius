require File.dirname(__FILE__) + '/../../spec_helper'
require 'complex'

describe "Complex#* with [Complex]" do
  it "needs to be reviewed for spec completeness" do
  end
  
  it "multiplies according to the usual rule for complex numbers: (a + bi) * (c + di) = ac - bd + (ad + bc)i" do
    a = 1
    b = 2
    c = 10
    d = 20
    (Complex(a, b) * Complex(c, d)).should == Complex((a * c) - (b * d), (a * d) + (b * c))
    
    e = 1.5
    f = 2.1
    g = 100.2
    h = -30.3
    (Complex(e, f) * Complex(g, h)).should == Complex((e * g) - (f * h), (e * h) + (f * g))
  end
end

describe "Complex#* with [real]" do
  it "needs to be reviewed for spec completeness" do
  end
  
  it "multiplies both parts of the complex number by the real number" do
    (Complex(3, 2) * 50).should == Complex(150, 100)
    (Complex(-3, 2) * 50.5).should == Complex(-151.5, 101)
  end
end