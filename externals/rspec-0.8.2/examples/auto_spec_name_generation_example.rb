require File.dirname(__FILE__) + '/spec_helper'

# Run spec w/ -fs to see the output of this file

context "specs with no names" do
  
  # spec name is auto-generated as "should equal(5)"
  specify do
    3.should equal(3)
    5.should equal(5)
  end
  
  specify { 3.should be < 5 }
  
  specify { ["a"].should include("a") }
  
  specify { [1,2,3].should respond_to(:size) }
end