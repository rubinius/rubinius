require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "StringIO#string" do
  it "returns the underlying string" do
    str = "hello"
    @io = StringIO.new(str)
    @io.string.should == str
    @io.string.object_id.should == str.object_id
  end
end

describe "StringIO#string=" do
  before(:each) do
    @io = StringIO.new("example\nstring")
  end

  it "changes the underlying string" do
    str = "hello"
    @io.string = str
    @io.string.should == str
    @io.string.object_id.should == str.object_id
  end

  it "resets the position" do
    @io.read(1)
    @io.pos.should == 1
    @io.string = "other"
    @io.pos.should == 0
    @io.read(1).should == 'o'
  end

  it "resets the line number" do
    @io.gets
    @io.lineno.should == 1
    @io.string = "other"
    @io.lineno.should == 0
    @io.gets.should == "other"
  end
end
