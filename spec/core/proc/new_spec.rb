require File.dirname(__FILE__) + '/../../spec_helper'

describe "Proc.new" do
  it "returns a new Proc instance from the passed CompiledMethod instance" do
    Proc.new(def my_meth(); end).call.should == nil
    Proc.new(def my_meth(); 'hello'; end).call.should == 'hello'
  end
end
