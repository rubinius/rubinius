require File.dirname(__FILE__) + '/../../spec_helper'
require 'prime'

describe "Prime.new" do
  it "returns a new object representing the set of prime numbers" do
    Prime.new.class.should == Prime
  end

  it "returns a object with obsolete featrues" do
    Prime.new.should be_kind_of(Prime::OldCompatibility)
    Prime.new.should respond_to(:succ)
    Prime.new.should respond_to(:next)
  end

  it "complains that the method is obsolete" do
    lambda { Prime.new }.should complain(/obsolete.*use.*Prime::instance/)
  end
   
  it "raises a ArgumentError when is called with some arguments" do
    lambda { Prime.new(1) }.should raise_error(ArgumentError)
  end
end
