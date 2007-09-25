require File.dirname(__FILE__) + '/../../spec_helper'

describe "Proc#arity" do
  it "returns the number of arguments that are not ignored by self" do
    Proc.new { || }.arity.should == 0
    Proc.new { |a| }.arity.should == 1
    Proc.new { |_| }.arity.should == 1
    Proc.new { |a, b| }.arity.should == 2
    Proc.new { |a, b, c| }.arity.should == 3
  end
  
  it "returns the negative number of mandatory arguments - 1 if self does take optional arguments" do
    Proc.new { |*a| }.arity.should == -1
    Proc.new { |a, *b| }.arity.should == -2
    Proc.new { |a, b, *c| }.arity.should == -3
  end

  it "returns -1 if no argument declaration is made" do
    Proc.new { }.arity.should == -1
  end
end