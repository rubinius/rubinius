require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "StringIO#readlines" do
  before(:each) do
    @io = StringIO.new("line1\nline2\nline3\n")
  end

  it "returns an array of lines" do
    @io.readlines.should == ["line1\n", "line2\n", "line3\n"]
  end

  it "raises an IOError when it is not open for reading" do
    @io.close_read
    lambda { @io.readlines }.should raise_error(IOError)
  end

  it "returns the rest of the stream when separator is nil" do
    @io.read(4)
    @io.readlines(nil).should == ["1\nline2\nline3\n"]
  end

  it "optionally accepts a separator string" do
    @io.readlines('line').should == ["line", "1\nline", "2\nline", "3\n"]
  end
end
