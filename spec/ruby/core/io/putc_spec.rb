# -*- encoding: ascii-8bit -*-
require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "IO#putc" do
  before :each do
    @name = tmp "io_putc"
    @io = File.open @name, fmode("w:binary")
    @io.sync = true
  end

  after :each do
    @io.close
    rm_r @name
  end

  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_io.putc('a') }.should raise_error(IOError)
  end

  it "writes the first byte of a String" do
    @io.putc "foo"
    @name.should have_data("f")
  end

  it "calls #to_int on a non-Integer and writes the first byte of the value" do
    int = mock("integer 261")
    int.should_receive(:to_int).and_return(261)

    @io.putc int

    # \005 is the least significant byte of 261
    @name.should have_data("\005")
  end

  it "writes a Numeric that fits in a byte" do
    @io.putc(-128)
    @io.putc 0
    @io.putc 255

    @name.should have_data("\200\000\377")
  end

  it "write the first byte of a Numeric that does not fit in a byte" do
    @io.putc(-129)
    @io.putc 256

    @name.should have_data("\177\000")
  end
end
