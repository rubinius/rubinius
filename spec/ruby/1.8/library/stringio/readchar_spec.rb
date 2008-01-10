require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "StringIO#readchar" do
  before(:each) do
    @io = StringIO.new('abc')
  end

  it "returns character codes one at a time" do
    @io.readchar.should == ?a
    @io.readchar.should == ?b
    @io.readchar.should == ?c
  end

  it "raises an EOFError at the end of the string" do
    3.times { @io.readchar }
    lambda { @io.readchar }.should raise_error(EOFError)
  end
end
