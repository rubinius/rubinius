require File.dirname(__FILE__) + '/../../spec_helper'

describe "Proc#==" do
  it "returns true when the given value is self or a copy of self" do
    a = lambda { "hello" }
    
    a.should == a
    a.should == a.dup
    
    a.should_not == lambda { "hello" }
    a.should_not == Proc.new {}
    a.should_not == nil
  end
end