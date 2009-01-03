require File.dirname(__FILE__) + '/../../spec_helper'
require 'complex'

describe "Complex#hash" do
  # NOTE:
  # Complex(2, 1).hash == Complex(1, 2).hash
  # This can result in Bugs when using hash and
  # other stuff that relies on #hash
  it "should be static" do
    Complex(1).hash.should    == Complex(1).hash
    Complex(1, 0).hash.should == Complex(1).hash
    Complex(1, 1).hash.should == Complex(1, 1).hash
    
    Complex(1, 2).hash.should_not == Complex(1, 1).hash
    Complex(2, 1).hash.should_not == Complex(1, 1).hash
  end
end
