require File.dirname(__FILE__) + '/../../spec_helper'

describe "Proc#arity" do
  it "returns the number of arguments, using Proc.new with a CompiledMethod param" do
    Proc.new(def meth; end).arity.should == 0
    Proc.new(def meth(a); end).arity.should == 1
    Proc.new(def meth(_); end).arity.should == 1
    Proc.new(def meth(a,b); end).arity.should == 2
    Proc.new(def meth(a,b,c); end).arity.should == 3
  end

  it "if optional arguments, return the negative number of mandatory arguments using Proc.new with a CompiledMethod param" do
    Proc.new(def meth(*a); end).arity.should == -1
    Proc.new(def meth(a,*b); end).arity.should == -2
    Proc.new(def meth(a,b,*c); end).arity.should == -3
  end

  it "ignore block arguments in arguments count" do
    Proc.new(def meth(a,b,&c); end).arity.should == 2
  end
end
