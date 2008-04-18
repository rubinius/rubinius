require File.dirname(__FILE__) + '/../../spec_helper'
require 'complex'

describe "Complex#- with [Complex]" do
  it "needs to be reviewed for spec completeness" do
  end
  
  it "subtracts both the real and imaginary components" do
    a = 1
    b = 2
    c = 10
    d = 20
    (Complex.new(a, b) - Complex.new(c, d)).should == Complex.new(a - c, b - d)
    
    e = 1.5
    f = 2.1
    g = 100.2
    h = -30.3
    (Complex.new(e, f) - Complex.new(g, h)).should == Complex.new(e - g, f - h)
  end
end

describe "Complex#- with [real]" do
  it "needs to be reviewed for spec completeness" do
  end
  
  it "subtracts the real number from the real part of the complex number" do
    (Complex.new(1, 2) - 50).should == Complex.new(-49, 2)
    (Complex.new(1, 2) - 50.5).should == Complex.new(-49.5, 2)
  end
end