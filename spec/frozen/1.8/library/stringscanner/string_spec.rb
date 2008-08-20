require File.dirname(__FILE__) + '/../../spec_helper'
require 'strscan'

describe "StringScanner#string" do
  before :each do
    @s = StringScanner.new("This is a test")
  end

  it "returns the string being scanned" do
    @s.string.should == "This is a test"
    @s << " case"
    @s.string.should == "This is a test case"
  end
end

describe "StringScanner#string=" do
  before :each do
    @s = StringScanner.new("This is a test")
  end

  it "changes the string being scanned to the argument and resets the scanner" do
    @s.string = "Hello world"
    @s.string.should == "Hello world"
  end
end
