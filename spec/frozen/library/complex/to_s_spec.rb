require File.dirname(__FILE__) + '/../../spec_helper'
require 'complex'

describe "Complex#to_s when self's real component is 0" do
  it "returns only the imaginary component as String" do
    Complex(0, 5).to_s.should == "5i"
    Complex(0, -3.2).to_s.should == "-3.2i"
  end
end

describe "Complex#to_s" do
  it "returns self as String" do
    Complex(1, 5).to_s.should == "1+5i"
    Complex(-2.5, 1.5).to_s.should == "-2.5+1.5i"
    
    Complex(1, -5).to_s.should == "1-5i"
    Complex(-2.5, -1.5).to_s.should == "-2.5-1.5i"

    # Guard against the Mathn library
    conflicts_with :Prime do
      Complex(1, 0).to_s.should == "1+0i"
      Complex(1, -0).to_s.should == "1+0i"

      # This is a bit weird, but it's what MRI does
      Complex(1, 0.0).to_s.should == "1+0.0i"
      Complex(1, -0.0).to_s.should == "1+0.0i"
    end
  end
end