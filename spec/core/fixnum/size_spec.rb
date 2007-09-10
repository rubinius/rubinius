require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#size" do
  it "is provided" do
    1.respond_to?(:size).should == true
  end
  
  noncompliant :rubinius do
    it "returns the number of bytes in the machine representation of self" do
      -1.size.should == 4
      0.size.should == 4
      4091.size.should == 4
    end
  end
end
