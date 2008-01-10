require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "StringIO#read" do
  before(:each) do
    @io = StringIOSpecs.build
  end

  it "reads at most 'length' bytes" do
    @io.read(666).should == @io.string
  end

  it "reads to the end of the string if length is omitted" do
    @io.read.should == @io.string
    @io.read.should == ""
    @io.read(nil).should == ""
    @io.read(50).should == nil
  end

  it "only supports String buffers" do
    lambda { @io.read(5, []) }.should raise_error(TypeError)
    @io.pos.should == 0
  end

  it "reads data into a buffer string if provided" do
    @io = StringIO.new('buffered')
    buf = ""
    @io.read(5, buf).object_id.should == buf.object_id
    buf.should == "buffe"
    @io.read(1, buf)
    buf.should == 'r'
  end
end
