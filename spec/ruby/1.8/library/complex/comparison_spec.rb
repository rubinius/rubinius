require File.dirname(__FILE__) + '/../../spec_helper'
require 'complex'

describe "Complex#<=>" do
  it "needs to be reviewed for spec completeness" do
  end
  
  it "compares the absolute values of the two arguments" do
    (Complex(1, 2) <=> Complex(2, 1)).should == 0
    (Complex(-3, -10) <=> Complex(2, 1)).should > 0
    (Complex(3, 5) <=> Complex(100.0, -190.5)).should < 0
  end
end
