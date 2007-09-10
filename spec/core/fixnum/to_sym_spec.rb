require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#to_sym" do
  it "returns the symbol whose integer value is self" do
    a = :fred.to_i
    b = :wilma.to_i
    c = :bambam.to_i
    a.to_sym.should == :fred
    b.to_sym.should == :wilma
    c.to_sym.should == :bambam
  end  
end
