require File.expand_path('../../../spec_helper', __FILE__)
require 'prime'

describe "Prime.instance" do
  it "returns an object representing the set of prime numbers" do
    Prime.instance.should be_kind_of(Prime)
  end

  it "returns an object with no obsolete features" do
    Prime.instance.should_not be_kind_of(Prime::OldCompatibility)
    Prime.instance.should_not respond_to(:succ)
    Prime.instance.should_not respond_to(:next)
  end

  it "does not complain anything" do
    lambda { Prime.instance }.should_not complain
  end

  it "raises an ArgumentError when is called with some arguments" do
    lambda { Prime.instance(1) }.should raise_error(ArgumentError)
  end
end
