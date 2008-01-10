require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "StringIO#<<" do
  it "appends to the string when in append mode" do
    @io = StringIO.new("example", 'a')
    (@io << "x").should == @io
    @io.string.should == "examplex"
  end

  it "writes to the string when in default mode" do
    @io = StringIO.new("example")
    (@io << "x").should == @io
    @io.string.should == "xxample"
  end
end
