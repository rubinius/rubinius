require File.dirname(__FILE__) + '/../../spec_helper'
require 'pathname'

describe "Pathname.new" do
  it "returns a new Pathname Object with 1 argument" do
    Pathname.new('').class.should == Pathname
  end

  it "raises an ArgumentError when called with \0" do
    lambda { Pathname.new("\0")}.should raise_error(ArgumentError)
  end

  it "is tainted if path is tainted" do
    path = '/usr/local/bin'.taint
    Pathname.new(path).tainted?.should == true
  end

end

