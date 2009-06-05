# encoding: utf-8
require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#putc" do
  before :each do
    @filename = tmp "IO_putc_file_#{$$}"
    @io = File.open @filename, 'w'
    @io.sync = true
  end

  after :each do
    @io.close
    File.unlink @filename
  end

  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_file.putc('a') }.should raise_error(IOError)
  end

  it "writes the first byte of a String" do
    @io.putc "foo"
    File.read(@filename).should == 'f'
  end

  it "writes the first byte of an object's string representation" do
    o = Object.new
    def o.to_int() 261 end # \005

    @io.putc o
    File.read(@filename).should == "\005"
  end

  it "writes Numerics that fit in a C char" do
    @io.putc(-128)
    @io.putc 0
    @io.putc 255

    File.read(@filename).should == "\200\000\377"
  end

  it "write the first byte of Numerics that don't fit in a C char" do
    @io.putc(-129)
    @io.putc 256

    File.read(@filename).should == "\177\000"
  end

end
