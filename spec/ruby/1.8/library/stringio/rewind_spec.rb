require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "StringIO#rewind" do
  before(:each) do
    @io = StringIO.new("hello\nworld")
  end

  it "resets the position" do
    @io.gets
    @io.pos.should == 6
    @io.rewind
    @io.pos.should == 0
  end

  it "resets the line number" do
    @io.gets
    @io.lineno.should == 1
    @io.rewind
    @io.lineno.should == 0
  end

  it "should make the contents of the stream accessible again when stream was read beyond its end" do
    str = @io.string

    @io.read(@io.string.length + 1).should == str
    @io.rewind
    @io.read(@io.string.length + 1).should == str
  end
end
