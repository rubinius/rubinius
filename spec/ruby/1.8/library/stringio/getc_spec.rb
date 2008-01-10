require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "StringIO#getc" do
  before(:each) do
    @io = StringIO.new("getc")
  end

  it "returns the next character code" do
    @io.getc.should == ?g
    @io.getc.should == ?e
    @io.getc.should == ?t
    @io.getc.should == ?c
    @io.getc.should == nil
    @io.getc.should == nil
  end
end
