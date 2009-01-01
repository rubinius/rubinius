require File.dirname(__FILE__) + '/../../spec_helper'

describe "Class#superclass" do
  it "returns the superclass of self" do
    Object.superclass.should == nil
    Class.superclass.should == Module
    Class.new.superclass.should == Object
    Class.new(String).superclass.should == String
    Class.new(Fixnum).superclass.should == Fixnum
  end
end