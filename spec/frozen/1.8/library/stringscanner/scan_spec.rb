require File.dirname(__FILE__) + '/../../spec_helper'
require 'strscan'

describe "StringScanner#scan" do
  before :each do
    @s = StringScanner.new("This is a test")
  end

  it "returns the matched string" do
    @s.scan(/\w+/).should == "This"
    @s.scan(/.../).should == " is"
    @s.scan(//).should == ""
    @s.scan(/\s+/).should == " "
  end

  it "returns nil if there's no match" do
    @s.scan(/\d/).should == nil
  end

  it "raises a TypeError if pattern isn't a Regexp and can't be converted to a String" do
    lambda { @s.scan(5)         }.should raise_error(TypeError)
    lambda { @s.scan(:test)     }.should raise_error(TypeError)
    lambda { @s.scan(mock('x')) }.should raise_error(TypeError)
  end
end
