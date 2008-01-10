require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "StringIO#sysread" do
  before(:each) do
    @io = StringIOSpecs.build
  end

  it "reads at most 'length' bytes" do
    @io.sysread(666).should == @io.string
  end

  it "reads to the end of the string if length is omitted" do
    @io.sysread.should == @io.string
  end

  it "raises an EOFError after the end of the string" do
    @io.sysread
    lambda { @io.sysread    }.should raise_error(EOFError)
    lambda { @io.sysread(5) }.should raise_error(EOFError)
  end

  it "only supports String buffers" do
    lambda { @io.sysread(5, []) }.should raise_error(TypeError)
    @io.pos.should == 0
  end

  it "reads data into a buffer string if provided" do
    @io = StringIO.new('buffered')
    buf = ""
    @io.sysread(5, buf).object_id.should == buf.object_id
    buf.should == "buffe"
  end
end
