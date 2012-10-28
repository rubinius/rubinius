# -*- encoding: utf-8 -*-
require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

ruby_version_is "1.9" do
  describe "StringIO#ungetbyte" do
    before :each do
      @io = StringIO.new("abcdef")
    end

    it "returns nil" do
      @io.ungetbyte(65).should be_nil
    end

    it "prepends the byte to the data before data is read" do
      @io.ungetbyte(65)
      @io.read(2).should == "Aa"
    end

    it "preserves the prepended bytes when #rewind is called" do
      @io.ungetbyte(65)
      @io.ungetbyte(66)
      @io.rewind
      @io.read.should == "BAabcdef"
    end

    it "prepends data to the byte at the current position" do
      @io.read(3).should == "abc"
      @io.ungetbyte(65)
      @io.read(2).should == "Ad"
    end

    it "overwrites bytes in the data" do
      @io.read(3).should == "abc"
      @io.ungetbyte(66)
      @io.ungetbyte(65)
      @io.rewind
      @io.read.should == "aABdef"
    end

    it "raises an IOError when the mode is not readable" do
      lambda { StringIO.new("", "w").ungetbyte(42) }.should raise_error(IOError)
    end

    it "raises an IOError when read is closed" do
      @io.read
      @io.close_read
      lambda { @io.ungetbyte(42) }.should raise_error(IOError)
    end

    with_feature :encoding do
      it "does not change the encoding of the data" do
        @io.ungetbyte(0xff)
        result = @io.read
        result.should == "\xffabcdef"
        result.encoding.should == Encoding::UTF_8
      end
    end
  end
end
